# Recovered build record (session transcript)

The accessible Stage 3E.16B Codex session records `build.py` invoked twice in separate output directories, followed by successful `cmp` of both `protcl` and `protpr`. The captured ToolShed output reports:

- `protcl`: 3,610 bytes, data request 1,103, CRC `2FD2F3` Good, SHA-256 `e10f6f8e465264b1223f12738557484ee8cf103343ff4f4d19aadad810b8bfeb`, executable offset 13, type/attributes `$11/$80`; map program end 3,601, program DAT 1, data DAT 1, stack reserve 1,024.
- `protpr`: 9,059 bytes, data request 3,318, CRC `973E3D` Good, SHA-256 `dbbcb242197c9be870945e1dfcb497239035dd0a3f642bb56196db242c0b67bd`, executable offset 13, type/attributes `$11/$80`; map program end 8,688, program DAT 2, data DAT 1, stack reserve 1,024.

The original build outputs, map files, log files, and dependency hash JSON were not retained. The recovered client itself calls the broker API and contains no direct graphics path/PutBlk operation; the exact historical broker header that defines the 16-byte frame size was not archived or hash-pinned, so framing length cannot be independently verified from the recovered client source alone. This record proves what the session reported, not current byte-level access to the binaries or all build inputs. The session’s source-copy/hash command recorded `client.c` as `a9be9cce523524644ac8bdc2d4465fec3de5efee7b49be64a78ab58600177b7b` and `presenter.c` as `1bde874594657782aa8b8c75b7e6bd4d0ab3f94b2e8df7628756f2aba8c3cb3e`; only the client bytes were recovered exactly.
