#!/usr/bin/env node
/*
 * Private-only MCP/MAME host. It follows the preserved M6 private-host
 * pattern but gives this run its own config, bridge, media, states and ports.
 * It never edits MCP/.env or writes to the input media paths.
 */
import fs from 'node:fs';
import fsp from 'node:fs/promises';
import http from 'node:http';
import net from 'node:net';
import os from 'node:os';
import path from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import { execFileSync } from 'node:child_process';

const here = path.dirname(fileURLToPath(import.meta.url));
const harness = path.resolve(here, '..');
const repo = path.resolve(here, '../../../../../');
const mcp = path.join(repo, 'MCP');
const readJson = JSON.parse(await fsp.readFile(process.argv[2] || path.join(harness, 'launcher/config.json'), 'utf8'));
const required = ['mamePath', 'romPath', 'bootFloppySource', 'testFloppySource', 'vhdSource', 'readyStateSource'];
for (const key of required) if (!readJson[key]) throw new Error(`missing launcher config field: ${key}`);
if (readJson.machine !== 'coco3h' || readJson.cpu !== 'HD6309E' || readJson.monitor !== 'rgb'
    || readJson.screenConfig !== 1 || readJson.ram !== '2M'
    || readJson.mameBuild !== 'ample-289-u2') {
  throw new Error('launcher config must specify coco3h / HD6309E / RGB / screen_config=1 / 2M / ample-289-u2');
}
const mameVersion = execFileSync(readJson.mamePath, ['-version'], { encoding: 'utf8' }).trim();
if (!mameVersion.includes('0.289')) throw new Error(`expected MAME 0.289, got: ${mameVersion}`);

async function copyInput(source, destination) {
  const src = path.resolve(source);
  const dst = path.resolve(destination);
  const stat = await fsp.stat(src);
  if (!stat.isFile()) throw new Error(`input is not a regular file: ${src}`);
  if (src === dst) throw new Error(`refusing to use an input as its own writable copy: ${src}`);
  await fsp.copyFile(src, dst, fs.constants.COPYFILE_EXCL);
  return { source: src, privateCopy: dst, size: stat.size };
}

async function reservePort() {
  const server = net.createServer();
  await new Promise((resolve, reject) => server.once('error', reject).listen(0, '127.0.0.1', resolve));
  const address = server.address();
  await new Promise(resolve => server.close(resolve));
  return address.port;
}

const runRoot = await fsp.mkdtemp(path.join(os.tmpdir(), 'dag-mixed-scene-'));
const media = path.join(runRoot, 'media');
const stateDir = path.join(runRoot, 'states', 'coco3h');
const scriptDir = path.join(runRoot, 'scripts');
await Promise.all([fsp.mkdir(media, { recursive: true }), fsp.mkdir(stateDir, { recursive: true }), fsp.mkdir(scriptDir, { recursive: true })]);
const copied = [];
copied.push(await copyInput(readJson.bootFloppySource, path.join(media, 'boot-flop1.dsk')));
copied.push(await copyInput(readJson.testFloppySource, path.join(media, 'test-flop2.dsk')));
copied.push(await copyInput(readJson.vhdSource, path.join(media, 'system.vhd')));
copied.push(await copyInput(readJson.readyStateSource,
  path.join(stateDir, `${readJson.readyState || 'nos9_ready_v2'}.sta`)));

const baseBridge = await fsp.readFile(path.join(mcp, 'scripts/bridge.lua'));
const observer = await fsp.readFile(path.join(harness, 'observer/observer.lua'));
const privateBridgePath = path.join(scriptDir, 'bridge.lua');
await fsp.writeFile(privateBridgePath, Buffer.concat([baseBridge, Buffer.from('\n-- private Stage 3E.13 observer follows\n'), observer]));

const bridgePort = Number(readJson.bridgePort || await reservePort());
const httpPort = Number(readJson.httpPort || await reservePort());
if (!Number.isInteger(bridgePort) || bridgePort < 1024 || bridgePort > 65535 || bridgePort === httpPort)
  throw new Error('invalid or conflicting private ports');
process.env.MSH_RUN_ROOT = runRoot;

const importMcp = async name => import(pathToFileURL(path.join(mcp, 'dist', `${name}.js`)).href);
const [{ loadConfig, writeBridgePortFile }, { createBridgeServer }, { createMameController },
       { createToolHandlers }, { createToolchain }] = await Promise.all([
  importMcp('config'), importMcp('bridge-server'), importMcp('mame-process'),
  importMcp('tools'), importMcp('toolchain'),
]);
const config = loadConfig({
  MAME_PATH: readJson.mamePath,
  MAME_ROMPATH: readJson.romPath,
  MAME_MACHINE: 'coco3h',
  COCO_MONITOR: 'rgb',
  COCO_RAM: '2M',
  MAME_BOOT_FLOPPY: copied[0].privateCopy,
  MAME_VHD: copied[2].privateCopy,
  BRIDGE_PORT: String(bridgePort),
  NITROS9_READY_STATE: readJson.readyState || 'nos9_ready_v2',
}, runRoot);
if (config.mameCfgDir !== path.join(runRoot, 'work/mame-cfg')
    || config.bridgeLuaPath !== privateBridgePath
    || config.cocoMonitor !== 'rgb' || config.cocoRam !== '2M' || config.mameMachine !== 'coco3h')
  throw new Error('MCP config resolution escaped the private run root or changed required settings');
writeBridgePortFile(config);

const rawBridge = createBridgeServer(config.bridgePort, (line, error) => {
  fs.appendFileSync(path.join(runRoot, 'host-errors.log'), `${String(error)} ${line}\n`);
});
let activeRun = false;
let lifecycle = 'WAITING_FOR_GRAPHICS';
async function markLifecycle(state) {
  if (lifecycle === state) return;
  const response = await rawBridge.request('msh_observer_lifecycle', { state }, 3000);
  if (!response.ok || response.result?.state !== state) throw new Error(`observer lifecycle update failed: ${JSON.stringify(response)}`);
  lifecycle = state;
}
const bridge = {
  get listening() { return rawBridge.listening; },
  get port() { return rawBridge.port; },
  get address() { return rawBridge.address; },
  start: () => rawBridge.start(),
  stop: () => rawBridge.stop(),
  async request(cmd, params, timeout) {
    const response = await rawBridge.request(cmd, params, timeout);
    if (activeRun && cmd === 'read_text_console' && response.ok) {
      const display = response.result || {};
      if (display.supported === false && display.reasonCode === 'unsupported_display'
          && lifecycle === 'WAITING_FOR_GRAPHICS') await markLifecycle('GRAPHICS_ACTIVE');
      else if (display.supported === true && lifecycle === 'GRAPHICS_ACTIVE')
        await markLifecycle('GRAPHICS_DEPARTED');
    }
    return response;
  },
};
const mame = createMameController(config);
const handlers = createToolHandlers({
  config, bridge, mame, toolchain: createToolchain('decb'),
  logger: { log: message => fs.appendFileSync(path.join(runRoot, 'host.log'), `${message}\n`) },
});
const resultPath = path.join(runRoot, 'host-results.jsonl');
const allow = new Set(['coco_start', 'coco_stop', 'coco_status', 'coco_mount_flop',
  'coco_snapshot', 'os9_restore_ready', 'os9_run']);

async function drainObserver() {
  const response = await rawBridge.request('msh_observer_drain', { limit: 256 }, 3000);
  if (!response.ok) throw new Error(response.error);
  const result = response.result;
  const lines = (result.events || []).map(event => JSON.stringify(event)).join('\n');
  if (lines) fs.appendFileSync(path.join(runRoot, 'observer-events.jsonl'), `${lines}\n`);
  return result;
}

const server = http.createServer(async (req, res) => {
  try {
    if (req.method !== 'POST' || req.url !== '/rpc') { res.writeHead(404).end(); return; }
    let body = '';
    for await (const part of req) body += part;
    const query = JSON.parse(body);
    let result;
    if (query.name === 'msh_observer_ready' || query.name === 'msh_observer_state') {
      const response = await rawBridge.request(query.name, {}, 3000);
      if (!response.ok) throw new Error(response.error);
      result = response.result;
    } else if (query.name === 'msh_observer_drain') {
      result = await drainObserver();
    } else {
      if (!allow.has(query.name)) throw new Error(`unsupported private RPC: ${query.name}`);
      if (query.name === 'coco_start') {
        // Inspect the exact private config file before delegating to MAME start.
        const configFile = path.join(config.mameCfgDir, `${config.mameMachine}.cfg`);
        const { writeMonitorConfig } = await importMcp('config');
        writeMonitorConfig(config);
        const xml = await fsp.readFile(configFile, 'utf8');
        if (!xml.includes('<system name="coco3h">')
            || !xml.includes('tag=":screen_config" type="CONFIG" mask="1" defvalue="0" value="1"'))
          throw new Error(`private RGB config verification failed: ${configFile}`);
      }
      if (query.name === 'coco_mount_flop') {
        query.arguments = { ...(query.arguments || {}), drive: 'flop2', path: copied[1].privateCopy };
      }
      if (query.name === 'os9_restore_ready') {
        query.arguments = { ...(query.arguments || {}) };
      }
      if (query.name === 'os9_run') {
        const readiness = await rawBridge.request('msh_observer_ready', {}, 3000);
        if (!readiness.ok || !readiness.result?.ready || !readiness.result?.tapInstalled || !readiness.result?.frameRegistered)
          throw new Error('observer readiness not proven; refusing workload launch');
        activeRun = true;
      }
      try { result = await handlers[query.name](query.arguments || {}); }
      finally { if (query.name === 'os9_run') activeRun = false; }
      if (query.name === 'os9_run') result.observer = await drainObserver();
    }
    fs.appendFileSync(resultPath, `${JSON.stringify({ name: query.name, result })}\n`);
    res.writeHead(200, { 'content-type': 'application/json' }).end(JSON.stringify(result));
  } catch (error) {
    const message = error instanceof Error ? error.message : String(error);
    fs.appendFileSync(path.join(runRoot, 'host-errors.log'), `${message}\n`);
    res.writeHead(500, { 'content-type': 'application/json' }).end(JSON.stringify({ error: message }));
  }
});

await new Promise((resolve, reject) => server.once('error', reject).listen(httpPort, '127.0.0.1', resolve));
console.log(JSON.stringify({ ready: true, runRoot, machine: config.mameMachine, cpu: 'HD6309E',
  monitor: config.cocoMonitor, screen_config: 1, ram: config.cocoRam,
  mamePath: config.mamePath, bridgePort, httpPort, privateBridgePath,
  copiedInputs: copied.map(({ source, privateCopy, size }) => ({ source, privateCopy, size })) }));

async function stop() {
  try { await mame.stop(); } catch {}
  try { await bridge.stop(); } catch {}
  await new Promise(resolve => server.close(resolve));
  process.exit(0);
}
process.on('SIGINT', stop);
process.on('SIGTERM', stop);
