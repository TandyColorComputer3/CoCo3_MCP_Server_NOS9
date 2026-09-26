import { randomBytes } from "node:crypto";
import { stat } from "node:fs/promises";
import path from "node:path";
import { performance } from "node:perf_hooks";
import type { BridgeClient } from "./bridge-server.js";
import type { AppConfig } from "./config.js";

export interface ReadyResult {
  [key: string]: unknown;
  ready: boolean;
  state: string;
  loadScheduled: boolean;
  loadCompleted: boolean;
  shellVerified: boolean;
  timedOut: boolean;
  status: string;
  phase: string;
  elapsedMs: number;
}

/** Only the verified EOU native 80x25, attributed, unscrolled hardware text mode.
 * GIME renderer: MAME mame0289 src/mame/trs/gime.cpp, get_video_base,
 * get_lines_per_row, record_full_body_scanline, update_geometry, hires_font.
 * High character bit is ignored by the font renderer. Non-ASCII glyphs stay
 * unknown: this is a readiness reader, not a general terminal transcript.
 */
export function decodeConsole(value: unknown): string[] {
  const data = value as { supported?: boolean; registers?: number[]; cells?: string; reason?: string };
  if (!data || data.supported !== true) throw new Error(data?.reason ?? "unsupported console");
  const r = data.registers;
  if (!Array.isArray(r) || r.length !== 16 || r.some(v => !Number.isInteger(v) || v < 0 || v > 255)
      || (r[0]! & 0x80) !== 0 || (r[8]! & 0x87) !== 4 || (r[9]! & 0x75) !== 0x75
      || r[12] !== 0 || r[15] !== 0) throw new Error("unsupported GIME text layout");
  if (typeof data.cells !== "string" || !/^[0-9a-f]{8000}$/i.test(data.cells)) throw new Error("invalid console cells");
  const bytes = Buffer.from(data.cells, "hex");
  return Array.from({ length: 25 }, (_, y) => {
    let line = "";
    for (let x = 0; x < 80; x++) {
      const c = bytes[(y * 80 + x) * 2]! & 0x7f;
      line += c >= 32 && c <= 126 ? String.fromCharCode(c) : "\ufffd";
    }
    return line.trimEnd();
  });
}

export function hasProbe(rows: string[], marker: string, prompt: string): boolean {
  let last = rows.length - 1;
  while (last >= 0 && rows[last] === "") last--;
  const emitted = rows.findIndex(row => row === marker);
  return emitted >= 0 && last > emitted && rows[last] === prompt;
}

class ReadyFailure extends Error {
  constructor(readonly status: string, message: string, readonly timedOut = false) { super(message); }
}

/** Host-only dependencies for deterministic deadline tests. Production uses monotonic time. */
export interface ReadyClock {
  now(): number;
  poll(): Promise<void>;
  nonce(): string;
}
const clock: ReadyClock = {
  now: () => performance.now(),
  poll: () => new Promise(resolve => setTimeout(resolve, 50)),
  nonce: () => randomBytes(8).toString("hex"),
};

export async function restoreReady(config: AppConfig, bridge: BridgeClient, timeoutMs = 30_000,
  timing: ReadyClock = clock): Promise<ReadyResult> {
  const started = timing.now();
  const result: ReadyResult = { ready: false, state: config.nitros9ReadyState, loadScheduled: false,
    loadCompleted: false, shellVerified: false, timedOut: false, status: "pending", phase: "preflight", elapsedMs: 0 };
  const token = timing.nonce();
  const prompt = `MCP${token}:`;
  const marker = `READY${token}`;
  let epoch: number | undefined;
  let acquired = false;
  const deadline = started + timeoutMs;
  const remaining = () => deadline - timing.now();
  function checkDeadline(): void {
    if (remaining() <= 0) throw new ReadyFailure(result.loadCompleted ? "shell_handshake_failed" : "post_load_timeout",
      `deadline exceeded during ${result.phase}`, true);
  }
  async function request(cmd: string, params: Record<string, unknown> = {}): Promise<Record<string, unknown>> {
    checkDeadline();
    try {
      const response = await bridge.request(cmd, { ...params, token, ...(epoch === undefined ? {} : { epoch }) },
        Math.max(1, Math.min(2000, remaining())));
      if (!response.ok) throw new Error(response.error);
      if (!response.result || typeof response.result !== "object") throw new Error("invalid bridge response");
      return response.result as Record<string, unknown>;
    } catch (error) {
      if (error instanceof ReadyFailure) throw error;
      throw new ReadyFailure("bridge_failure", error instanceof Error ? error.message : String(error), remaining() <= 0);
    }
  }
  async function screen(): Promise<{ rows: string[]; idle: boolean }> {
    const data = await request("read_text_console");
    try {
      const rows = decodeConsole(data);
      result.console = { columns: 80, rows: 25, source: "physical_ram", text: rows.join("\n") };
      return { rows, idle: data.idle === true };
    } catch (error) {
      throw new ReadyFailure("shell_handshake_failed", error instanceof Error ? error.message : String(error));
    }
  }
  async function untilScreen(predicate: (rows: string[]) => boolean): Promise<void> {
    for (;;) {
      checkDeadline();
      const observed = await screen();
      if (observed.idle && predicate(observed.rows)) return;
      await timing.poll();
    }
  }
  try {
    if (!Number.isInteger(timeoutMs) || timeoutMs < 1000 || timeoutMs > 120_000) {
      throw new ReadyFailure("invalid_arguments", "timeout_ms must be an integer from 1000 through 120000");
    }
    const file = path.join(config.stateDir, config.mameMachine, `${config.nitros9ReadyState}.sta`);
    result.file = file;
    try {
      const info = await stat(file);
      if (!info.isFile() || info.size === 0) throw new ReadyFailure("state_missing", "state is empty or not a file");
    } catch (error) {
      if (error instanceof ReadyFailure) throw error;
      if ((error as NodeJS.ErrnoException).code === "ENOENT") throw new ReadyFailure("state_missing", "state file does not exist");
      throw new ReadyFailure("state_unreadable", error instanceof Error ? error.message : String(error));
    }
    result.phase = "load";
    const scheduled = await request("load_state_tracked", { name: config.nitros9ReadyState });
    if (scheduled.scheduled !== true || scheduled.token !== token) throw new ReadyFailure("bridge_failure", "invalid load acknowledgement");
    acquired = true;
    result.loadScheduled = true;
    result.phase = "post_load";
    for (;;) {
      const status = await request("load_state_status");
      if (status.token !== token || status.invalidated === true) throw new ReadyFailure("restore_invalidated", "load operation invalidated");
      if (status.completed === true) {
        if (!Number.isInteger(status.epoch)) throw new ReadyFailure("bridge_failure", "missing post-load epoch");
        epoch = status.epoch as number;
        result.loadCompleted = true;
        result.postLoadEpoch = epoch;
        break;
      }
      await timing.poll();
    }
    result.phase = "shell_candidate";
    const candidate = await screen();
    // Live fixture's raw character cells contain {Term|02}/DD:. Accept the
    // documented displayed form too; never type into arbitrary application text.
    const last = candidate.rows.filter(row => row !== "").at(-1) ?? "";
    if (!candidate.idle || !/^(?:\{Term\|\d+\}|<Term:\d+>)[^\s]*:$/.test(last)
        || candidate.rows.some(row => row.includes(token))) {
      throw new ReadyFailure("shell_handshake_failed", "checkpoint does not show the expected idle Term shell");
    }
    result.phase = "prompt";
    // Shell+ PROMPTS: <=21 chars, no expansion tokens. Verified on this EOU 2.2a.
    await request("type", { text: `p=${prompt}{ENTER}` });
    await untilScreen(rows => rows.filter(row => row !== "").at(-1) === prompt);
    result.phase = "probe";
    await request("type", { text: `echo ${marker}{ENTER}` });
    await untilScreen(rows => hasProbe(rows, marker, prompt));
    // Check operation ownership again after the last observation.
    const final = await request("load_state_status");
    if (final.invalidated === true || final.completed !== true || final.epoch !== epoch || final.token !== token) {
      throw new ReadyFailure("restore_invalidated", "load operation changed during handshake");
    }
    result.ready = true;
    result.shellVerified = true;
    result.status = "ready";
    result.phase = "ready";
    result.handshake = { prompt, marker, markerObserved: true, promptReturned: true };
  } catch (error) {
    const failure = error instanceof ReadyFailure ? error : new ReadyFailure("bridge_failure", String(error));
    result.status = failure.status;
    result.timedOut = failure.timedOut;
    result.error = failure.message;
  } finally {
    if (acquired) {
      try {
        const ended = await bridge.request("finish_restore", { token }, 1000);
        if (!ended.ok) throw new Error(ended.error);
      } catch (error) {
        result.ready = false;
        result.cleanupError = error instanceof Error ? error.message : String(error);
        if (result.status === "ready") result.status = "bridge_failure";
      }
    }
    result.elapsedMs = Math.round(timing.now() - started);
  }
  return result;
}
