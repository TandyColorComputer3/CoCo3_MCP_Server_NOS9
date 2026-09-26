# coco-dev: optional reproducible build environment

[Navigation](README.md). Snapshot: `ccdf721ba452d282364f5b37b6225e22011aaf16`; package/wrapper version **0.88**. Evidence: [Dockerfile](/Volumes/SEDONA/Projects/coco-dev-reference/Dockerfile), [README.md](/Volumes/SEDONA/Projects/coco-dev-reference/README.md), [Makefile](/Volumes/SEDONA/Projects/coco-dev-reference/Makefile), [coco-dev](/Volumes/SEDONA/Projects/coco-dev-reference/coco-dev).

## Toolchain declared by this recipe

| Component | Version / selection / integration |
|---|---|
| Linux base | `mcr.microsoft.com/vscode/devcontainers/base:ubuntu-24.04`; apt update/upgrade/install, not an immutable digest/package lock |
| LWTOOLS | 4.25 release tarball; built in foundation, installed before CMOC. Provides the assembler/linker/archive toolchain; MVKit explicitly uses `lwar`. Dockerfile notes CMOC configure requires lwasm >=4.11. |
| CMOC | 0.1.100 tarball; configured/built/installed into staging; smoke compiles `tests/cmoc-os9/hello.c` using `cmoc --os9` |
| ToolShed | Git tag v2.6.1; `make -C build/unix`, DESTDIR installation. Supplies `os9`/`decb`; separate parallel stage because it is a runtime build-workflow dependency, not required to compile CMOC. |
| NitrOS-9 definitions | Commit `24f3d77f7e7ff05131929efbf098aabc9b8dec08`; only `defs/*` installed to `/usr/local/share/lwasm/`, source checkout removed from image |
| MAME | Default build arg `MAME_VERSION=0287`; CoCo-only subtarget from `src/mame/trs/coco3.cpp`, executable installed as `mame`; plugins/language under `/usr/local/share/mame` |
| Python converters/tools | coco-tools 0.28, milliluk-tools 0.1, mc10-tools 0.11; numpy 2.5.3, pillow 12.3.0, pypng 0.20220715.0, ruff 0.16.8, ty 0.0.82, uv 0.12.17, wand 0.7.2 |
| Other compilers | BASIC-To-6809 5.50 (architecture-selected binary); Java Grinder+naken_asm; mcbasic; tasm6801 |
| Compression/preprocessing | ZX0, salvador, decbpp pinned to explicit commits in Dockerfile |
| Host support | GCC/build-essential, ccache, Java, Python venv, make, Doxygen/Graphviz, SDL and other apt packages; apt versions are not pinned |

Non-release source pins in Dockerfile:

```text
preprocessor 97b7988aaa37dd0baeec3f77a154d5d531711d9f
ZX0          ecde3a2ae05061fe06469ed46df81a33b7de7d86
salvador     1662b625a8dcd6f3f7e3491c88840611776533f5
naken_asm    247c23706909f09bac77c587780b8a826bbda27c
java_grinder 63e20803059e8444ce3ae75da4726b16d23add88
tasm6801     0820625bf8e78053ced348a3d747191d54e5e24f
mcbasic      1030ec4413df400e07709a9aabffcaaf4772eb82
BASIC-To-6809 3747a693b70deeac03acd89cf02c5e6b38b63ef3
```

These are **recipe declarations**, not measured versions in the user's existing NAS container. Definition commit differs from our official reference HEAD and from unproven historical EOU build definitions; record all three identities rather than mixing headers by filename. MVKit apps additionally fetch **cmoc_os9** at `14b8f6bc983a1c694d36e3890f34b16c06a2af20` for libc/libcgfx: it is not the same thing as the CMOC compiler or the copied NitrOS-9 assembler defs. That dependency was not fetched here.

## Docker and devcontainer architecture

Foundation supplies Ubuntu, Python environment and LWTOOLS. Independent tool stages build against it, install into `/staging`, and final copies those trees. Ccache BuildKit mounts are build caches, not shipped artifacts. MAME defaults to two jobs because its large compiler units use substantial RAM, per Dockerfile comments. CI builds Ubuntu x86/ARM variants and pushes a combined image manifest; see [.github/workflows/build-docker.yml](/Volumes/SEDONA/Projects/coco-dev-reference/.github/workflows/build-docker.yml).

[.devcontainer/devcontainer.json](/Volumes/SEDONA/Projects/coco-dev-reference/.devcontainer/devcontainer.json) builds `../Dockerfile`, installs editor extensions and forwards port 3000. Its post-create script is currently just a shell shebang; no persistent build-worker service/protocol is defined. The `coco-dev` wrapper uses **new disposable `docker run -it --rm` containers**, maps home directories and selects Darwin/Linux behavior; Windows is explicitly unsupported. Its macOS `/Users`→`/home` rewrite does not automatically solve our `/Volumes/SEDONA/...` path mapping. **Do not use this wrapper to access the already installed NAS environment.**

## Tests and MAME

`make test` itself creates a disposable container, so it was **not run**. Its ten checks compile sample programs, exercise compression round trips, test preprocessing/defs presence and invoke `mame -listfull coco3` plus `mame -validate coco3`. The MAME checks do not boot EOU, execute the compiled OS-9 hello, or validate our `coco3h`/2M/RGB/RTC/state configuration.

The README supplies headless MAME flags (`-video none -sound none`) with SDL dummy drivers and an autoboot-script example; ROMs are not bundled. The container MAME recipe defaults to **0.287**, unlike our verified native Ample **0.289**. Preserve native MAME for canonical EOU validation; do not move save states between those builds on an assumption of compatibility.

## Useful build patterns for MCP

- Isolated output staging and explicit tool/defs pins; retain compile/link listings and maps alongside modules.
- A small target-specific compile smoke test before image packaging. Treat artifact existence as weaker than an ident/checksum plus successful guest run.
- ToolShed `os9` packaging into a **copied** base image, as historical MVKit `app.mk` demonstrates; never point those write operations at reference/frozen media.
- CMOC `--os9`, explicit libc/libcgfx include/library paths and documented stack/data budgets. Don't mix historical DCC wrappers from `/dd/SOURCECODE/C/LIB` with CMOC ABI without checking.
- Build-worker abstraction independent of emulator control. Native macOS remains a supported backend.

## Proposed use of the existing Synology worker — no deployment

User reports coco-dev is already installed there; current container name, image digest, mounts, architecture and running/stopped state are **unknown**. No NAS inspection was needed or performed for this pass.

A future authorized worker integration should first inspect that existing instance and report its identity/state; if stopped, require an explicit decision about using it, not create a replacement. Use noninteractive execution in the **existing** container (conceptually `docker exec <verified-existing-id> ...` if running), with a dedicated writable job directory and explicit working directory/environment. Transfer only selected project inputs; keep reference checkouts read-only and retrieve outputs/logs by job identity and hash. Record actual compiler versions, defs/library commits and container image digest. Report missing/mismatched dependencies rather than silently installing/upgrading them. Add bounded execution, cancellation, resource limits and separate build exit status from OS-9 program status.

This is a proposed contract, not a discovered NAS service. Do not run the repository wrapper, `make test`, devcontainer create, Docker Compose build or deployment to fulfill it. Pinning some inputs improves repeatability, but mutable apt/base tags and runtime dependencies mean this recipe alone does not guarantee bit-identical rebuilds.

Knowledge links: [C development](../source-index/C_DEVELOPMENT.md), [development tools](../source-index/DEVELOPMENT_TOOLS.md), [modern upstream recipes](../source-index/UPSTREAM_NITROS9.md), [runtime evidence](../source-index/RUNTIME_MATCHES.md).
