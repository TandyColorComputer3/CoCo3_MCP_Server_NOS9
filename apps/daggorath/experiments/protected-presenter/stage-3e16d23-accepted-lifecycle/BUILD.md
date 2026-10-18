# Build and artifact provenance

This is a preservation record. **No build is run by this document or by the
D.24 archive task.** The accepted output binaries are stored under `artifacts/`.

## Inputs and identity

- Parent source: `source/ppown3.c`, SHA-256
  `dd17fc7b3844f60fbdf236c173790272596423214b8d66722ce30014f6589d09`.
- Parent shared inputs are the archived `ppgraphics.c/.h`, `ipc.c/.h`,
  `os9.c`, `fileio.c/.h`, `platform.h`, `ppown3.s`, and `sleep0.asm`.
- Child source: `source/ppclient3.c`, SHA-256
  `582cd685a97b5e32d8f30a79f59a4eb5cacc82e14d0c3bb70db3421a4e7264fa`.
  It is the same source as the preserved D.3/D.8 child. D.20's copied child
  source omitted `ackread.c/.h`; the exact dependency was recovered from the
  matching D.3 ACK-read source and is archived here.
- `source/ackread.c`: SHA-256
  `da24dbc0ab56945505e0f0a9ec2e6bd182c3a2b3e07a585ac548a71cd4d9ab90`.
- `source/ackread.h`: SHA-256
  `fb4ad452fada887d00534d27580aad9d5a12a79db29fb421d813572e621d9f84`.
- Full input-file hashes and historical source paths are listed in
  `provenance.json`.

## Historical toolchain

- CMOC 0.1.90 (`/usr/local/bin/cmoc`), binary SHA-256
  `76cf6c59de150c3ced93fe39a349223ef600fc3e3d74d0e7fa76aa9d637c35d4`.
- lwasm/lwlink 4.22, SHA-256 respectively
  `174d0ef03732647cdb7f5c2b3d75762ed1190902601879961ab3be14635c865c` and
  `0f834f21dc237b3fd8b9426b0aa23f5a6cfb4d090fb00b45396d2945835c1253`.
- ToolShed 2.2 `os9`, SHA-256
  `ee9c3e3585640dfd28f4e3def75a09d14eddeaff464241f7ad94d5b6d51bb569`.
- The preprocessing/runtime inputs are identified by hash in
  `provenance.json`.

## Recorded build method and reproducibility

The archived child `build/child/original-build-once.sh` and logs record the
build convention. It hard-codes a historical `/private/tmp` source path and is
preserved as evidence, not a portable script. A clean reproduction should
substitute this directory's `source/` path while keeping the flags and input
set shown below:

```sh
lwasm -fobj --pragma=forwardrefmax -DOS9 --output=sleep0.o source/sleep0.asm
cmoc --os9 -i -o ppown20 source/ppown3.c source/ppgraphics.c source/ipc.c \
  source/os9.c source/fileio.c sleep0.o
cmoc --os9 -i -o ppclient3 source/ppclient3.c source/ackread.c source/ipc.c \
  source/os9.c source/fileio.c sleep0.o
```

The parent historical link used `lwlink --format=os9` with the recorded
CMOC OS-9 runtime libraries. Its exact link input is preserved in
`build/parent/ppown20.link`; generated map/listing and full command output are
in the same directory. The child's two build logs and generated link/map files
are preserved in `build/child/`.

Parent outputs A/B/C were byte-identical with SHA-256
`896a47b794a8b5c4e50dc9b21af50ac1b57924f8cd2870094ddd2ab8caee97de`.
Child outputs 1/2 and the D.23 staged/extracted module were byte-identical with
SHA-256
`eba7a09379b15ef4a3c09bd07e48107d90720ddd3e97b736acb57363685701e7`.
The complete host/guest identities are also checked against `SHA256SUMS`.

The historical results establish reproducibility for the recorded inputs and
toolchain. Rebuilding now is not necessary to use these accepted binaries and
was expressly outside D.24.
