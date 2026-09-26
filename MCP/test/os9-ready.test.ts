import assert from "node:assert/strict";
import { mkdtemp, mkdir, writeFile, rm, readFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import path from "node:path";
import net from "node:net";
import test from "node:test";
import { loadConfig } from "../src/config.js";
import { createBridgeServer, type BridgeClient } from "../src/bridge-server.js";
import { decodeConsole, hasProbe, restoreReady, type ReadyClock } from "../src/os9-ready.js";
import { createToolHandlers, type ToolDeps } from "../src/tools.js";

// Captured GIME registers from the real frozen checkpoint; physical screen is
// 80x25 cells, two bytes per cell, at 0x1fe000 (not the CPU's current MMU map).
const registers = [0x6c, 0, 9, 0, 9, 0, 0, 0, 4, 0x75, 1, 3, 0, 0xfc, 0, 0];
function consoleData(rows: string[], idle = true) {
  const bytes = Buffer.alloc(4000);
  for (let y = 0; y < 25; y++) for (let x = 0; x < 80; x++) {
    bytes[(y * 80 + x) * 2] = rows[y]?.charCodeAt(x) || 32;
    bytes[(y * 80 + x) * 2 + 1] = 0x81; // attributes/blink must not erase underlying text
  }
  return { supported: true, registers, cells: bytes.toString("hex"), idle };
}
async function fixture(name?: string) {
  const root = await mkdtemp(path.join(tmpdir(), "os9-ready-"));
  const config = loadConfig(name ? { NITROS9_READY_STATE: name, MAME_MACHINE: "alternate" } : {}, root);
  await mkdir(path.join(config.stateDir, config.mameMachine), { recursive: true });
  await writeFile(path.join(config.stateDir, config.mameMachine, `${config.nitros9ReadyState}.sta`), "fixture");
  let now = 0;
  const timing: ReadyClock = { now: () => now, poll: async () => { now += 100; }, nonce: () => "1234567890abcdef" };
  const calls: Array<{ cmd: string; params: Record<string, unknown> }> = [];
  const state = { pending: 2, noLoad: false, wrongCandidate: false, noEcho: false, echoOnly: false,
    disconnected: false, failCommand: "", prompt: "", marker: "", invalidated: false };
  const bridge: BridgeClient = {
    listening: true, port: 0, address: "127.0.0.1", start: async () => {}, stop: async () => {},
    async request(cmd, params) {
      calls.push({ cmd, params });
      if (state.disconnected) throw new Error("bridge not connected");
      if (cmd === state.failCommand) return { id: "1", ok: false, error: "bridge operation failed" };
      let result: unknown = {};
      if (cmd === "load_state_tracked") result = { scheduled: true, token: params.token };
      if (cmd === "load_state_status") result = { token: params.token, epoch: 7, completed: !state.noLoad && --state.pending <= 0,
        invalidated: state.invalidated };
      if (cmd === "type") {
        const text = String(params.text);
        if (text.startsWith("p=")) state.prompt = text.slice(2).replace("{ENTER}", "");
        if (text.startsWith("echo ")) state.marker = text.slice(5).replace("{ENTER}", "");
        result = { queued: true };
      }
      if (cmd === "read_text_console") {
        if (state.wrongCandidate) result = consoleData(["Editor: press a key"]);
        else if (!state.prompt) result = consoleData(["{Term|02}/DD:"]);
        else if (!state.marker || state.noEcho) result = consoleData([state.prompt]);
        else if (state.echoOnly) result = consoleData([state.prompt + "echo " + state.marker, state.prompt]);
        else result = consoleData([state.prompt + "echo " + state.marker, state.marker, "", state.prompt]);
      }
      return { id: "1", ok: true, result };
    },
  };
  return { root, config, timing, calls, state, bridge, clean: () => rm(root, { recursive: true, force: true }) };
}

test("ready waits for the post-load event then proves fresh output and prompt", async () => {
  const f = await fixture();
  try {
    const r = await restoreReady(f.config, f.bridge, 5000, f.timing);
    assert.equal(r.ready, true);
    assert.equal(r.loadScheduled, true);
    assert.equal(r.loadCompleted, true);
    assert.equal(r.shellVerified, true);
    assert.equal(r.timedOut, false);
    assert.equal(r.postLoadEpoch, 7);
    assert.deepEqual(f.calls.slice(0, 4).map(x => x.cmd), ["load_state_tracked", "load_state_status", "load_state_status", "read_text_console"]);
    assert.ok(f.calls.filter(x => x.cmd === "type").every(x => x.params.epoch === 7));
    assert.equal(f.calls.at(-1)?.cmd, "finish_restore");
    assert.ok(f.calls.every(x => !["save_state", "write_mem", "mount"].includes(x.cmd)));
  } finally { await f.clean(); }
});

test("configured state and machine control discovery and scheduling", async () => {
  const f = await fixture("my_fixture");
  try {
    const r = await restoreReady(f.config, f.bridge, 5000, f.timing);
    assert.equal(r.state, "my_fixture");
    assert.equal(r.ready, true);
    assert.equal(f.calls[0]?.params.name, "my_fixture");
    assert.equal(r.file, path.join(f.config.stateDir, "alternate", "my_fixture.sta"));
    assert.equal(loadConfig({}, f.root).nitros9ReadyState, "nos9_ready_v2");
    for (const name of ["", "../escape", "foo/bar", "foo\\bar"]) {
      assert.throws(() => loadConfig({ NITROS9_READY_STATE: name }, f.root), /invalid NITROS9_READY_STATE/);
    }
  } finally { await f.clean(); }
});

test("missing state is structured and never asks MAME to load", async () => {
  const f = await fixture();
  try {
    await rm(f.config.stateDir, { recursive: true });
    const r = await restoreReady(f.config, f.bridge, 5000, f.timing);
    assert.equal(r.status, "state_missing");
    assert.equal(r.loadScheduled, false);
    assert.equal(r.ready, false);
    assert.equal(f.calls.length, 0);
  } finally { await f.clean(); }
});

test("post-load deadline cannot be satisfied by a scheduled acknowledgement", async () => {
  const f = await fixture();
  try {
    f.state.noLoad = true;
    const r = await restoreReady(f.config, f.bridge, 1000, f.timing);
    assert.equal(r.status, "post_load_timeout");
    assert.equal(r.loadScheduled, true);
    assert.equal(r.loadCompleted, false);
    assert.equal(r.timedOut, true);
    assert.ok(f.calls.every(x => x.cmd !== "type"));
  } finally { await f.clean(); }
});

for (const mode of ["wrongCandidate", "noEcho", "echoOnly"] as const) {
  test(`shell verification fails safely: ${mode}`, async () => {
    const f = await fixture();
    try {
      f.state[mode] = true;
      const r = await restoreReady(f.config, f.bridge, 1000, f.timing);
      assert.equal(r.status, "shell_handshake_failed");
      assert.equal(r.loadCompleted, true);
      assert.equal(r.shellVerified, false);
      assert.equal(r.ready, false);
      assert.equal(r.timedOut, mode !== "wrongCandidate");
      if (mode === "wrongCandidate") assert.ok(f.calls.every(x => x.cmd !== "type"));
    } finally { await f.clean(); }
  });
}

for (const mode of ["disconnect", "response_failure", "invalidated"] as const) {
  test(`bridge/operation failure is structured: ${mode}`, async () => {
    const f = await fixture();
    try {
      f.state.disconnected = mode === "disconnect";
      f.state.failCommand = mode === "response_failure" ? "load_state_status" : "";
      f.state.invalidated = mode === "invalidated";
      const r = await restoreReady(f.config, f.bridge, 1000, f.timing);
      assert.equal(r.status, mode === "invalidated" ? "restore_invalidated" : "bridge_failure");
      assert.equal(r.ready, false);
      assert.equal(r.shellVerified, false);
    } finally { await f.clean(); }
  });
}

test("console decoder handles attributes and rejects unsupported modes or incomplete data", () => {
  const data = consoleData(["READYabcdef", "MCPabcdef:"]);
  assert.deepEqual(decodeConsole(data).slice(0, 2), ["READYabcdef", "MCPabcdef:"]);
  for (const [index, value] of [[0, 0x80], [8, 0x84], [9, 0x65], [12, 1], [15, 0x80]]) {
    const altered = [...registers]; altered[index!] = value!;
    assert.throws(() => decodeConsole({ ...data, registers: altered }), /unsupported/);
  }
  assert.throws(() => decodeConsole({ ...data, cells: "20" }), /invalid console/);
  assert.equal(hasProbe(["echo READYabc", "MCPabc:"], "READYabc", "MCPabc:"), false);
  assert.equal(hasProbe(["MCPabc:", "READYabc"], "READYabc", "MCPabc:"), false);
  assert.equal(hasProbe(["READYabc", "MCPabc:", "busy"], "READYabc", "MCPabc:"), false);
  assert.equal(hasProbe(["READYabc", "", "MCPabc:"], "READYabc", "MCPabc:"), true);
});

test("mutating tools cannot interleave with the restore handshake", async () => {
  const f = await fixture();
  try {
    let release!: () => void;
    const gate = new Promise<void>(resolve => { release = resolve; });
    const real = f.bridge.request.bind(f.bridge);
    f.bridge.request = async (...args) => { if (args[0] === "load_state_tracked") await gate; return real(...args); };
    const handlers = createToolHandlers({ config: f.config, bridge: f.bridge } as ToolDeps);
    const pending = handlers.os9_restore_ready!({});
    for (const name of ["os9_restore_ready", "coco_type", "coco_load_state", "coco_save_state", "coco_soft_reset", "coco_stop", "coco_mount_flop"]) {
      const r = await handlers[name]!({});
      assert.equal((r.structuredContent as { status: string }).status, "busy");
    }
    release();
    const r = await pending;
    assert.equal((r.structuredContent as { ready: boolean }).ready, true);
  } finally { await f.clean(); }
});

test("restore runs over the real JSON/TCP bridge and reports a socket disconnect", async () => {
  const f = await fixture();
  const bridge = createBridgeServer(0);
  let peer: net.Socket | undefined;
  try {
    await bridge.start();
    peer = net.createConnection(bridge.port, bridge.address);
    await new Promise<void>(resolve => peer!.once("connect", resolve));
    let input = "";
    let disconnect = false;
    peer.on("data", chunk => {
      input += chunk.toString();
      let newline: number;
      while ((newline = input.indexOf("\n")) >= 0) {
        const req = JSON.parse(input.slice(0, newline)); input = input.slice(newline + 1);
        if (disconnect) { peer!.destroy(); break; }
        void f.bridge.request(req.cmd, req.params).then(r => peer!.write(JSON.stringify({ ...r, id: req.id }) + "\n"));
      }
    });
    const r = await restoreReady(f.config, bridge, 5000);
    assert.equal(r.ready, true);
    disconnect = true;
    const failed = await restoreReady(f.config, bridge, 5000);
    assert.equal(failed.status, "bridge_failure");
    assert.equal(failed.ready, false);
  } finally { peer?.destroy(); await bridge.stop(); await f.clean(); }
});

test("Lua retains post-load/reset subscriptions and reads physical RAM without a guest MMU write", async () => {
  const source = await readFile(new URL("../scripts/bridge.lua", import.meta.url), "utf8");
  assert.match(source, /post_load_notifier = emu\.add_machine_post_load_notifier/);
  assert.match(source, /restore_reset_notifier = emu\.add_machine_reset_notifier/);
  const reader = source.slice(source.indexOf("local function cmd_read_text_console"), source.indexOf("local commands ="));
  assert.match(reader, /read_block\(base, 4000\)/);
  assert.doesNotMatch(reader, /write_u8|program_space\(/);
});
