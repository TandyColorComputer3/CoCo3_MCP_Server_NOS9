import assert from "node:assert/strict";
import test from "node:test";
import { mkdtemp, readFile, readdir, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import path from "node:path";
import { nativeModule, stageArtifact, type StageRunner } from "../src/os9-stage.js";

// Real lwasm 4.22 output from examples/staging/stgprobe.asm; ToolShed 2.2 CRC E0136B.
const probe = Buffer.from("87cd003b000d1181130016010073746770726f62e501308d000f108e000f8601103f8c25015f103f064d43502053544147494e47204f4b0de0136b", "hex");
const ident = "Module CRC : $E0136B (Good)\nEdition    : $01  #1\n";
async function setup(t: { after: (fn: () => Promise<void>) => void }) {
  const root = await mkdtemp(path.join(tmpdir(), "os9-stage-test-"));
  t.after(() => rm(root, { recursive: true, force: true }));
  const artifact = path.join(root, "input module");
  await writeFile(artifact, probe);
  return { artifact, outputRoot: path.join(root, "out with spaces"), os9: "toolshed os9" };
}
function fake(options: { fail?: string; crc?: boolean; corrupt?: boolean; mutate?: string } = {}) {
  const calls: string[][] = [];
  const runner: StageRunner = async (_command, args) => {
    calls.push(args);
    if (options.fail !== undefined && args[0] === options.fail) throw new Error("tool failed");
    if (args.length === 0) return { stdout: "os9 from Toolshed 2.2", stderr: "" };
    if (args[0] === "ident") return { stdout: options.crc === false ? ident.replace("Good", "Bad") : ident, stderr: "" };
    if (args[0] === "format") await writeFile(args.at(-1)!, "fresh image");
    if (args[0] === "copy" && args[1]!.includes(",")) await writeFile(args[2]!, options.corrupt ? Buffer.alloc(59) : probe);
    if (options.mutate) await writeFile(options.mutate, "changed after snapshot");
    return { stdout: "", stderr: "" };
  };
  return { runner, calls };
}

test("native module inspection reports real header identity and rejects packs/bad headers", () => {
  assert.equal(nativeModule(probe).name, "stgprobe");
  assert.equal(nativeModule(probe).crc, "E0136B");
  assert.equal(nativeModule(probe).revision, 1);
  for (const bad of [Buffer.alloc(10), Buffer.concat([probe, probe]), Buffer.from(probe)]) {
    if (bad.length === probe.length) bad[8] ^= 1;
    assert.throws(() => nativeModule(bad));
  }
});

test("staging verifies installed bytes, retains provenance and never replaces a destination", async t => {
  const req = await setup(t);
  const provenance = path.join(path.dirname(req.artifact), "build.json");
  await writeFile(provenance, '{"assembler":"lwasm 4.22"}');
  const tool = fake();
  const a = await stageArtifact({ ...req, provenance }, tool.runner);
  const b = await stageArtifact(req, fake().runner);
  assert.notEqual(a.image.path, b.image.path);
  assert.equal(a.replacedExisting, false);
  assert.equal(a.installedInCanonicalVhd, false);
  assert.equal(a.module.edition, 1);
  assert.equal(a.guestDestination.device, null);
  assert.equal(a.guestDestination.path, "/stgprobe");
  assert.equal(a.provenance?.claimsVerified, false);
  assert.ok(tool.calls.some(a => a.join(" ").startsWith("format -e -t40 -ds")));
  assert.ok(tool.calls.every(a => !a.includes("-r")));
  assert.ok((await readFile(a.hostArtifact.snapshot)).equals(probe));
  assert.ok((await readFile(path.join(path.dirname(a.image.path), "manifest.json"), "utf8")).includes(a.image.sha256));
});

test("staging reads a stable snapshot even if the original artifact changes", async t => {
  const req = await setup(t);
  const r = await stageArtifact(req, fake({ mutate: req.artifact }).runner);
  assert.ok((await readFile(r.hostArtifact.snapshot)).equals(probe));
});

for (const failure of [{ fail: "copy" }, { crc: false }, { corrupt: true }]) {
  test(`staging fails closed without a manifest: ${JSON.stringify(failure)}`, async t => {
    const req = await setup(t);
    await assert.rejects(stageArtifact(req, fake(failure).runner), /staging failed/);
    const jobs = await readdir(req.outputRoot);
    assert.equal(jobs.length, 1);
    const files = await readdir(path.join(req.outputRoot, jobs[0]!));
    assert.ok(files.includes("failure.json"));
    assert.ok(!files.includes("manifest.json"));
  });
}

test("rejects ToolShed delimiter in destination before running commands", async t => {
  const req = await setup(t);
  const tool = fake();
  await assert.rejects(stageArtifact({ ...req, outputRoot: req.outputRoot + ",bad" }, tool.runner), /delimiter/);
  assert.equal(tool.calls.length, 0);
});
