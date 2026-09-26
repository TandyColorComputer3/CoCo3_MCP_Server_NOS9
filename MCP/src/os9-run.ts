import { randomBytes } from "node:crypto";
import { performance } from "node:perf_hooks";
import type { BridgeClient } from "./bridge-server.js";
import { decodeConsole, type ReadyResult } from "./os9-ready.js";

interface Session { prompt: string; epoch: number }
const sessions = new WeakMap<BridgeClient, Session>();
export function forgetShell(bridge: BridgeClient): void { sessions.delete(bridge); }
export function rememberShell(bridge: BridgeClient, ready: ReadyResult): void {
  forgetShell(bridge);
  const prompt = (ready.handshake as { prompt?: unknown } | undefined)?.prompt;
  if (ready.ready && typeof prompt === "string" && /^MCP[0-9a-f]{16}:$/.test(prompt) && Number.isInteger(ready.postLoadEpoch)) {
    sessions.set(bridge, { prompt, epoch: ready.postLoadEpoch as number });
  }
}

export interface RunResult {
  [key: string]: unknown;
  command: string;
  completed: boolean;
  commandCompleted: boolean;
  status: number | null;
  statusText: string | null;
  timedOut: boolean;
  shellReady: boolean;
  outcome: string;
  phase: string;
  elapsedMs: number;
  outputComplete: false;
}
export function runResult(command: string, outcome = "pending"): RunResult {
  return { command, completed: false, commandCompleted: false, status: null, statusText: null,
    timedOut: false, shellReady: false, outcome, phase: "preflight", elapsedMs: 0, outputComplete: false };
}

// Shell+ control words (bundled shellplus2.2a.asm CmdList) and known console
// replacement/configuration programs cannot preserve this prompt protocol.
const shellControl = new Set(("chd chx cd cx ex w x p t setpr v pause if then else fi endif clrif goto onerr l " +
  "shell gshell login tsmon montype display xmode iniz deiniz").split(" "));
export function validateCommand(command: unknown): asserts command is string {
  // Fits the 80-column input echo with the 20-character session prompt. No
  // Shell+ separators, expansion, redirects, coded keys, quotes or control bytes.
  if (typeof command !== "string" || command.length > 58 || /[^\x20-\x7e]/.test(command)
      || !/^[A-Za-z][A-Za-z0-9_-]*(?: +[A-Za-z0-9_./-]+)*$/.test(command)) {
    throw new Error("use one foreground program with simple arguments (1-58 ASCII characters); shell metacharacters and coded keys are unsupported");
  }
  if (shellControl.has(command.split(" ")[0]!.toLowerCase())) throw new Error("shell/console control commands are unsupported");
}
const lastLine = (rows: string[]) => rows.filter(row => row !== "").at(-1) ?? "";

/** Parse only the final, returned-prompt screen; partial marker output is normal
 * while the guest is writing. Old marker rows are explicitly ignored.
 */
export function parseMarker(rows: string[], marker: string, prompt: string, oldRows: Set<string>): string {
  const candidates = rows.filter(row => row.startsWith(marker));
  if (candidates.length !== 1 || !new RegExp(`^${marker} [0-9]{3}$`).test(candidates[0] ?? "")) {
    const unexpected = rows.some(row => row.startsWith("MCPDONE") && !oldRows.has(row));
    throw new Error(unexpected ? "malformed or unexpected completion marker" : "completion marker missing");
  }
  const status = candidates[0]!.slice(marker.length + 1);
  if (Number(status) > 255 || lastLine(rows) !== prompt || rows.indexOf(candidates[0]!) >= rows.lastIndexOf(prompt)) {
    throw new Error("invalid status or prompt ordering");
  }
  return status;
}

export interface RunClock { now(): number; poll(): Promise<void>; nonce(): string }
const clock: RunClock = {
  now: () => performance.now(), poll: () => new Promise(resolve => setTimeout(resolve, 25)),
  nonce: () => randomBytes(16).toString("hex"),
};
class RunFailure extends Error {
  constructor(readonly outcome: string, message: string) { super(message); }
}

export async function runOs9(bridge: BridgeClient, command: unknown, timeoutMs: unknown = 30_000,
  timing: RunClock = clock, allowGraphics: unknown = false): Promise<RunResult> {
  const started = timing.now();
  const result = runResult(typeof command === "string" ? command : "");
  result.allowGraphics = allowGraphics === true;
  result.displayDepartures = 0;
  result.consoleReturned = false;
  result.executionState = "READY_TERM";
  let acquired = false;
  let active = false;
  const token = timing.nonce();
  const marker = `MCPDONE${token}`;
  try {
    try {
      validateCommand(command);
      if (typeof allowGraphics !== "boolean") throw new Error("allow_graphics must be boolean");
      if (typeof timeoutMs !== "number" || !Number.isInteger(timeoutMs) || timeoutMs < 1000 || timeoutMs > 120000) {
        throw new Error("timeout_ms must be an integer from 1000 through 120000");
      }
    } catch (error) { throw new RunFailure("invalid_input", String(error)); }
    const session = sessions.get(bridge);
    if (!session) throw new RunFailure("shell_not_ready", "call os9_restore_ready first");
    active = true;
    const deadline = started + (timeoutMs as number);
    const check = () => { if (timing.now() >= deadline) throw new RunFailure("timeout", `deadline exceeded during ${result.phase}`); };
    async function request(cmd: string, params: Record<string, unknown> = {}): Promise<Record<string, unknown>> {
      check();
      try {
        const response = await bridge.request(cmd, { ...params, token, epoch: session!.epoch }, Math.max(1, Math.min(2000, deadline - timing.now())));
        if (!response.ok) {
          if (/operation invalidated|epoch mismatch/.test(response.error)) throw new RunFailure("shell_not_ready", response.error);
          throw new Error(response.error);
        }
        if (!response.result || typeof response.result !== "object") throw new Error("invalid bridge response");
        return response.result as Record<string, unknown>;
      } catch (error) {
        if (error instanceof RunFailure) throw error;
        if (timing.now() >= deadline && error instanceof Error && error.message.startsWith("timeout waiting for")) {
          throw new RunFailure("timeout", `deadline exceeded during ${result.phase}`);
        }
        throw new RunFailure("bridge_failure", error instanceof Error ? error.message : String(error));
      }
    }
    async function screen(tolerateDisplay = false): Promise<{ rows: string[]; idle: boolean } | null> {
      const data = await request("read_text_console");
      if (data.epoch !== session!.epoch) throw new RunFailure("shell_not_ready", "console epoch changed");
      if (tolerateDisplay && data.supported === false && data.reasonCode === "unsupported_display") {
        if (result.executionState !== "AWAITING_TERM") result.displayDepartures = Number(result.displayDepartures) + 1;
        result.executionState = "AWAITING_TERM";
        result.consoleReturned = false;
        return null;
      }
      try { return { rows: decodeConsole(data), idle: data.idle === true }; }
      catch (error) { throw new RunFailure("shell_not_ready", String(error)); }
    }
    async function freshPrompt(tolerateDisplay = false, baseline: string[] = []): Promise<string[]> {
      let sawActivity = false;
      for (;;) {
        const view = await screen(tolerateDisplay);
        if (view === null) {
          // A display departure is not evidence of a new shell prompt.
          sawActivity = false;
        } else {
          if (result.executionState === "AWAITING_TERM") {
            result.consoleReturned = true;
            result.executionState = "AWAITING_FRESH_PROMPT";
          }
          if (lastLine(view.rows) !== session!.prompt) sawActivity = true;
          // After a departure, also permit a changed returned screen (command
          // echo/output + new prompt). An unchanged saved prompt is never enough.
          const changed = JSON.stringify(view.rows) !== JSON.stringify(baseline);
          const returnedDisplay = tolerateDisplay && Number(result.displayDepartures) > 0;
          if ((sawActivity || (returnedDisplay && changed)) && (!returnedDisplay || changed)
              && view.idle && lastLine(view.rows) === session!.prompt) return view.rows;
        }
        check();
        await timing.poll();
      }
    }
    const lease = await request("begin_run");
    if (lease.token !== token || lease.epoch !== session.epoch) throw new RunFailure("bridge_failure", "invalid console lease");
    acquired = true;
    const initial = await screen();
    if (!initial || !initial.idle || lastLine(initial.rows) !== session.prompt) throw new RunFailure("shell_not_ready", "verified prompt is not idle");
    result.marker = marker;
    result.phase = "command";
    result.executionState = "COMMAND_RUNNING";
    await request("type", { text: `${command}{ENTER}` });
    const returned = await freshPrompt(allowGraphics === true, initial.rows);
    result.commandCompleted = true;
    result.commandPromptMs = Math.round(timing.now() - started);
    result.phase = "status_marker";
    result.executionState = "STATUS_MARKER";
    // Verified Shell+ v2.2a protocol: %* on a NEW line, after a fresh prompt.
    // A semicolon tail is skipped on errors and same-line %* is pre-expanded.
    // docs/architecture/NITROS9_COMMAND_COMPLETION.md, sections 3 and 7.
    await request("type", { text: `echo ${marker} %*{ENTER}` });
    result.executionState = "AWAITING_FINAL_PROMPT";
    const finalRows = await freshPrompt();
    try { result.statusText = parseMarker(finalRows, marker, session.prompt, new Set(returned)); }
    catch (error) { throw new RunFailure("protocol_error", String(error)); }
    result.status = Number(result.statusText);
    result.completed = true;
    result.shellReady = true;
    result.outcome = "completed";
    result.phase = "complete";
    result.executionState = "COMPLETE";
  } catch (error) {
    const failure = error instanceof RunFailure ? error : new RunFailure("bridge_failure", String(error));
    result.outcome = failure.outcome;
    result.timedOut = failure.outcome === "timeout";
    result.error = failure.message;
    if (result.timedOut && result.executionState === "AWAITING_TERM") result.timeoutReason = "console_not_returned";
  } finally {
    if (acquired) {
      try {
        const released = await bridge.request("finish_run", { token }, 1000);
        if (!released.ok || (released.result as { released?: boolean })?.released !== true) throw new Error("console release failed");
        if ((released.result as { invalidated?: boolean }).invalidated) throw new RunFailure("shell_not_ready", "console invalidated before release");
      } catch (error) {
        result.shellReady = false;
        result.outcome = error instanceof RunFailure ? error.outcome : "bridge_failure";
        result.cleanupError = String(error);
      }
    }
    if (active && result.outcome !== "completed") forgetShell(bridge);
    result.elapsedMs = Math.round(timing.now() - started);
  }
  return result;
}
