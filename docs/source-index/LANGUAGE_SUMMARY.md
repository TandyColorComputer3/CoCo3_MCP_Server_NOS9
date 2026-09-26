# SOURCECODE language and file-type summary

Counts are a reproducible triage of file contents plus filenames, not a compiler/parser result. Archives remain opaque; language totals describe visible files only. A source extension alone does not establish language.

## Content classification

| Language / representation | Files |
|---|---:|
| 6809-family assembly (CPU not certified) | 726 |
| not applicable | 586 |
| BASIC09 | 258 |
| 6809-family assembly (6309 indicators) | 205 |
| unknown | 138 |
| C | 130 |
| make / OS-9 command script | 105 |
| assembly listing (not source) | 83 |
| prose | 53 |

## Artifact types

| Type | Files |
|---|---:|
| source | 1259 |
| OS-9 module signature (not individually validated) | 330 |
| probable relocatable/library (extension-based; not decoded) | 204 |
| build script | 105 |
| assembler listing | 83 |
| text/unclassified | 70 |
| unknown/binary | 68 |
| documentation | 53 |
| archive (not expanded) | 52 |
| include/definitions | 32 |
| include/header | 28 |

## Extension distribution

Extensions are lowercased; `(none)` means no suffix. `.a` is often assembly here, `.r` often relocatable, and neither is treated as universal.

| Extension | Files |
|---|---:|
| `.a` | 491 |
| `(none)` | 480 |
| `.asm` | 414 |
| `.b09` | 212 |
| `.r` | 201 |
| `.c` | 102 |
| `.listing` | 81 |
| `.bas` | 41 |
| `.lzh` | 39 |
| `.h` | 29 |
| `.txt` | 24 |
| `.d` | 10 |
| `.mpc` | 10 |
| `.zip` | 9 |
| `.org` | 8 |
| `.doc` | 8 |
| `.source` | 8 |
| `.hlp` | 8 |
| `.bin` | 7 |
| `.mge` | 6 |
| `.new` | 5 |
| `.src` | 5 |
| `.ar` | 4 |
| `.vef` | 4 |
| `.defs` | 4 |
| `.l` | 4 |
| `.dat` | 4 |
| `.til` | 3 |
| `.bat` | 3 |
| `.me` | 2 |
| `.fnt` | 2 |
| `.d1` | 2 |
| `.h0` | 2 |
| `.o` | 2 |
| `.fix` | 2 |
| `.dr` | 2 |
| `.dd` | 2 |
| `.old` | 2 |
| `.map` | 2 |
| `.docs` | 1 |
| `.intro` | 1 |
| `.new2` | 1 |
| `.runbnew` | 1 |
| `.test` | 1 |
| `.bak` | 1 |
| `.guillaume` | 1 |
| `.name` | 1 |
| `.links` | 1 |
| `.tech` | 1 |
| `.1` | 1 |
| `.2` | 1 |
| `.3` | 1 |
| `.4` | 1 |
| `.pwd` | 1 |
| `.dsk` | 1 |
| `.file` | 1 |
| `.file_alt` | 1 |
| `.file_orig` | 1 |
| `.list` | 1 |
| `.lcb` | 1 |
| `.l3` | 1 |
| `.122` | 1 |
| `.orig` | 1 |
| `.help` | 1 |
| `.oldlisting` | 1 |
| `.gp1` | 1 |
| `.gp121` | 1 |
| `.dun` | 1 |
| `.pal` | 1 |
| `.scores` | 1 |
| `.set` | 1 |
| `.dungeon` | 1 |
| `.guib` | 1 |
| `.ext` | 1 |
| `.env` | 1 |
| `.lnk` | 1 |
| `.d0` | 1 |
| `.direct` | 1 |
| `.stdio` | 1 |

## Verified representative distinctions

- **6809-family assembly:** `ASM/NITROS9/KERNEL/fid.asm` contains `ldx`, `lda`, `sta`, `rts` and documents F$ID. CPU labels are conservative; absence of a 6309 keyword does not prove 6809-only compatibility.
- **6309-oriented assembly:** `ASM/BASIC09/runbcd.asm` identifies a 6309 RunB; `basic09.real.mul.63.asm`, `basic09.real.div.63.asm`, and `basic09.real.add.63.asm` are optimization leads. Kernel files use `IFNE H6309`; mixed conditional sources must not be labeled exclusively 6309.
- **C / DCC / CMOC:** `C/SDCCMDR/Makefile:1` sets `CC = dcc`, then `CFLAGS =-dOS9=1`. `sdccmdr.c:1` conditionally includes `<cmoc.h>` under `DECB`, and contains OS9/FLEX branches. This is evidence of CMOC-oriented code, not proof the CMOC branch builds for OS-9. No compilation was attempted.
- **Assembly inside C category:** `C/LIB/signal.a`, `intercept.a` and process/runtime wrappers are assembly, not C. `C/MAMOU/h6309.c` is C implementing an assembler, not guest assembly.
- **BASIC09:** `BASIC09/GFX5/GFX5Demo.b09` starts with `PROCEDURE`, typed variables and `RUN gfx2`; `GUIB30/Guib.b09` is a procedure library. `ASM/BASIC09` largely implements BASIC09 in assembly; directory name is not language.
- **Build scripts:** `ASM/BASIC09/makegfx2` invokes `asm` and redirects a listing. `ASM/NITROS9/DW/makedwio` contains an absolute `/h1/asm/...` output path. These scripts were inspected, never run.
- **Definitions:** files named `defsfile` may contain include directives referencing `/dd/defs`, rather than the full definitions. `ASM/MINTED/DEFS/DEFS` is an additional nested definitions collection. Dependencies outside SOURCECODE are not inventoried here.
- **Binary modules:** ToolShed `ident` verifies `ASM/2_01_boot/IOMan` as an OS-9 system module, size 2,485, edition 12, good CRC 97530B. Other JSON module classifications use header signature only.
- **Archives/listings/data:** LZH/ZIP/AR files are retained as opaque entries. `.listing` is generated evidence, not an additional independent source revision. Images, font/resource files and object code contribute to total size.

## Classification method / limitations

Text candidates have >95% printable ASCII or TAB/LF/FF/CR/SUB bytes. Classification checks assembler listings, BASIC09 PROCEDURE, C syntax in C/header files, build commands/names, documentation names, then assembly opcode/directive patterns. Files not identified remain text/unclassified or binary/unknown. This threshold can miss high-bit text and classify mixed files imperfectly. Topic regex hits are search leads, including comments, not verified API implementations. Raw hashes are computed before CR normalization. Evidence line numbers use CR-to-LF normalization; historical control codes or CRLF files may need line-number reconciliation in a future index.
