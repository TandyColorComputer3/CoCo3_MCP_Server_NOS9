import { execFile } from "node:child_process";
import { createHash } from "node:crypto";
import { chmod, mkdir, mkdtemp, open, readFile, writeFile } from "node:fs/promises";
import path from "node:path";

export interface StageCommand { command: string; args: string[]; stdout: string; stderr: string }
export type StageRunner = (command: string, args: string[]) => Promise<{ stdout: string; stderr: string }>;
const run: StageRunner = (command, args) => new Promise((resolve, reject) => {
  execFile(command, args, { timeout: 30_000, maxBuffer: 1024 * 1024, windowsHide: true },
    (error, stdout, stderr) => error ? reject(new Error(`${command}: ${error.message}\n${stderr}`)) : resolve({ stdout, stderr }));
});
const sha256 = (bytes: Buffer) => createHash("sha256").update(bytes).digest("hex");

/** OS-9 header layout: nitros9-reference defs/os9.d, M$* fields.
 * CRC authority is ToolShed ident (checked again on the installed bytes).
 * This milestone accepts ONE native program, not concatenated module packs.
 */
export function nativeModule(bytes: Buffer) {
  if (bytes.length < 17 || bytes.length > 65535 || bytes.readUInt16BE(0) !== 0x87cd
      || bytes.readUInt16BE(2) !== bytes.length || bytes[6] !== 0x11) {
    throw new Error("expected exactly one OS-9 Prgrm/6809-object module");
  }
  let parity = 0;
  for (const b of bytes.subarray(0, 9)) parity ^= b;
  if (parity !== 255) throw new Error("invalid module header parity");
  const nameOffset = bytes.readUInt16BE(4);
  if (nameOffset < 13 || nameOffset >= bytes.length - 4) throw new Error("invalid module name offset");
  let end = nameOffset;
  while (end < bytes.length - 4 && !(bytes[end]! & 0x80)) end++;
  if (!(bytes[end]! & 0x80)) throw new Error("unterminated module name");
  const name = Array.from(bytes.subarray(nameOffset, end + 1), b => String.fromCharCode(b & 0x7f)).join("");
  if (!/^[A-Za-z][A-Za-z0-9_-]{0,28}$/.test(name)) throw new Error("unsupported module name");
  const entry = bytes.readUInt16BE(9);
  if (entry < 13 || entry >= bytes.length - 3) throw new Error("invalid execution offset");
  return { name, type: "Prgrm", language: "6809 Obj", typeLanguage: "11", size: bytes.length,
    crc: bytes.subarray(-3).toString("hex").toUpperCase(), revision: bytes[7]! & 15,
    attributes: bytes[7]! >> 4, executionOffset: entry, dataSize: bytes.readUInt16BE(11) };
}

async function boundedFile(filename: string, limit: number): Promise<Buffer> {
  const file = await open(filename, "r");
  try {
    const info = await file.stat();
    if (!info.isFile() || info.size > limit) throw new Error(`expected a regular file of at most ${limit} bytes`);
    const data = Buffer.alloc(limit + 1);
    let count = 0;
    while (count < data.length) {
      const { bytesRead } = await file.read(data, count, data.length - count, count);
      if (!bytesRead) break;
      count += bytesRead;
    }
    if (count > limit) throw new Error("input grew beyond size limit");
    return data.subarray(0, count);
  } finally { await file.close(); }
}

export interface StageRequest {
  artifact: string;
  outputRoot: string;
  os9: string;
  /** Optional build record retained verbatim; never executed or treated as verified claims. */
  provenance?: string;
}

/** Prepare new media only. Does not mount, modify existing media, restore, or install into /dd.
 * Fresh mkdtemp ownership + fixed paths eliminate destination-overwrite races.
 */
export async function stageArtifact(req: StageRequest, runner: StageRunner = run) {
  const outputRoot = path.resolve(req.outputRoot);
  if (outputRoot.includes(",")) throw new Error("output root cannot contain ToolShed's image-path delimiter ','");
  const artifact = path.resolve(req.artifact);
  const bytes = await boundedFile(artifact, 65535);
  const module = nativeModule(bytes);
  const provenance = req.provenance ? await boundedFile(path.resolve(req.provenance), 1024 * 1024) : undefined;
  await mkdir(outputRoot, { recursive: true });
  const directory = await mkdtemp(path.join(outputRoot, "stage-"));
  const commands: StageCommand[] = [];
  async function command(args: string[]) {
    const result = await runner(req.os9, args);
    commands.push({ command: req.os9, args, ...result });
    return result.stdout + result.stderr;
  }
  try {
    const snapshot = path.join(directory, "artifact.module");
    const image = path.join(directory, "artifact.dsk");
    await writeFile(snapshot, bytes, { flag: "wx" });
    if (provenance) await writeFile(path.join(directory, "build-provenance.json"), provenance, { flag: "wx" });
    const versionOutput = await command([]);
    const version = /os9 from Toolshed[^\r\n]*/.exec(versionOutput)?.[0] ?? versionOutput.trim();
    async function inspect(target: string) {
      const report = await command(["ident", target]);
      if (!new RegExp(`Module CRC\\s*:\\s*\\$${module.crc} \\(Good\\)`, "i").test(report)
          || /\(Bad\)/i.test(report)) throw new Error("ToolShed did not verify the module CRC");
      const edition = /Edition\s*:\s*\$[0-9a-f]+\s+#(\d+)/i.exec(report);
      return { report, edition: edition ? Number(edition[1]) : null };
    }
    const inspected = await inspect(snapshot);
    // Verified native ToolShed 2.2 format/copy/attr syntax. No -r (replace) ever.
    await command(["format", "-e", "-t40", "-ds", image]);
    const guest = `${image},${module.name}`;
    await command(["copy", snapshot, guest]);
    await command(["attr", "-e", "-pe", "-pr", "-nw", guest]);
    const installed = await inspect(guest);
    if (installed.edition !== inspected.edition) throw new Error("installed module edition changed");
    const extracted = path.join(directory, "verified.module");
    await command(["copy", guest, extracted]);
    if (!(await readFile(extracted)).equals(bytes)) throw new Error("installed artifact differs from host bytes");
    const imageHash = sha256(await readFile(image));
    // Best-effort OS-level write protection is not a filesystem rollback mechanism.
    await chmod(image, 0o444);
    const result = {
      staged: true, installedInCanonicalVhd: false,
      hostArtifact: { path: artifact, snapshot, sha256: sha256(bytes) },
      module: { ...module, edition: inspected.edition },
      guestDestination: { image, path: `/${module.name}`, device: null },
      replacedExisting: false, stagingMethod: "fresh-artifact-floppy",
      image: { path: image, sha256: imageHash, tracks: 40, sides: 2, sectorsPerTrack: 18, bytesPerSector: 256 },
      requires: "cold boot with this image attached; do not restore a state made with different media bytes",
      provenance: provenance ? { path: path.join(directory, "build-provenance.json"), sha256: sha256(provenance), claimsVerified: false } : null,
      toolshed: { executable: req.os9, version, hostIdent: inspected.report, installedIdent: installed.report },
      commands,
    };
    await writeFile(path.join(directory, "manifest.json"), JSON.stringify(result, null, 2) + "\n", { flag: "wx" });
    return result;
  } catch (error) {
    // No successful manifest is published. Keep bounded diagnostics for investigation.
    await writeFile(path.join(directory, "failure.json"), JSON.stringify({ staged: false, error: String(error), commands }, null, 2), { flag: "wx" });
    throw new Error(`staging failed in ${directory}: ${String(error)}`);
  }
}
