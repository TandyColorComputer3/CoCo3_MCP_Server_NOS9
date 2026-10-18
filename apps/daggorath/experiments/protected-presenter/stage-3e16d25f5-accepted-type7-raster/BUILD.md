# Exact module and build provenance

The archived executables are the accepted artifacts. This preservation step did not rebuild them.

| Module | Size | Data request | Entry | ToolShed CRC | SHA-256 |
| --- | ---: | ---: | --- | --- | --- |
| `ppown25c` | 7,557 | 53 | `$000D` | `007195` Good | `5e8f9bfb2f84002490ce0c1edc5aa08e653814390f174f278b5ddf26e95a682e` |
| `ppclient25c` | 7,672 | 217 | `$000D` | `6DF5D0` Good | `b3e59eeaa0af91eee5d592693c79b91e39e49b01ac5ec5cee9438746ea0c249d` |

Both are reentrant OS-9 program modules (`Ty/La=$11`, `At/Rv=$80`). Identical accepted copies were found in original `build-c` and `build-d`. The D.25F.5 guest extracts were byte-identical. `PROVENANCE.json` records every preserved source and build-file hash.

## Build inputs

`source/` contains the observed C, headers, and `sleep0.asm`. `build/accepted-c/` contains the accepted output's generated assembly, object files, listings, maps, and `.link` section descriptions. The original `source/ppown3.s` was *not* selected as accepted generated assembly: it differs from `build-c/ppown3.s`. That exclusion is explicit in `PROVENANCE.json`. The historical D.25C compiler command line was not preserved; do not claim a fresh byte-exact rebuild from this archive alone.

The installed historical toolchain identities are CMOC 0.1.90 (SHA-256 `76cf6c59de150c3ced93fe39a349223ef600fc3e3d74d0e7fa76aa9d637c35d4`), lwasm/lwlink 4.22 (`174d0ef03732647cdb7f5c2b3d75762ed1190902601879961ab3be14635c865c`, `0f834f21dc237b3fd8b9426b0aa23f5a6cfb4d090fb00b45396d2945835c1253`), and ToolShed 2.2 `os9` (`ee9c3e3585640dfd28f4e3def75a09d14eddeaff464241f7ad94d5b6d51bb569`). The `.link` files retain the section order and OS-9 program entry; maps and listings identify the generated code.

The following is a **candidate reconstruction recipe** based on the D.23 archived build convention and D.25C input inventory. Its exact flags were not independently recovered or run here. Use only in a disposable directory and reject any output that differs from the table above:

```sh
lwasm -fobj --pragma=forwardrefmax -DOS9 --output=sleep0.o source/sleep0.asm
cmoc --os9 -i -o ppown25c source/ppown3.c source/ppgraphics.c source/ipc.c source/os9.c source/fileio.c sleep0.o
cmoc --os9 -i -o ppclient25c source/ppclient3.c source/ackread.c source/ipc.c source/os9.c source/fileio.c sleep0.o
```

Verify `os9 ident` CRC, module name/type/entry/data request, size, and full SHA-256 against the accepted binaries before any guest use. The accepted binaries, not a speculative rebuild, remain authoritative.
