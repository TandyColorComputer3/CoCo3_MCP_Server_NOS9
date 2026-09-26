import assert from "node:assert/strict";
import test from "node:test";
import net from "node:net";
import { createBridgeServer, type BridgeClient } from "../src/bridge-server.js";
import { rememberShell, runOs9, validateCommand, type RunClock } from "../src/os9-run.js";
import type { ReadyResult } from "../src/os9-ready.js";
import { createToolHandlers, type ToolDeps } from "../src/tools.js";
import { loadConfig } from "../src/config.js";

const prompt = "MCP1234567890abcdef:";
function ready(bridge: BridgeClient) {
  rememberShell(bridge, { ready: true, postLoadEpoch: 7, handshake: { prompt } } as unknown as ReadyResult);
}
function view(rows: string[], idle = true) {
  const bytes = Buffer.alloc(4000);
  for (let y = 0; y < 25; y++) for (let x = 0; x < 80; x++) {
    bytes[(y * 80 + x) * 2] = rows[y]?.charCodeAt(x) || 32;
    bytes[(y * 80 + x) * 2 + 1] = 0x81;
  }
  return { supported: true, registers: [0x6c, 0, 9, 0, 9, 0, 0, 0, 4, 0x75, 1, 3, 0, 0xfc, 0, 0], cells: bytes.toString("hex"), idle, epoch: 7 };
}
function fixture() {
  let now = 0, nonce = 0, reads = 0, commandReturned = false;
  const timing: RunClock = { now: () => now, poll: async () => { now += 100; }, nonce: () => (++nonce).toString(16).padStart(32, "0") };
  const calls: Array<{ cmd: string; params: Record<string, unknown> }> = [];
  const state = { phase: "idle", code: "000", marker: "", failure: "", hang: false, stale: false,
    lost: false, malformed: false, unexpected: false, echoOnly: false, duplicate: false, epoch: 7, unsupported: false,
    markerHang: false, invalidatedOnRelease: false };
  const bridge: BridgeClient = {
    port: 0, address: "127.0.0.1", listening: true, start: async () => {}, stop: async () => {},
    async request(cmd, params) {
      calls.push({ cmd, params });
      if (cmd === state.failure) throw new Error("bridge not connected");
      let result: unknown = {};
      if (cmd === "begin_run") { state.phase = "idle"; commandReturned = false; result = { token: params.token, epoch: 7 }; }
      if (cmd === "type") {
        const text = String(params.text);
        reads = 0;
        if (text.startsWith("echo MCPDONE")) {
          assert.equal(commandReturned, true, "status query must wait for foreground prompt");
          assert.match(text, /^echo MCPDONE[0-9a-f]{32} %\*\{ENTER\}$/);
          state.marker = text.split(" ")[1]!; state.phase = "marker";
        } else { assert.equal(text.includes(";"), false); assert.equal(text.includes("%*"), false); state.phase = "command"; }
        result = { queued: true };
      }
      if (cmd === "read_text_console") {
        if (state.phase === "idle") result = view([state.lost ? "some other prompt" : prompt]);
        else {
          reads++;
          const busy = reads === 1 || (state.hang && state.phase === "command") || (state.markerHang && state.phase === "marker");
          if (state.stale && state.phase === "command") result = view([prompt]);
          else if (busy) result = view([prompt + "typing"], false);
          else if (state.phase === "command") { commandReturned = true; result = view(["old output", "MCPDONEold 216", prompt]); }
          else {
            const emitted = state.unexpected ? "MCPDONEwrong 000" : state.marker + (state.malformed ? " bad" : " " + state.code);
            result = view([prompt + "echo " + state.marker + " %*", ...(state.echoOnly ? [] : [emitted]),
              ...(state.duplicate ? [emitted] : []), "", prompt]);
          }
        }
        result = { ...(result as object), epoch: state.epoch, ...(state.unsupported ? { supported: false, reason: "graphics mode" } : {}) };
      }
      if (cmd === "finish_run") result = { released: true, invalidated: state.invalidatedOnRelease };
      return { id: "1", ok: true, result };
    },
  };
  ready(bridge);
  return { bridge, timing, calls, state };
}

for (const code of ["000", "216"]) {
  test(`two-stage run preserves status ${code} and leaves session reusable`, async () => {
    const f = fixture(); f.state.code = code;
    const r = await runOs9(f.bridge, "mdir", 5000, f.timing);
    assert.equal(r.completed, true); assert.equal(r.commandCompleted, true);
    assert.equal(r.status, Number(code)); assert.equal(r.statusText, code);
    assert.equal(r.outcome, "completed"); assert.equal(r.shellReady, true); assert.equal(r.timedOut, false);
    assert.equal(r.outputComplete, false); assert.equal("output" in r, false);
    assert.deepEqual(f.calls.filter(c => c.cmd === "type").map(c => c.params.text), ["mdir{ENTER}", `echo ${r.marker} %*{ENTER}`]);
    const next = await runOs9(f.bridge, "pwd", 5000, f.timing);
    assert.equal(next.completed, true);
    assert.notEqual(next.marker, r.marker);
  });
}

test("production marker generator creates a new 128-bit ID for each request", async () => {
  const f = fixture();
  const a = await runOs9(f.bridge, "pwd"), b = await runOs9(f.bridge, "pwd");
  assert.match(String(a.marker), /^MCPDONE[0-9a-f]{32}$/);
  assert.notEqual(a.marker, b.marker);
});

for (const mode of ["hang", "stale"] as const) {
  test(`${mode} cannot trigger a status query; timeout invalidates session`, async () => {
    const f = fixture(); f.state[mode] = true;
    const r = await runOs9(f.bridge, "mdir", 1000, f.timing);
    assert.equal(r.outcome, "timeout"); assert.equal(r.timedOut, true); assert.equal(r.shellReady, false);
    assert.equal(r.commandCompleted, false); assert.equal(r.status, null);
    assert.equal(f.calls.filter(c => c.cmd === "type").length, 1);
    assert.equal((await runOs9(f.bridge, "pwd", 1000, f.timing)).outcome, "shell_not_ready");
  });
}

for (const mode of ["lost", "unsupported"] as const) {
  test(`${mode} console is rejected before input`, async () => {
    const f = fixture(); f.state[mode] = true;
    const r = await runOs9(f.bridge, "mdir", 1000, f.timing);
    assert.equal(r.outcome, "shell_not_ready");
    assert.equal(f.calls.some(c => c.cmd === "type"), false);
  });
}

for (const mode of ["malformed", "unexpected", "echoOnly", "duplicate"] as const) {
  test(`rejects ${mode} marker at a returned prompt`, async () => {
    const f = fixture(); f.state[mode] = true;
    const r = await runOs9(f.bridge, "pwd", 1000, f.timing);
    assert.equal(r.outcome, "protocol_error"); assert.equal(r.commandCompleted, true);
    assert.equal(r.completed, false); assert.equal(r.status, null); assert.equal(r.shellReady, false);
  });
}

test("out-of-range status and changed epoch fail closed", async () => {
  const f = fixture(); f.state.code = "999";
  assert.equal((await runOs9(f.bridge, "pwd", 1000, f.timing)).outcome, "protocol_error");
  ready(f.bridge); f.state.epoch = 8;
  assert.equal((await runOs9(f.bridge, "pwd", 1000, f.timing)).outcome, "shell_not_ready");
});

for (const cmd of ["begin_run", "read_text_console", "type", "finish_run"]) {
  test(`bridge failure during ${cmd} is not a guest status`, async () => {
    const f = fixture(); f.state.failure = cmd;
    const r = await runOs9(f.bridge, "pwd", 1000, f.timing);
    assert.equal(r.outcome, "bridge_failure"); assert.equal(r.shellReady, false);
    assert.equal((await runOs9(f.bridge, "pwd", 1000, f.timing)).outcome, "shell_not_ready");
  });
}

test("invalid input cannot inject shell sequencing, expansion, control keys or change shell state", async () => {
  const f = fixture();
  for (const command of ["", " pwd", "pwd\n", "pwd\r", "pwd\nmdir", "pwd; echo bad", "echo %*", "dir *", "pwd | cat", "pwd &", "(pwd)",
    "echo {ENTER}", "echo $x", "echo >file", "echo 'x'", "echo \"x\"", "shell", "EX", "-x", "p=bad", "montype r", "a".repeat(59)]) {
    assert.throws(() => validateCommand(command), command);
    assert.equal((await runOs9(f.bridge, command, 1000, f.timing)).outcome, "invalid_input");
  }
  assert.equal(f.calls.length, 0);
  for (const value of [0, -1, 0.5, Infinity, "1000", null]) assert.equal((await runOs9(f.bridge, "pwd", value, f.timing)).outcome, "invalid_input");
  assert.equal((await runOs9(f.bridge, "nosuchcommand", 5000, f.timing)).completed, true);
});

test("MCP treats nonzero guest status as success and serializes mutating calls", async () => {
  const f = fixture(); f.state.code = "216";
  const h = createToolHandlers({ bridge: f.bridge, config: loadConfig({}, "/tmp/os9-run-tests") } as ToolDeps);
  const pending = h.os9_run!({ command: "nosuchcommand" });
  const busy = await h.os9_run!({ command: "pwd" });
  assert.equal((busy.structuredContent as { outcome: string }).outcome, "busy");
  assert.equal((await h.coco_type!({ text: "pwd" })).isError, true);
  const r = await pending;
  assert.equal(r.isError, false);
  assert.equal((r.structuredContent as { status: number }).status, 216);
});

test("real TCP bridge delivers status and disconnect failures", async () => {
  const f = fixture(), bridge = createBridgeServer(0);
  let peer: net.Socket | undefined;
  try {
    await bridge.start(); peer = net.createConnection(bridge.port, bridge.address);
    await new Promise<void>(resolve => peer!.once("connect", resolve));
    let buffer = "", disconnect = false;
    peer.on("data", chunk => {
      buffer += chunk.toString(); let newline: number;
      while ((newline = buffer.indexOf("\n")) >= 0) {
        const req = JSON.parse(buffer.slice(0, newline)); buffer = buffer.slice(newline + 1);
        if (disconnect) { peer!.destroy(); break; }
        void f.bridge.request(req.cmd, req.params).then(r => peer!.write(JSON.stringify({ ...r, id: req.id }) + "\n"));
      }
    });
    ready(bridge);
    assert.equal((await runOs9(bridge, "pwd")).completed, true);
    disconnect = true;
    assert.equal((await runOs9(bridge, "pwd")).outcome, "bridge_failure");
  } finally { peer?.destroy(); await bridge.stop(); }
});

test("lost prompt after status query times out with command completion preserved", async () => {
  const f = fixture(); f.state.markerHang = true;
  const r = await runOs9(f.bridge, "pwd", 1000, f.timing);
  assert.equal(r.outcome, "timeout"); assert.equal(r.timedOut, true);
  assert.equal(r.commandCompleted, true); assert.equal(r.completed, false);
  assert.equal(r.status, null); assert.equal(r.phase, "status_marker");
  assert.equal(f.calls.filter(c => c.cmd === "type").length, 2);
});

test("a reset/load at release cannot leave the session marked ready", async () => {
  const f = fixture(); f.state.invalidatedOnRelease = true;
  const r = await runOs9(f.bridge, "pwd", 1000, f.timing);
  assert.equal(r.outcome, "shell_not_ready"); assert.equal(r.shellReady, false);
  assert.equal((await runOs9(f.bridge, "pwd", 1000, f.timing)).outcome, "shell_not_ready");
});

test("raw keyboard input invalidates the cached ready session", async () => {
  const f = fixture();
  const request = f.bridge.request.bind(f.bridge);
  f.bridge.request = async (cmd, params) => cmd === "wait_idle"
    ? { id: "1", ok: true, result: { idle: true } } : request(cmd, params);
  const h = createToolHandlers({ bridge: f.bridge, config: loadConfig({}, "/tmp/os9-run-tests") } as ToolDeps);
  await h.coco_type!({ text: "pwd" });
  const r = await h.os9_run!({ command: "pwd" });
  assert.equal((r.structuredContent as { outcome: string }).outcome, "shell_not_ready");
});

// New policy tests wrap the existing fixture without altering strict tests.
function graphicsFixture(mode = "return") {
  const f = fixture(), original = f.bridge.request.bind(f.bridge);
  let graphicsReads = 0;
  f.bridge.request = async (cmd, params, timeout) => {
    if (cmd === "read_text_console" && f.state.phase === "command") {
      graphicsReads++;
      if (graphicsReads <= 2 || mode === "hang") {
        if (graphicsReads === 2 && mode === "disconnect") throw new Error("bridge disconnected");
        return { id: "g", ok: true, result: { supported: false, reasonCode: "unsupported_display",
          reason: "graphics mode", epoch: mode === "epoch" && graphicsReads === 2 ? 8 : 7 } };
      }
    }
    return original(cmd, params, timeout);
  };
  return f;
}
for (const enabled of [false, true]) test(`text execution with allow_graphics=${enabled}`, async () => {
  const f = fixture();
  assert.equal((await runOs9(f.bridge, "mdir", 5000, f.timing, enabled)).status, 0);
});
for (const code of ["000", "216"]) test(`graphics round trip preserves status ${code}`, async () => {
  const f = graphicsFixture(); f.state.code = code;
  const r = await runOs9(f.bridge, "probe", 5000, f.timing, true);
  assert.equal(r.completed, true); assert.equal(r.statusText, code);
  assert.equal(r.executionState, "COMPLETE"); assert.equal(r.displayDepartures, 1);
  assert.equal(r.consoleReturned, true); assert.equal(r.shellReady, true);
  assert.equal(f.calls.filter(c => c.cmd === "type").length, 2);
});
for (const [mode, outcome] of [["hang", "timeout"], ["disconnect", "bridge_failure"], ["epoch", "shell_not_ready"]]) {
  test(`graphics ${mode} fails closed`, async () => {
    const f = graphicsFixture(mode);
    const r = await runOs9(f.bridge, "probe", 1000, f.timing, true);
    assert.equal(r.outcome, outcome); assert.equal(r.completed, false); assert.equal(r.status, null);
    assert.equal(f.calls.filter(c => c.cmd === "type").length, 1);
    if (mode === "hang") { assert.equal(r.timedOut, true); assert.equal(r.timeoutReason, "console_not_returned"); }
    assert.equal((await runOs9(f.bridge, "pwd", 1000, f.timing)).outcome, "shell_not_ready");
  });
}
test("malformed marker after graphics is rejected", async () => {
  const f = graphicsFixture(); f.state.malformed = true;
  const r = await runOs9(f.bridge, "probe", 5000, f.timing, true);
  assert.equal(r.outcome, "protocol_error"); assert.equal(r.commandCompleted, true); assert.equal(r.status, null);
});
test("graphics returning to unchanged stale prompt cannot complete", async () => {
  const f = graphicsFixture(); f.state.stale = true;
  const r = await runOs9(f.bridge, "probe", 1000, f.timing, true);
  assert.equal(r.outcome, "timeout"); assert.equal(r.commandCompleted, false);
  assert.equal(r.consoleReturned, true); assert.equal(f.calls.filter(c => c.cmd === "type").length, 1);
});
test("default policy rejects graphics after sending command", async () => {
  const f = graphicsFixture();
  assert.equal((await runOs9(f.bridge, "probe", 5000, f.timing)).outcome, "shell_not_ready");
});
test("opt-in cannot bypass unsupported preflight or malformed console data", async () => {
  const f = fixture(); f.state.unsupported = true;
  assert.equal((await runOs9(f.bridge, "probe", 5000, f.timing, true)).outcome, "shell_not_ready");
  assert.equal(f.calls.some(c => c.cmd === "type"), false);
});
test("invalid graphics policy is rejected before input", async () => {
  const f = fixture();
  assert.equal((await runOs9(f.bridge, "probe", 5000, f.timing, "true")).outcome, "invalid_input");
  assert.equal(f.calls.length, 0);
});

test("graphics policy stays strict during status marker and on corrupt text", async () => {
  for (const phase of ["command", "marker"]) {
    const f = graphicsFixture(), request = f.bridge.request.bind(f.bridge);
    f.bridge.request = async (cmd, params, timeout) => {
      const response = await request(cmd, params, timeout);
      if (cmd === "read_text_console" && f.state.phase === phase && response.ok) {
        return { ...response, result: phase === "marker"
          ? { supported: false, reasonCode: "unsupported_display", epoch: 7 }
          : { ...view([prompt]), cells: "corrupt" } };
      }
      return response;
    };
    const r = await runOs9(f.bridge, "probe", 5000, f.timing, true);
    assert.equal(r.outcome, "shell_not_ready"); assert.equal(r.completed, false);
  }
});
test("MCP handler forwards opt-in without changing default policy", async () => {
  const f = graphicsFixture();
  const h = createToolHandlers({ bridge: f.bridge, config: loadConfig({}, "/tmp/os9-run-tests") } as ToolDeps);
  const r = await h.os9_run!({ command: "probe", allow_graphics: true });
  assert.equal(r.isError, false);
  assert.equal((r.structuredContent as { displayDepartures: number }).displayDepartures, 1);
});
