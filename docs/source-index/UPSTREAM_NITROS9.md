# Current upstream NitrOS-9 reference

[Navigation](README.md) · [EOU crosswalk](EOU_UPSTREAM_CROSSWALK.md) · [Modernization and ABI notes](MODERNIZATION_NOTES.md)

## Snapshot and authority

Inspected 2026-09-26. External, read-only checkout: `/Volumes/SEDONA/Projects/nitros9-reference`. Origin is the official [nitros9project/nitros9](https://github.com/nitros9project/nitros9) repository. **Every upstream comparison in these three documents is pinned to `f470fa52eb172b59b22c1b722074998cb42de9b1`**, not to a moving branch name. The checkout was clean, non-shallow, and was not fetched, built, checked out, or modified. “Current” means this supplied snapshot; it does not assert that the remote branch cannot have advanced.

| Item | Observed value |
|---|---|
| HEAD | [`f470fa52eb172b59b22c1b722074998cb42de9b1`](https://github.com/nitros9project/nitros9/commit/f470fa52eb172b59b22c1b722074998cb42de9b1) |
| Subject | RP2040 core management for RC18 |
| Author timestamp | 2026-09-25 11:20:10 -0500 |
| Commit timestamp | 2026-09-25 14:53:28 -0500 |
| `git describe --tags --always` | `v3.3.0-1129-gf470fa52` |
| v3.3.0 tagged commit | `b83f83abbd11da1d7d902916be9630add83c823c` |
| Tagged commit timestamp | 2014-04-13 21:19:27 +02:00 |

The [GitHub releases page](https://github.com/nitros9project/nitros9/releases) labels V3.3.0 as Latest. A published release, a local tag, and current development HEAD are different identities. HEAD's subject concerns Wildbits/RP2040 work; it is not itself evidence of a CoCo 3 improvement.

Local tags observed:

```text
Revision_C Revision_F Revision_F_1 basic09v010100 nitrosl2beta1
os9l1phoenix phoenixbeta1 pre-lwtools-dummymerge
v2.1.0beta2 v3.0.2beta2
v3.1.0 v3.1.1 v3.1.2 v3.1.3 v3.1.4
v3.2.0 v3.2.1 v3.2.2 v3.2.3 v3.2.4 v3.2.5 v3.2.6
v3.2.7 v3.2.8 v3.2.9 v3.3.0
```

These are Git refs, not a claim that every tag has a published release or supports our frozen EOU machine.

## Where CoCo 3 Level II comes from

Read [docs/repository-layout.md](/Volumes/SEDONA/Projects/nitros9-reference/docs/repository-layout.md), [docs/supported-ports.md](/Volumes/SEDONA/Projects/nitros9-reference/docs/supported-ports.md), and [recipes/README.md](/Volumes/SEDONA/Projects/nitros9-reference/recipes/README.md) first.

| Layer | Actual structure and use |
|---|---|
| Port identity | [level2/coco3/port.mak](/Volumes/SEDONA/Projects/nitros9-reference/level2/coco3/port.mak) selects CPU 6809; [level2/coco3_6309/port.mak](/Volumes/SEDONA/Projects/nitros9-reference/level2/coco3_6309/port.mak) selects CPU 6309. Both use PORT=coco3, LEVEL=2. Port files are inputs, not the primary build entry points. |
| Shared definitions | [defs/os9.d](/Volumes/SEDONA/Projects/nitros9-reference/defs/os9.d), [defs/scf.d](/Volumes/SEDONA/Projects/nitros9-reference/defs/scf.d), [defs/rbf.d](/Volumes/SEDONA/Projects/nitros9-reference/defs/rbf.d), [defs/coco.d](/Volumes/SEDONA/Projects/nitros9-reference/defs/coco.d), [defs/cocovtio.d](/Volumes/SEDONA/Projects/nitros9-reference/defs/cocovtio.d) |
| Level II kernel | [level2/modules/kernel/krn.asm](/Volumes/SEDONA/Projects/nitros9-reference/level2/modules/kernel/krn.asm), [level2/modules/kernel/krnp2.asm](/Volumes/SEDONA/Projects/nitros9-reference/level2/modules/kernel/krnp2.asm), with shared kernel routines also in `level1/modules/kernel/` |
| Shared services | IOMan, SCF and PipeMan resolve to `level1/modules/`; RBF to `level2/modules/`. A `level1` path does not mean the file is irrelevant to Level II. |
| CoCo 3 video | `level2/coco3/modules/`: VTIO, CoWin, CoVDG, descriptors, sound and new Co3HiRes. GrfDrv resolves to [level2/cmds/grfdrv.asm](/Volumes/SEDONA/Projects/nitros9-reference/level2/cmds/grfdrv.asm). |
| CPU selection | Shared source uses `H6309` and level/port conditionals. There is no independent complete 6309 source tree. |
| Historical material | `archive/` is explicitly unsupported by the maintained recipes; `level3/` is experimental. Do not silently substitute either for the Level II implementation. |

### Recipes and search order

6809 CoCo 3 entry points include [recipes/coco3/floppy/makefile](/Volumes/SEDONA/Projects/nitros9-reference/recipes/coco3/floppy/makefile), [recipes/coco3/dw/makefile](/Volumes/SEDONA/Projects/nitros9-reference/recipes/coco3/dw/makefile), [recipes/coco3/dw_mega/makefile](/Volumes/SEDONA/Projects/nitros9-reference/recipes/coco3/dw_mega/makefile), and [recipes/coco3/basic09/makefile](/Volumes/SEDONA/Projects/nitros9-reference/recipes/coco3/basic09/makefile). The 6309 floppy entry point is **[recipes/coco3_6309/40d/makefile](/Volumes/SEDONA/Projects/nitros9-reference/recipes/coco3_6309/40d/makefile)**, which sets LEVEL=2 and includes [recipes/coco3_6309/coco3_6309.mak](/Volumes/SEDONA/Projects/nitros9-reference/recipes/coco3_6309/coco3_6309.mak).

[recipes/rules.mak](/Volumes/SEDONA/Projects/nitros9-reference/recipes/rules.mak) searches Level II platform modules/commands, Level I platform modules/commands, shared Level II and Level I modules/commands, then kernel and font paths. The 6309 recipe additionally supplies `level1/coco1/modules`, resolving such components as Clock2 and relocation. Inspect the actual `vpath` and include order when multiple filenames exist.

The 6309 recipe adds `-DH6309=1`; common assembler flags use `lwasm --6309 --format=os9`. The assembler's accepted instruction set alone does **not** establish that every resulting module requires a 6309. Conditional source, frame layout, and emitted instructions determine that.

Selected defaults observed in `coco3_6309.mak`:

| Group | Recipe selection |
|---|---|
| Kernel track | relocation, `boot_1773_6ms`, Krn |
| OS9Boot | KrnP2, IOMan, Init, configured service groups, SysGo and shell_21 |
| RBF | RBF, rb1773, 40-track disk descriptors |
| SCF | SCF, VTIO, **Co3HiRes**, snddrv_cc3, joydrv_joy, selected terminal I/O and window descriptors |
| Terminal | TERM_COLS defaults to 80; 32 selects CoVDG, 40/80 selects CoWin |
| Pipes | PipeMan, Piper, Pipe descriptor |
| Clock | clock_60hz plus **clock2_soft** |
| Shell command pack | ShellPlus plus Date, DeIniz, Echo, Iniz, Link, Load, Save, Unlink |

**This recipe is not the frozen EOU boot recipe.** Our observed Clock2 matches `clock2_messemu`, and the VHD uses EmuDsk; those facts must not be replaced by assumptions based on the upstream floppy defaults. This task changes no emulator configuration or boot media.

[recipes/coco3/README.md](/Volumes/SEDONA/Projects/nitros9-reference/recipes/coco3/README.md) documents KEYRPT and MAME=1 for the 6809 recipe's generated startup, and [recipes/coco3/floppy/recipe.mak](/Volumes/SEDONA/Projects/nitros9-reference/recipes/coco3/floppy/recipe.mak) implements the choice. Do not assume every recipe exposes identical options.

[README.md](/Volumes/SEDONA/Projects/nitros9-reference/README.md) identifies LWTOOLS, ToolShed and make, plus sibling `nitros9-languages`/`nitros9-apps` repositories for relevant packaged language/application content. Those sibling repositories were not audited. Missing application source here is not proof that it is absent from the upstream ecosystem.

## Methods and limits

Reused the Pass 1 byte-preserving EOU extraction and Pass 2 DEFS/module observations tied to the development VHD hash. No guest command, new live module survey, source build, archive-wide extraction or disk write was performed. [Runtime evidence](RUNTIME_MATCHES.md) remains the identified 2026-09-26 restored-machine survey, not a fresh observation of new upstream binaries.

Read-only command families used (UP denotes the external checkout):

```sh
git -C "$UP" status --short
git -C "$UP" remote -v
git -C "$UP" rev-parse HEAD
git -C "$UP" rev-parse --is-shallow-repository
git -C "$UP" show -s --format=fuller HEAD
git -C "$UP" describe --tags --always
git -C "$UP" tag --list
git -C "$UP" log --follow --format='%h %ad %s' --date=short -- <selected-path>
git -C "$UP" show --stat 732cb596
git -C "$UP" show 14e9394a -- level2/modules/rbf.asm
git -C "$UP" show b6f7cc95 -- level1/modules/scdwv.asm
rg --files "$UP"
rg -n '<targeted-symbol-or-instruction>' <selected-files>
shasum -a 256 media/63SDC-MCP-DEV.VHD
```

Selected paired files were read with Python `Path.read_bytes/read_text`; CR/LF, whitespace and case normalization helped locate changes with `difflib`. Comments were retained. Similarity scores were **not** used as lineage or binary-equivalence proof: mechanical label renaming can dominate a diff. Changes were checked against surrounding routines, definitions, recipe selection and `git log/show`. Comparison scripts and intermediate diffs stayed under `/private/tmp/upstream-crosswalk/`, not in Git; the crosswalk records the source pairs needed to repeat the comparisons.

Local manuals referenced by project instructions (`MCP/Documents`, `DOCS_INDEX.md`) were unavailable in this checkout. These documents cite implementation and observed-module evidence; they do not claim independent manual verification of hardware or ABI contracts.

### Integrity

Development VHD SHA-256 before and after this pass:

```text
4c6bdc0cd2f68aa57a9c75f75f15fe429f30926aa522917dd8d14aa1b9ae622e
```

No upstream source was copied into this repository. Local upstream links refer to the external checkout; use the pinned commit above to recover the same files if that checkout later moves. No MCP source, EOU media, or reference-checkout changes were made by this task; no commit was made. Existing unrelated working-tree changes were preserved.
