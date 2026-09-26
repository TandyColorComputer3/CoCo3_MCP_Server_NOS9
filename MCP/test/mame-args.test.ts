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


test("verified MPI slot 3 Glenside launch preserves canonical settings and EOU media", () => {
  const root = path.resolve("MCP");
  const cfg = loadConfig({
    MAME_SLOTS: '{"ext":"multi","ext:multi:slot3":"ide","ext:multi:slot4":"fdc"}',
    MAME_BOOT_FLOPPY: "../media/63EMU.DSK",
    MAME_VHD: "../media/63SDC-MCP-DEV.VHD",
  }, root);
  const args = buildMameArgs(cfg);
  const legacy = buildMameArgs(loadConfig({}, root));
  assert.deepEqual(args, [
    ...legacy.slice(0, 7),
    "-ext", "multi", "-ext:multi:slot3", "ide", "-ext:multi:slot4", "fdc",
    ...legacy.slice(9),
    "-flop1", path.resolve(root, "../media/63EMU.DSK"),
    "-hard1", path.resolve(root, "../media/63SDC-MCP-DEV.VHD"),
  ]);
  assert.equal(cfg.cocoMonitor, "rgb");
});


test("verified Disto RTC occupies the SCII Mini Expansion Bus while retaining Glenside", () => {
  const cfg = loadConfig({ MAME_SLOTS: '{"ext":"multi","ext:multi:slot3":"ide","ext:multi:slot4":"scii","ext:multi:slot4:scii:meb":"rtime"}' }, path.resolve("MCP"));
  const args = buildMameArgs(cfg);
  assert.deepEqual(args.slice(7, 15), ["-ext", "multi", "-ext:multi:slot3", "ide", "-ext:multi:slot4", "scii", "-ext:multi:slot4:scii:meb", "rtime"]);
});
