import { stageArtifact } from "./os9-stage.js";

try {
  const values: Record<string, string> = {};
  const args = process.argv.slice(2);
  for (let i = 0; i < args.length; i += 2) {
    const key = args[i]!;
    const value = args[i + 1];
    if (!["--artifact", "--output-root", "--os9", "--provenance"].includes(key) || !value || values[key]) {
      throw new Error("usage: os9-stage --artifact MODULE --output-root DIRECTORY --os9 TOOLSHED_OS9 [--provenance BUILD_JSON]");
    }
    values[key] = value;
  }
  if (!values["--artifact"] || !values["--output-root"] || !values["--os9"]) throw new Error("--artifact, --output-root and --os9 are required");
  const result = await stageArtifact({ artifact: values["--artifact"], outputRoot: values["--output-root"],
    os9: values["--os9"], provenance: values["--provenance"] });
  process.stdout.write(JSON.stringify(result, null, 2) + "\n");
} catch (error) {
  process.stderr.write(JSON.stringify({ staged: false, error: String(error) }) + "\n");
  process.exitCode = 1;
}
