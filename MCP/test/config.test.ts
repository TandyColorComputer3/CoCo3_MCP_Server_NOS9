import assert from "node:assert/strict";
import { mkdtemp, readFile, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import path from "node:path";
import test from "node:test";
import { loadConfig, loadDotEnv, writeMonitorConfig } from "../src/config.js";

test("loadConfig applies documented defaults", () => {
  const root = path.resolve("C:/coco/MCP");
  const cfg = loadConfig({}, root);
  assert.equal(cfg.bridgePort, 18765);
  assert.equal(cfg.cocoRam, "2M");
  assert.equal(cfg.mameMachine, "coco3h");
  assert.equal(cfg.cocoMonitor, "rgb");
  assert.equal(cfg.mamePath, "");
  assert.equal(cfg.mameRomPath, "");
  assert.equal(cfg.decbPath, "decb");
  assert.equal(cfg.rootDir, root);
  assert.equal(cfg.bridgeLuaPath, path.join(root, "scripts", "bridge.lua"));
  assert.equal(cfg.snapshotDir, path.join(root, "snapshots"));
  assert.equal(cfg.stateDir, path.join(root, "states"));
  assert.equal(cfg.workDir, path.join(root, "work"));
  assert.equal(cfg.logDir, path.join(root, "logs"));
  for (const value of [cfg.rootDir, cfg.bridgeLuaPath, cfg.snapshotDir, cfg.stateDir, cfg.workDir, cfg.logDir]) {
    assert.equal(path.isAbsolute(value), true);
  }
});

test("loadConfig resolves a relative root and honors overrides", () => {
  const cfg = loadConfig(
    {
      BRIDGE_PORT: "19000",
      COCO_RAM: "128K",
      MAME_MACHINE: "coco3",
      COCO_MONITOR: "composite",
      MAME_PATH: "C:\\mame\\mame.exe",
      MAME_ROMPATH: "C:\\mame\\roms",
      DECB_PATH: "C:\\toolshed\\decb.exe",
    },
    "relative-root",
  );
  assert.equal(cfg.rootDir, path.resolve("relative-root"));
  assert.equal(cfg.bridgePort, 19000);
  assert.equal(cfg.cocoRam, "128K");
  assert.equal(cfg.mameMachine, "coco3");
  assert.equal(cfg.cocoMonitor, "composite");
  assert.equal(cfg.mamePath, "C:\\mame\\mame.exe");
  assert.equal(cfg.mameRomPath, "C:\\mame\\roms");
  assert.equal(cfg.decbPath, "C:\\toolshed\\decb.exe");
  assert.equal(cfg.bridgeLuaPath, path.join(cfg.rootDir, "scripts", "bridge.lua"));
});

test("machine names stay within a single state/config directory and monitor values are validated", () => {
  for (const machine of ["", "../coco3", "a/b", "a\\b", 'coco3\"']) {
    assert.throws(() => loadConfig({ MAME_MACHINE: machine }, "root"), /MAME_MACHINE/);
  }
  assert.throws(() => loadConfig({ COCO_MONITOR: "vga" }, "root"), /COCO_MONITOR/);
});

test("monitor config encodes the installed MAME Monitor Type field for RGB and composite", async () => {
  const root = await mkdtemp(path.join(tmpdir(), "coco-monitor-"));
  try {
    for (const [monitor, value] of [["rgb", 1], ["composite", 0]] as const) {
      const cfg = loadConfig({ MAME_MACHINE: "coco3h", COCO_MONITOR: monitor }, root);
      writeMonitorConfig(cfg);
      const xml = await readFile(path.join(cfg.mameCfgDir, "coco3h.cfg"), "utf8");
      assert.match(xml, /<mameconfig version="10">/);
      assert.match(xml, /<system name="coco3h">/);
      assert.ok(xml.includes(`<port tag=":screen_config" type="CONFIG" mask="1" defvalue="0" value="${value}" />`));
    }
  } finally {
    await rm(root, { recursive: true, force: true });
  }
});

test("loadConfig rejects a bad bridge port", () => {
  for (const port of ["nope", "0", "65536", "1.5", "-1"]) {
    assert.throws(() => loadConfig({ BRIDGE_PORT: port }, path.resolve("root")), port);
  }
});

test("loadDotEnv parses comments, blanks, and one pair of quotes", async () => {
  const dir = await mkdtemp(path.join(tmpdir(), "coco-env-"));
  try {
    const file = path.join(dir, ".env");
    await writeFile(
      file,
      ["# comment", "BRIDGE_PORT=19001", 'MAME_PATH="C:\\mame\\mame.exe"', "DECB_PATH='decb.exe'", "", "COCO_RAM=512K", ""].join(
        "\n",
      ),
    );
    assert.deepEqual(loadDotEnv(file), {
      BRIDGE_PORT: "19001",
      MAME_PATH: "C:\\mame\\mame.exe",
      DECB_PATH: "decb.exe",
      COCO_RAM: "512K",
    });
    assert.deepEqual(loadDotEnv(path.join(dir, "missing.env")), {});
  } finally {
    await rm(dir, { recursive: true, force: true });
  }
});
