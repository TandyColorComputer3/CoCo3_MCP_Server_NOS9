import assert from "node:assert/strict";
import path from "node:path";
import test from "node:test";
import { loadConfig } from "../src/config.js";
import { buildMameArgs } from "../src/mame-process.js";

test("buildMameArgs matches the required coco3 command", () => {
  const root = path.resolve("C:/coco/MCP");
  const cfg = loadConfig(
    {
      MAME_PATH: "C:\\mame\\mame.exe",
      MAME_ROMPATH: "C:\\mame\\roms",
      COCO_RAM: "512K",
      MAME_MACHINE: "coco3",
    },
    root,
  );
  assert.deepEqual(buildMameArgs(cfg), [
    "coco3",
    "-window",
    "-skip_gameinfo",
    "-natural",
    "-nomouse",
    "-mouse_device",
    "none",
    "-ext",
    "fdc",
    "-ramsize",
    "512K",
    "-rompath",
    "C:\\mame\\roms",
    "-autoboot_script",
    path.join(root, "scripts", "bridge.lua"),
    "-autoboot_delay",
    "0",
    "-snapshot_directory",
    path.join(root, "snapshots"),
    "-state_directory",
    path.join(root, "states"),
    "-statename",
    "%g",
    "-cfg_directory",
    path.join(root, "work", "mame-cfg"),
  ]);
  assert.equal(buildMameArgs(cfg).includes("-console"), false);
  assert.equal(path.isAbsolute(buildMameArgs(cfg)[14]), true);
});

test("canonical launch uses coco3h, 2M and the MCP-owned monitor config directory", () => {
  const cfg = loadConfig({}, path.resolve("MCP"));
  const args = buildMameArgs(cfg);
  assert.equal(args[0], "coco3h");
  assert.equal(args[args.indexOf("-ramsize") + 1], "2M");
  assert.equal(args[args.indexOf("-cfg_directory") + 1], cfg.mameCfgDir);
  assert.equal(args[args.indexOf("-statename") + 1], "%g");
});
