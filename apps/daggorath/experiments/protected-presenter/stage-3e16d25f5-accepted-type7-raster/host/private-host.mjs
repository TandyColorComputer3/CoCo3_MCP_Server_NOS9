#!/usr/bin/env node
/* One-off private MAME host for Stage 3E.16D.25F.5. */
import fs from 'node:fs';
import fsp from 'node:fs/promises';
import crypto from 'node:crypto';
import http from 'node:http';
import net from 'node:net';
import path from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import { execFileSync } from 'node:child_process';
import { requalifyAfterMount } from './requalify.mjs';

const here = path.dirname(fileURLToPath(import.meta.url));
const cfgPath = process.argv[2];
if (!cfgPath) throw new Error('usage: private-host.mjs <private-config.json>');
const cfg = JSON.parse(await fsp.readFile(cfgPath, 'utf8'));
const repo = '/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9';
const mcp = path.join(repo, 'MCP');
const runRoot = path.resolve(cfg.runRoot);
if (!(runRoot === '/private/tmp/dag-3e16d25f5-phase-20261018/session' || runRoot.startsWith('/private/tmp/dag-3e16d25f5-phase-20261018/session/') || runRoot.startsWith('/private/tmp/dag-3e16d25f5-phase-20261018/session-')))
  throw new Error('runRoot escaped the authorized private workspace');
try { await fsp.access(runRoot); throw new Error('runRoot already exists; refusing stale session reuse'); }
catch (e) { if (e?.code !== 'ENOENT') throw e; }
await fsp.mkdir(runRoot, { recursive: true });

const sha = b => crypto.createHash('sha256').update(b).digest('hex');
const copyInput = async (label, source, dest, expected) => {
  const src = path.resolve(source), dst = path.resolve(dest);
  if (src === dst || !src.startsWith('/private/tmp/')) throw new Error(`${label}: source must be a distinct private input`);
  const bytes = await fsp.readFile(src);
  const digest = sha(bytes);
  if (digest !== expected) throw new Error(`${label}: source hash mismatch (${digest})`);
  await fsp.writeFile(dst, bytes, { flag: 'wx', mode: 0o600 });
  return { label, source: src, privateCopy: dst, size: bytes.length, sha256: digest };
};
const mediaDir = path.join(runRoot, 'media');
const stateDir = path.join(runRoot, 'states');
const cfgDir = path.join(runRoot, 'work', 'mame-cfg');
const scriptDir = path.join(runRoot, 'scripts');
const evidenceDir = path.join(runRoot, 'evidence');
await Promise.all([mediaDir, stateDir, cfgDir, scriptDir, evidenceDir].map(p => fsp.mkdir(p, { recursive: true })));
const inputs = [];
inputs.push(await copyInput('boot floppy', cfg.bootFloppySource,
  path.join(mediaDir, 'boot-flop1.dsk'), '9a51adb8656b9f003c7f822352c291487362f1b4c43aac1a16672d5d2bcb4c40'));
inputs.push(await copyInput('system VHD', cfg.vhdSource,
  path.join(mediaDir, 'system.vhd'), '4c6bdc0cd2f68aa57a9c75f75f15fe429f30926aa522917dd8d14aa1b9ae622e'));
inputs.push(await copyInput('fresh gfx7cal D1', cfg.testFloppySource,
  path.join(mediaDir, 'gfx7cal-flop2.dsk'), '0b06d72546472058ce2ddb8942cbadedde19a8b0f6108a3fa36b9e1269f92de5'));
inputs.push(await copyInput('fresh presenter D1', cfg.presenterD1Source,
  path.join(mediaDir, 'presenter-flop2.dsk'), cfg.presenterD1Sha256));

if (cfg.machine !== 'coco3h' || cfg.cpu !== 'HD6309E' || cfg.monitor !== 'rgb'
    || cfg.screenConfig !== 1 || cfg.ram !== '2M' || cfg.mameBuild !== 'ample-289-u2')
  throw new Error('private MAME config must be coco3h / HD6309E / RGB / screen_config=1 / 2M / ample-289-u2');
const mameVersion = execFileSync(cfg.mamePath, ['-version'], { encoding: 'utf8' }).trim();
if (mameVersion !== '0.289 (ample-289-u2)') throw new Error(`unexpected MAME: ${mameVersion}`);
const xml = execFileSync(cfg.mamePath, ['-listxml', 'coco3h'], { encoding: 'utf8', maxBuffer: 8 * 1024 * 1024 });
const cpuMatch = xml.match(/<device_ref\b[^>]*tag=":maincpu"[^>]*>/i);
if (!xml.includes('<machine name="coco3h"') || !cpuMatch || !/name="hd6309e"/i.test(cpuMatch[0]))
  throw new Error(`coco3h HD6309E identity not independently confirmed: ${cpuMatch?.[0] ?? 'no maincpu device ref'}`);

async function reservePort() {
  const server = net.createServer();
  await new Promise((resolve, reject) => server.once('error', reject).listen(0, '127.0.0.1', resolve));
  const port = server.address().port;
  await new Promise(resolve => server.close(resolve));
  return port;
}
const bridgePort = await reservePort();
let httpPort = await reservePort();
while (httpPort === bridgePort) httpPort = await reservePort();

const baseBridge = await fsp.readFile(path.join(mcp, 'scripts/bridge.lua'));
const observerPath = path.join(path.dirname(here), 'observer', 'observer.lua');
const observer = await fsp.readFile(observerPath);
const privateBridgePath = path.join(scriptDir, 'bridge.lua');
await fsp.writeFile(privateBridgePath,
  Buffer.concat([baseBridge, Buffer.from('\n-- Disposable Stage 3E.16D.25F.5 observer\n'), observer, Buffer.from(`\n-- Disposable D.25F.1 serializer qualification command\ncommands.type7_array_probe = function()\n  return { bandRGB = mark_array({\"000000\", \"555555\", \"AAAAAA\", \"FFFFFF\"}), paletteValues = mark_array({ 0x00, 0x07, 0x38, 0x3F }) }\nend\n`)]), { flag: 'wx', mode: 0o600 });

process.env.MSH_RUN_ROOT = runRoot;
const privateMcpDir = path.join(runRoot, 'private-mcp');
await fsp.mkdir(privateMcpDir, { recursive: true });
await fsp.writeFile(path.join(privateMcpDir, 'package.json'), '{\"type\":\"module\"}\n', { flag: 'wx', mode: 0o600 });
const productionBridgeServer = await fsp.readFile(path.join(mcp, 'dist', 'bridge-server.js'), 'utf8');
const importLine = 'import net from "node:net";';
const onDataLine = '            const line = buffer.slice(0, newline);';
if (!productionBridgeServer.includes(importLine) || !productionBridgeServer.includes(onDataLine))
  throw new Error('production bridge server source does not match the audited patch context');
const auditedBridgeServer = productionBridgeServer
  .replace(importLine, `${importLine}\nimport fs from "node:fs";`)
  .replace(onDataLine, `${onDataLine}\n            if (process.env.BRIDGE_RAW_LOG) fs.appendFileSync(process.env.BRIDGE_RAW_LOG, line + "\\n", "utf8");`);
await fsp.writeFile(path.join(privateMcpDir, 'bridge-server.js'), auditedBridgeServer, { flag: 'wx', mode: 0o600 });
await fsp.copyFile(path.join(mcp, 'dist', 'protocol.js'), path.join(privateMcpDir, 'protocol.js'));
process.env.BRIDGE_RAW_LOG = path.join(runRoot, 'evidence', 'bridge-responses.raw.jsonl');
const importMcp = async name => import(pathToFileURL(name === 'bridge-server'
  ? path.join(privateMcpDir, 'bridge-server.js') : path.join(mcp, 'dist', `${name}.js`)).href);
const [{ loadConfig, writeBridgePortFile, writeMonitorConfig }, { createBridgeServer },
  { createMameController }, { createToolHandlers }, { createToolchain }] = await Promise.all([
  importMcp('config'), importMcp('bridge-server'), importMcp('mame-process'),
  importMcp('tools'), importMcp('toolchain'),
]);
const { decodeConsole } = await importMcp('os9-ready');
const { rememberShell, forgetShell } = await importMcp('os9-run');
const config = loadConfig({
  MAME_PATH: cfg.mamePath, MAME_ROMPATH: cfg.romPath, MAME_MACHINE: 'coco3h',
  COCO_MONITOR: 'rgb', COCO_RAM: '2M',
  MAME_BOOT_FLOPPY: inputs[0].privateCopy, MAME_VHD: inputs[1].privateCopy,
  BRIDGE_PORT: String(bridgePort), NITROS9_READY_STATE: cfg.readyState,
}, runRoot);
if (config.mameCfgDir !== cfgDir || config.bridgeLuaPath !== privateBridgePath
    || config.cocoMonitor !== 'rgb' || config.cocoRam !== '2M' || config.mameMachine !== 'coco3h')
  throw new Error('resolved private config escaped or changed the required settings');
writeBridgePortFile(config);
const expectedReadyStateSha = 'f57eb8de7899adb62c2f5a436bd172ca50caaa167b70695ea7cd02afd06fc486';
if (cfg.readyStateSha256 !== expectedReadyStateSha || cfg.readyState !== 'gfx7cal_25e_ready'
    || config.nitros9ReadyState !== cfg.readyState || config.mameMachine !== 'coco3h')
  throw new Error('ready-state identity or machine mismatch');
const readyStateSource = path.resolve(cfg.readyStateSource);
if (readyStateSource !== path.join(path.dirname(here), 'inputs', 'gfx7cal_25e_ready.sta'))
  throw new Error('ready-state source escaped the private verified inputs');
const readyStateDestination = path.join(config.stateDir, config.mameMachine, `${config.nitros9ReadyState}.sta`);
if (readyStateDestination !== path.join(runRoot, 'states', 'coco3h', 'gfx7cal_25e_ready.sta'))
  throw new Error('ready-state destination differs from os9_restore_ready lookup');
await fsp.mkdir(path.dirname(readyStateDestination), { recursive: true });
const readyStateCopy = await copyInput('stable EOU-ready state', readyStateSource,
  readyStateDestination, expectedReadyStateSha);
if (readyStateCopy.size !== 143064) throw new Error('ready-state size mismatch');
await fsp.access(readyStateDestination, fs.constants.R_OK);
await fsp.writeFile(path.join(evidenceDir, 'ready-state-staging.json'), JSON.stringify({
  source: readyStateCopy.source, destination: readyStateCopy.privateCopy,
  sourceSha256: expectedReadyStateSha, destinationSha256: readyStateCopy.sha256,
  sourceSize: readyStateCopy.size, destinationSize: (await fsp.stat(readyStateDestination)).size,
  readableByPrivateHost: true, stateDir: config.stateDir, machine: config.mameMachine,
  stateName: config.nitros9ReadyState, expectedByOs9RestoreReady: readyStateDestination,
  checkpointMedia: inputs.slice(0, 3).map(v => ({ label: v.label, sha256: v.sha256, privateCopy: v.privateCopy }))
}, null, 2) + '\n', { flag: 'wx', mode: 0o600 });
const rawBridge = createBridgeServer(config.bridgePort, (line, error) => {
  fs.appendFileSync(path.join(runRoot, 'host-errors.log'), `${String(error)} ${line}\n`);
});
const mame = createMameController(config);
const handlers = createToolHandlers({ config, bridge: rawBridge, mame, toolchain: createToolchain('decb'),
  logger: { log: s => fs.appendFileSync(path.join(runRoot, 'host.log'), `${s}\n`) } });
const resultFile = path.join(evidenceDir, 'host-results.jsonl');
let activeRun = false;
let lifecycle = 'WAITING_FOR_GRAPHICS';
let snapshotNumber = 0;

async function markLifecycle(next) {
  if (lifecycle === next) return;
  const r = await rawBridge.request('msh_observer_lifecycle', { state: next }, 3000);
  if (!r.ok || r.result?.state !== next) throw new Error(`observer lifecycle update failed: ${JSON.stringify(r)}`);
  lifecycle = next;
}
const bridge = {
  get listening() { return rawBridge.listening; }, get port() { return rawBridge.port; },
  get address() { return rawBridge.address; }, start: () => rawBridge.start(), stop: () => rawBridge.stop(),
  async request(cmd, params, timeout) {
    const r = await rawBridge.request(cmd, params, timeout);
    if (activeRun && cmd === 'read_text_console' && r.ok) {
      const d = r.result || {};
      fs.appendFileSync(path.join(evidenceDir, 'console-lifecycle.jsonl'), JSON.stringify({
        emulatedTime: d.emulatedTime, supported: d.supported, reasonCode: d.reasonCode,
        idle: d.idle, epoch: d.epoch,
      }) + '\n');
      if (d.supported === false && d.reasonCode === 'unsupported_display' && lifecycle === 'WAITING_FOR_GRAPHICS')
        await markLifecycle('GRAPHICS_ACTIVE');
      else if (d.supported === true && lifecycle === 'GRAPHICS_ACTIVE')
        await markLifecycle('GRAPHICS_DEPARTED');
    }
    return r;
  },
};
const exposedMame = createMameController(config);
const wrappedHandlers = createToolHandlers({ config, bridge, mame: exposedMame,
  toolchain: createToolchain('decb'), logger: { log: s => fs.appendFileSync(path.join(runRoot, 'host.log'), `${s}\n`) } });
const allow = new Set(['coco_start','coco_status','coco_mount_flop','coco_list_images','coco_type',
  'coco_snapshot','coco_save_state','os9_restore_ready','os9_run','coco_stop','mount_presenter_d1','type7_array_probe','post_mount_requalify']);
let currentReady = null;
let presenterMount = null;

async function drainAll() {
  const all = [];
  for (let i = 0; i < 8; i++) {
    const r = await rawBridge.request('msh_observer_drain', { limit: 256 }, 5000);
    if (!r.ok) throw new Error(`observer drain: ${r.error}`);
    const batch = r.result?.events || [];
    all.push(...batch);
    if (batch.length < 256) break;
  }
  const out = path.join(evidenceDir, 'observer-events.jsonl');
  if (all.length) await fsp.appendFile(out, all.map(v => JSON.stringify(v)).join('\n') + '\n');
  return { count: all.length, final: all.at(-1) || null };
}
function firstText(r) { return r?.content?.find(x => x.type === 'text')?.text; }
async function saveStateEvidence(name, toolResult) {
  const text = firstText(toolResult);
  if (!text) throw new Error('save state returned no text status');
  const status = JSON.parse(text);
  if (!status.fileFound || !status.file) throw new Error('new paired state file not found');
  const b = await fsp.readFile(status.file);
  const rec = { state: status.file, size: b.length, sha256: sha(b), name, createdAfterFinalMediaMount: true,
    media: inputs.map(v => ({ label: v.label, sha256: v.sha256, path: v.privateCopy })) };
  await fsp.writeFile(path.join(evidenceDir, 'checkpoint-pair.json'), JSON.stringify(rec, null, 2) + '\n');
  return rec;
}
async function handle(name, args) {
  if (name === 'msh_observer_ready' || name === 'msh_observer_state') {
    const r = await rawBridge.request(name, {}, 5000); if (!r.ok) throw new Error(r.error);
    fs.appendFileSync(resultFile, JSON.stringify({ name, result: r.result }) + '\n'); return r.result;
  }
  if (name === 'msh_observer_drain') return await drainAll();
  if (name === 'msh_observer_raw_export') {
    const infoResult = await rawBridge.request('msh_observer_raw_info', {}, 5000);
    if (!infoResult.ok || !infoResult.result?.present) throw new Error(`raw first candidate unavailable: ${JSON.stringify(infoResult)}`);
    const info = infoResult.result;
    if (!Number.isInteger(info.byteLength) || info.byteLength < 1 || info.byteLength > 2_000_000)
      throw new Error('raw snapshot size outside bounded export range');
    const parts = [];
    for (let offset = 0; offset < info.byteLength; offset += 8192) {
      const length = Math.min(8192, info.byteLength - offset);
      const r = await rawBridge.request('msh_observer_raw_chunk', { offset, length }, 5000);
      if (!r.ok || r.result?.offset !== offset || r.result?.length !== length
          || typeof r.result.hex !== 'string' || r.result.hex.length !== length * 2
          || !/^[0-9a-f]+$/i.test(r.result.hex)) throw new Error(`raw snapshot chunk mismatch at ${offset}`);
      parts.push(Buffer.from(r.result.hex, 'hex'));
    }
    const bytes = Buffer.concat(parts);
    if (bytes.length !== info.byteLength) throw new Error('raw snapshot total length mismatch');
    const crop = [];
    if (info.width >= 96 && info.height >= 94 && bytes.length === info.width * info.height * 4)
      for (let y = 86; y <= 93; y++) crop.push(bytes.subarray((y * info.width + 64) * 4, (y * info.width + 96) * 4));
    const cropBytes = Buffer.concat(crop);
    if (info.cropHex && cropBytes.toString('hex').toUpperCase() !== info.cropHex.toUpperCase())
      throw new Error('raw crop differs from full-snapshot extraction');
    const snapshotPath = path.join(evidenceDir, 'first-candidate-snapshot.raw');
    const cropPath = path.join(evidenceDir, 'first-candidate-crop.raw');
    await fsp.writeFile(snapshotPath, bytes, { flag: 'wx', mode: 0o600 });
    await fsp.writeFile(cropPath, cropBytes, { flag: 'wx', mode: 0o600 });
    const proof = { ...info, snapshotPath, snapshotSha256: sha(bytes), cropPath,
      cropSha256: sha(cropBytes), cropBytes: cropBytes.length };
    await fsp.writeFile(path.join(evidenceDir, 'first-candidate-raw.json'), JSON.stringify(proof, null, 2) + '\n', { flag: 'wx' });
    fs.appendFileSync(resultFile, JSON.stringify({ name, result: proof }) + '\n');
    return proof;
  }
  if (name === 'private_info') return { runRoot, mameVersion, cpuNode: cpuMatch?.[0], bridgePort, httpPort,
    config: { machine: config.mameMachine, monitor: config.cocoMonitor, screenConfig: 1, ram: config.cocoRam,
      romPath: config.mameRomPath, mamePath: config.mamePath, stateDir: config.stateDir,
      cfgDir: config.mameCfgDir, bridgeLuaPath: config.bridgeLuaPath }, inputs, presenterD1: inputs[3], readyStateCopy };
  if (name === 'private_shutdown') {
    await exposedMame.stop(); await bridge.stop();
    return { stopped: true, status: exposedMame.status() };
  }
  if (name === 'mount_presenter_d1') {
    const result = await wrappedHandlers.coco_mount_flop({ drive: 'flop2', path: inputs[3].privateCopy });
    presenterMount = result;
    fs.appendFileSync(resultFile, JSON.stringify({ name, result }) + '\n');
    return result;
  }
  if (name === 'post_mount_requalify') {
    if (!currentReady || !presenterMount) throw new Error('restore and mount must complete before requalification');
    const mount = JSON.parse(firstText(presenterMount));
    const imageResult = await wrappedHandlers.coco_list_images({});
    const images = JSON.parse(firstText(imageResult));
    const proof = await requalifyAfterMount({ bridge, mount, images,
      expectedImage: inputs[3].privateCopy, ready: currentReady, decodeConsole, rememberShell, forgetShell });
    fs.appendFileSync(resultFile, JSON.stringify({ name, result: proof }) + '\n');
    return proof;
  }
  if (name === 'type7_array_probe') {
    const ping = await rawBridge.request('ping', {}, 5000);
    const status = await rawBridge.request('status', {}, 5000);
    if (!ping.ok || typeof ping.result?.t !== 'number') throw new Error(`bridge ping failed: ${JSON.stringify(ping)}`);
    if (!status.ok || status.result?.driver !== 'coco3h') throw new Error(`bridge status failed: ${JSON.stringify(status)}`);
    const response = await rawBridge.request('type7_array_probe', {}, 5000);
    if (!response.ok) throw new Error(`type7 array probe failed: ${response.error}`);
    const value = response.result || {};
    const expectedBands = ['000000','555555','AAAAAA','FFFFFF'];
    const expectedPalette = [0,7,56,63];
    if (!Array.isArray(value.bandRGB) || value.bandRGB.length !== 4
        || value.bandRGB.some((v,i) => v !== expectedBands[i])
        || !Array.isArray(value.paletteValues) || value.paletteValues.length !== 4
        || value.paletteValues.some((v,i) => v !== expectedPalette[i]))
      throw new Error(`host JSON array assertion failed: ${JSON.stringify(value)}`);
    const ready = await rawBridge.request('msh_observer_ready', {}, 5000);
    if (!ready.ok || ready.result?.ready !== true || ready.result?.classifierSelfTests !== 39
        || ready.result?.notifierInstalled !== true || ready.result?.ramBytes !== 2097152
        || ready.result?.frameCallbacks < 1 || ready.result?.failure || ready.result?.overflow)
      throw new Error(`observer preflight failed: ${JSON.stringify(ready.result || ready)}`);
    const rawLines = (await fsp.readFile(process.env.BRIDGE_RAW_LOG, 'utf8')).trim().split('\n');
    const rawResponse = rawLines.find(line => { try { return JSON.parse(line).id === response.id; } catch { return false; } });
    if (!rawResponse) throw new Error('raw type7 bridge response was not captured');
    const proof = { ping, status, requestId: response.id, rawLuaToHostResponse: rawResponse,
      parsedByProductionParseResponseLine: response, observerReady: ready.result,
      assertions: { bandRGBArrayLength: value.bandRGB.length, bandRGB: value.bandRGB,
        paletteValuesArrayLength: value.paletteValues.length, paletteValues: value.paletteValues } };
    await fsp.writeFile(path.join(evidenceDir, 'type7-array-probe.json'), JSON.stringify(proof, null, 2) + '\n', { flag: 'wx' });
    fs.appendFileSync(resultFile, JSON.stringify({ name, result: proof }) + '\n');
    return proof;
  }
  if (!allow.has(name)) throw new Error(`unsupported private RPC ${name}`);
  if (name === 'coco_start') {
    const readyBytes = await fsp.readFile(readyStateDestination);
    if (readyBytes.length !== 143064 || sha(readyBytes) !== expectedReadyStateSha)
      throw new Error('ready-state bytes changed before MAME start');
    if (exposedMame.status().running) throw new Error('refusing to reuse an already-running MAME session');
    writeMonitorConfig(config);
    const cfgFile = path.join(config.mameCfgDir, 'coco3h.cfg');
    const actualXml = await fsp.readFile(cfgFile, 'utf8');
    if (!actualXml.includes('<system name="coco3h">')
        || !actualXml.includes('tag=":screen_config" type="CONFIG" mask="1" defvalue="0" value="1"'))
      throw new Error('actual generated MAME RGB configuration failed validation');
    const argv = exposedMame.buildArgs();
    const expected = [inputs[0].privateCopy, inputs[1].privateCopy];
    if (!argv.includes('-ramsize') || argv[argv.indexOf('-ramsize') + 1] !== '2M'
        || !argv.includes('-flop1') || argv[argv.indexOf('-flop1') + 1] !== expected[0]
        || !argv.includes('-hard1') || argv[argv.indexOf('-hard1') + 1] !== expected[1]
        || !argv.includes('-cfg_directory') || argv[argv.indexOf('-cfg_directory') + 1] !== config.mameCfgDir
        || !argv.includes('-state_directory') || argv[argv.indexOf('-state_directory') + 1] !== config.stateDir
        || !argv.includes('-statename') || argv[argv.indexOf('-statename') + 1] !== '%g'
        || argv.some(x => x === '/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/media/63EMU.DSK'
          || x === '/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/media/63SDC-MCP-DEV.VHD'
          || x === '/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/media/63SDC.VHD'))
      throw new Error('MAME argument/media preflight failed');
    const rec = { executable: config.mamePath, version: mameVersion, machine: config.mameMachine,
      cpuNode: cpuMatch[0], monitor: 'RGB', screen_config: 1, ram: '2M', cwd: path.dirname(config.mamePath),
      argv, generatedConfig: cfgFile, generatedConfigSha256: sha(Buffer.from(actualXml)),
      readyStateCopy, readyStateDestination,
      mountedAtLaunch: { flop1: inputs[0].privateCopy, hard1: inputs[1].privateCopy },
      floppy2WillBeMountedBeforeDOS: inputs[2].privateCopy };
    await fsp.writeFile(path.join(evidenceDir, 'launch.json'), JSON.stringify(rec, null, 2) + '\n');
  }
  let actualArgs = args || {};
  if (name === 'coco_mount_flop') actualArgs = { drive: 'flop2', path: inputs[2].privateCopy };
  if (name === 'os9_restore_ready') actualArgs = { timeout_ms: Number(args?.timeout_ms || 30000) };
  if (name === 'os9_run') {
    const r = await rawBridge.request('msh_observer_ready', {}, 5000);
    if (!r.ok || !r.result?.ready || r.result?.classifierSelfTests !== 39
        || r.result?.notifierInstalled !== true || r.result?.frameCallbacks < 1
        || r.result?.ramBytes !== 2097152 || r.result?.failure)
      throw new Error(`observer readiness failure: ${JSON.stringify(r.result || r)}`);
    activeRun = true;
  }
  let result;
  try {
    if (name === 'coco_snapshot') {
      result = await wrappedHandlers[name](actualArgs);
      const image = result.content?.find(v => v.type === 'image');
      if (!image?.data) throw new Error(`snapshot failed: ${firstText(result) || 'missing PNG'}`);
      snapshotNumber++;
      const file = path.join(evidenceDir, `screen-${String(snapshotNumber).padStart(2,'0')}.png`);
      const bytes = Buffer.from(image.data, 'base64');
      await fsp.writeFile(file, bytes, { flag: 'wx', mode: 0o600 });
      result = { saved: file, size: bytes.length, sha256: sha(bytes) };
    } else {
      result = await wrappedHandlers[name](actualArgs);
      if (name === 'os9_restore_ready') currentReady = result.structuredContent || null;
      if (name === 'coco_save_state') result.privateCheckpoint = await saveStateEvidence(actualArgs.name, result);
      if (name === 'os9_run') {
        result.observer = await drainAll();
        activeRun = false;
      }
    }
  } finally { if (name === 'os9_run') activeRun = false; }
  if (name === 'coco_start') {
    const ready = await rawBridge.request('msh_observer_ready', {}, 5000);
    if (!ready.ok || ready.result?.classifierSelfTests !== 39 || ready.result?.ramBytes !== 2097152)
      throw new Error(`observer/MAME startup gate failed: ${JSON.stringify(ready.result || ready)}`);
  }
  if (name === 'coco_save_state') {
    const pathText = firstText(result);
    const parsed = pathText ? JSON.parse(pathText) : {};
    result = { ...parsed, privateCheckpoint: result.privateCheckpoint };
  }
  fs.appendFileSync(resultFile, JSON.stringify({ name, result }) + '\n');
  return result;
}

const server = http.createServer(async (req, res) => {
  try {
    if (req.method !== 'POST' || req.url !== '/rpc') { res.writeHead(404).end(); return; }
    let body = ''; for await (const part of req) body += part;
    const q = JSON.parse(body);
    const result = await handle(q.name, q.arguments || {});
    res.writeHead(200, { 'content-type': 'application/json' }).end(JSON.stringify(result));
    if (q.name === 'private_shutdown') setTimeout(() => { server.close(); process.exit(0); }, 100);
  } catch (e) {
    const message = e instanceof Error ? e.stack || e.message : String(e);
    fs.appendFileSync(path.join(runRoot, 'host-errors.log'), `${message}\n`);
    res.writeHead(500, { 'content-type': 'application/json' }).end(JSON.stringify({ error: String(e?.message || e) }));
  }
});
await new Promise((resolve, reject) => server.once('error', reject).listen(httpPort, '127.0.0.1', resolve));
const startup = { ready: true, runRoot, mameVersion, cpuNode: cpuMatch[0], bridgePort, httpPort,
  mamePath: config.mamePath, romPath: config.mameRomPath, observerSource: observerPath,
  inputs, readyStateCopy, readyState: config.nitros9ReadyState, stateDir: config.stateDir, configDir: config.mameCfgDir };
await fsp.writeFile(path.join(runRoot, 'private-session.json'), JSON.stringify(startup, null, 2) + '\n');
console.log(JSON.stringify(startup));
async function stop() {
  try { await exposedMame.stop(); } catch {}
  try { await bridge.stop(); } catch {}
  try { server.close(); } catch {}
  process.exit(0);
}
process.on('SIGINT', stop); process.on('SIGTERM', stop);
