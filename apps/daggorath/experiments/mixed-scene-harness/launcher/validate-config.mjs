#!/usr/bin/env node
/* Static configuration validation. Does not spawn MAME or read MCP/.env. */
import assert from 'node:assert/strict';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

const repo = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '../../../../../');
const configModule = await import(pathToFileURL(path.join(repo, 'MCP/dist/config.js')).href);
const root = fs.mkdtempSync(path.join(os.tmpdir(), 'msh13-config-check-'));
try {
  const config = configModule.loadConfig({
    MAME_PATH: '/Applications/Emulators/Ample.app/Contents/MacOS/mame64',
    MAME_ROMPATH: '/roms', MAME_MACHINE: 'coco3h', COCO_MONITOR: 'rgb', COCO_RAM: '2M',
    MAME_BOOT_FLOPPY: path.join(root, 'media/boot.dsk'), MAME_VHD: path.join(root, 'media/system.vhd'),
    BRIDGE_PORT: '18831', NITROS9_READY_STATE: 'nos9_ready_v2',
  }, root);
  configModule.writeMonitorConfig(config);
  const monitor = fs.readFileSync(path.join(config.mameCfgDir, 'coco3h.cfg'), 'utf8');
  assert.equal(config.mameMachine, 'coco3h');
  assert.equal(config.cocoMonitor, 'rgb');
  assert.equal(config.cocoRam, '2M');
  assert.equal(config.bridgePort, 18831);
  assert.equal(config.mameCfgDir, path.join(root, 'work/mame-cfg'));
  assert.equal(config.bridgeLuaPath, path.join(root, 'scripts/bridge.lua'));
  assert.match(monitor, /tag=":screen_config" type="CONFIG" mask="1" defvalue="0" value="1"/);
  console.log(JSON.stringify({ machine: config.mameMachine, cpu: 'HD6309E', monitor: config.cocoMonitor,
    screen_config: 1, ram: config.cocoRam, bridgePort: config.bridgePort,
    cfg: path.join(config.mameCfgDir, 'coco3h.cfg'), configXml: monitor.trim() }, null, 2));
} finally {
  fs.rmSync(root, { recursive: true, force: true });
}
