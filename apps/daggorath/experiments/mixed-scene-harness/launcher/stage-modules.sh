#!/bin/sh
# Binary-safe staging into a disposable RBF /d1 image. Never use a canonical
# disk as $1 or reuse an existing destination image.
set -eu
if [ "$#" -ne 3 ]; then
  echo "usage: $0 DISPOSABLE_SOURCE_D1.dsk NEW_PRIVATE_OUTPUT.dsk MODULE_DIR" >&2
  exit 2
fi
source_disk=$1
target_disk=$2
module_dir=$3
os9=${TOOLSHED_OS9:-/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9}
[ -f "$source_disk" ] || { echo "missing source disk: $source_disk" >&2; exit 2; }
[ ! -e "$target_disk" ] || { echo "refusing to overwrite output disk: $target_disk" >&2; exit 2; }
[ -x "$os9" ] || { echo "ToolShed os9 executable unavailable: $os9" >&2; exit 2; }
mkdir -p "$(dirname "$target_disk")"
cp "$source_disk" "$target_disk"
tmp=$(mktemp -d "${TMPDIR:-/private/tmp}/msh13-stage.XXXXXX")
trap 'rm -rf "$tmp"' EXIT HUP INT TERM

verify_sha() {
  file=$1 expected=$2
  actual=$(shasum -a 256 "$file" | awk '{print $1}')
  [ "$actual" = "$expected" ] || { echo "SHA-256 mismatch for $file: $actual" >&2; exit 1; }
}
verify_module() {
  name=$1 expected=$2 expected_crc=$3
  host="$module_dir/$name"
  [ -f "$host" ] || { echo "missing module $host" >&2; exit 1; }
  verify_sha "$host" "$expected"
  "$os9" copy -r "$host" "$target_disk,$name"
  "$os9" attr -e -r -pe -pr "$target_disk,$name"
  "$os9" ident "$target_disk,$name" > "$tmp/$name.ident"
  grep -q '(Good)' "$tmp/$name.ident" || { cat "$tmp/$name.ident" >&2; exit 1; }
  if [ -n "$expected_crc" ]; then
    grep -qi "$expected_crc" "$tmp/$name.ident" || { cat "$tmp/$name.ident" >&2; exit 1; }
  fi
  "$os9" copy "$target_disk,$name" "$tmp/$name.extracted"
  cmp "$host" "$tmp/$name.extracted"
  printf '%s  %s\n' "$(shasum -a 256 "$tmp/$name.extracted" | awk '{print $1}')" "$name"
}

verify_module msharness "$(python3 -c 'import json,sys;print(json.load(open(sys.argv[1]))["sha256"])' "$module_dir/build-result.json")" ""
verify_module dodhbp d6e092562d7b2a69b55df428ae7acd03f5d8a8550576906c192ca7b08bf5a086 8890F4
verify_module dhbpack 289e77066d9fe11da4528df3aac5e1b6730543cafc720d4fd6558ad92b50ab1e ""
"$os9" dir "$target_disk" > "$tmp/d1-directory.txt"
cp "$tmp/d1-directory.txt" "${target_disk}.directory.txt"
printf 'private D1 image: %s\n' "$target_disk"
printf 'private D1 SHA-256: %s\n' "$(shasum -a 256 "$target_disk" | awk '{print $1}')"
echo "No -l text conversion was used; each round-trip extraction matched the host bytes."
