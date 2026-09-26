# OS9 ROF disassembler: object knowledge potential

[Navigation](README.md). Snapshot `a66579f5e3a933b36ae41be96a4aa7f582335d52`. Repository calls itself KAOS Motorola MC6809 Disassembler for OS-9 Relocatable Object Files; source is MIT licensed. Read [rof.h](/Volumes/SEDONA/Projects/os9rof-reference/rof.h), [roflib.c](/Volumes/SEDONA/Projects/os9rof-reference/roflib.c), [disasm.c](/Volumes/SEDONA/Projects/os9rof-reference/disasm.c), [disasm_op.c](/Volumes/SEDONA/Projects/os9rof-reference/disasm_op.c), [rdump.c](/Volumes/SEDONA/Projects/os9rof-reference/rdump.c).

## Format and metadata actually parsed

`LoadROF` reads the classic signature **0x62CD2387**, then type/language, assembler-valid byte, creation date, edition, code/data/uninitialized-data/direct-page sizes, stack size, execution entry and NUL-terminated object name. It loads global definitions, object code, initialized DP/data bytes, external references and local references. The CLI loops over concatenated objects; help describes `.r` and `.l` inputs. This is not a linked OS-9 module decoder or a general archive autodetector.

| Information | Representation / useful capability |
|---|---|
| Global definitions | Symbol name, flags, offset; code/data/DP/init distinctions |
| External references | Imported symbol names with reference sites; useful dependency and unresolved-symbol inventory |
| Local references | Relocation sites and section/width flags; enables symbolic reconstruction rather than raw numeric operands |
| Flag vocabulary | F_RELATIVE, F_NEGATE, CODLOC, DIRLOC, F_BYTE, CODENT, DIRENT, INIENT in `rof.h` |
| Section layout | Code, initialized/uninitialized data and direct-page data are distinct, with file offsets retained |
| Output | `TraceObjectCode` plus `DisasmObjectCode`, reference-label lookup in [genasm.c](/Volumes/SEDONA/Projects/os9rof-reference/genasm.c), OS-9 call names from [os9calls.c](/Volumes/SEDONA/Projects/os9rof-reference/os9calls.c) |

Flag definitions are format evidence, not proof all combinations are implemented correctly: `GetReference` contains a FIXME about code/data discrimination, and AddReference flags unsupported bits. Do not infer source lines, types or complete debug information from symbol/relocation records.

## CPU and decoding limits

**6309 support is explicitly a TODO** near the start of `disasm.c`. Tables in `disasm_op.c` cover the 6809 base and 0x10/0x11 opcode pages; register/indexed-mode definitions are 6809-oriented and reserve illegal encodings. No selectable 6309 mode or TFM/LDQ/DIVQ/MULD decoder was found. A 6309 object can therefore be misdecoded or classified as data/illegal instructions. Do not use its output as CPU truth for our HD6309 target.

The tracer uses 64 KiB analysis arrays and entry/control-flow information. Static tracing is not execution: indirect targets, mixed code/data and alternate entry points can defeat it. `MAX_OS9CALLS=0x91` and a hard-coded call table require comparison with the selected NitrOS-9 defs before relying on modern call names.

## Integration readiness and concrete defects

- Build files are Visual Studio `.sln/.vcxproj`; code uses `<io.h>`, `filelength`, `stricmp` and arithmetic on `fpos_t`. A native macOS/Linux build has not been established. No executable was built here.
- Help advertises `-a`, but the parser has no `case 'a'`. It accepts undocumented `-i` to write object-name `.asm` files. `-r` controls DumpROFInfo; `-g`/`-o` are set but not consumed elsewhere in the inspected CLI. Treat usage text as weaker evidence than code.
- `FreeROF` tests `objectCode == NULL` before freeing and does not free all allocated fields; malformed-input handling is not a hardened parser contract. Some reads lack comprehensive length checks. Do not embed it unchanged as an untrusted-file MCP service.
- It recognizes classic ROF, not every file called `.o`, `.r`, `.a` or `.l`. **CMOC/LWTOOLS output format compatibility was not demonstrated.** Detect magic/dialect and retain native listings/maps; reject unsupported formats rather than feed them to this decoder.

## Proposed compile/link/debug role

```text
compile/assemble → identify object dialect → symbols/sections/relocations
                → link + retain map/listing → linked OS-9 module fingerprint
                → load into disposable guest → process/module address correlation
```

For classic ROF, study `ReadROFHeader`, `LoadGlobals`, `LoadExtRefs`, `LoadLocalRefs`, `AddReference` and `GetReference` as a starting data model. An MCP could eventually report module/section sizes, exports, imports and relocation sites with source provenance, then resolve them using a link map. Neither a pre-link offset nor a linked logical address is automatically a physical MAME address.

First useful step is an **object inspection adapter**, not a promise of full source debugging or 6309 disassembly. It should expose detected format/CPU/confidence, bounded parse errors and artifact identity. Add fixture coverage for each toolchain's actual output before building runtime-symbol tools.

Cross-reference `/dd/SOURCECODE/C/RMA`, `C/RLINK`, `C/RDUMP`, `C/MAMOU` through [development tools](../source-index/DEVELOPMENT_TOOLS.md), [modules](../source-index/MODULES.md), [6309](../source-index/CPU_6309.md), [memory/MMU](../source-index/MEMORY_MMU.md), and current upstream [defs/os9.d](/Volumes/SEDONA/Projects/nitros9-reference/defs/os9.d). The historical corpus and this decoder offer complementary evidence, not authority for an unverified modern binary format.
