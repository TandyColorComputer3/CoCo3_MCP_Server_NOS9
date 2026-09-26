# Modernization notes and source precedence

[Navigation](README.md) · [Pinned upstream](UPSTREAM_NITROS9.md) · [Source/runtime crosswalk](EOU_UPSTREAM_CROSSWALK.md)

This is a knowledge-layer audit, **not a proposal to upgrade the frozen EOU environment**. Upstream means `f470fa52eb172b59b22c1b722074998cb42de9b1`. EOU source paths below are guest paths. Runtime claims refer to [the recorded EOU module survey](RUNTIME_MATCHES.md), not newly built upstream modules.

## What current upstream adds to the EOU teaching material

### 1. Shared CoCo 3 HiRes services and explicit lifecycle

Study [level2/coco3/modules/co3hires.asm](/Volumes/SEDONA/Projects/nitros9-reference/level2/coco3/modules/co3hires.asm), [level2/coco3/modules/vtio.asm](/Volumes/SEDONA/Projects/nitros9-reference/level2/coco3/modules/vtio.asm), [level2/coco3/modules/cowin.asm](/Volumes/SEDONA/Projects/nitros9-reference/level2/coco3/modules/cowin.asm) and [defs/cocovtio.d](/Volumes/SEDONA/Projects/nitros9-reference/defs/cocovtio.d) together. Commit `03e6fdbd` (2026-07-15) and the new module's header identify shared handling of SS.AScrn, SS.DScrn, SS.PScrn, SS.FScrn and the CoVDG form of SS.ScInf.

Observed: VTIO links `Co3HiRes` with F$Link and retains its entry pointer; CoWin delegates through H$Init/H$Term/H$SetStt/H$Show and tracks application-screen state. The 6309 recipe includes `co3hires.sb`. These paths are absent from `/dd/SOURCECODE/ASM/NITROS9/SCF/vtio_beta6.asm` and `cowin_beta6.asm`.

Reusable interpretation: study initialization, optional-service handling, delegation and teardown as one contract. Do not transplant a caller or definition independently of the provider. VTIO edition 4 and CoWin edition 2 remain declared upstream despite the newer code, matching the edition numbers observed in EOU; this concretely demonstrates why edition equality cannot establish binary identity.

### 2. Runtime keyboard-repeat configuration

Upstream VTIO's SS.GIP path reads R$Y and updates the keyboard-repeat state (`G.KyRept`); commit `fec475b9` (2026-05-29) records runtime control. Compare the EOU VTIO candidate above. [recipes/coco3/README.md](/Volumes/SEDONA/Projects/nitros9-reference/recipes/coco3/README.md) and [recipes/coco3/floppy/recipe.mak](/Volumes/SEDONA/Projects/nitros9-reference/recipes/coco3/floppy/recipe.mak) document/implement KEYRPT=0 and MAME=1 for the 6809 recipe startup. This supplies a modern source-backed configuration example; it does not change or verify our frozen EOU repeat behavior.

### 3. Complete lock-request restoration in RBF

At `L0B1D` in [level2/modules/rbf.asm](/Volumes/SEDONA/Projects/nitros9-reference/level2/modules/rbf.asm), [commit 14e9394a](https://github.com/nitros9project/nitros9/commit/14e9394a95fdce57e1908eeded2c3c2974577e78) restores the saved D and X on retry after parking. `/dd/SOURCECODE/ASM/NITROS9/RBF/rbf_postbeta6.asm` still reloads only A at the corresponding point. This is a concrete newer correction to study when teaching retry loops and preservation across blocking calls.

Scope limit: the EOU header's separate concern about deleting files with large ToolShed-created segment lists is **not** established as fixed by this lock-retry change. Nor was the live RBF binary disassembled/rebuilt to prove which instructions it contains.

### 4. Shared DriveWire channel ownership

[level1/modules/scdwv.asm](/Volumes/SEDONA/Projects/nitros9-reference/level1/modules/scdwv.asm) and [commit b6f7cc95](https://github.com/nitros9project/nitros9/commit/b6f7cc95fb6b8c9ac88adbcc8e28f8e4974d9f20) retain a channel while another path remains open, testing `V.PDLHd` before closure. Compare `/dd/SOURCECODE/ASM/NITROS9/DW/scdwv.asm`. This is a useful lifecycle/reference-ownership example beyond the EOU source snapshot.

Later transaction/FujiNet branches (`2011bf75`) also exist, but their presence is not evidence that the frozen EOU machine exposes such a device. No DriveWire driver was identified resident in the recorded survey. Transport capability must be established from the built driver, descriptor, attachments and runtime, not a repository search alone.

### 5. Maintained build composition and more descriptive kernel labels

Use [recipes/rules.mak](/Volumes/SEDONA/Projects/nitros9-reference/recipes/rules.mak) and [recipes/coco3_6309/coco3_6309.mak](/Volumes/SEDONA/Projects/nitros9-reference/recipes/coco3_6309/coco3_6309.mak) instead of assuming loose EOU guest makefiles describe the modern build. These specify resolution order, CPU/level symbols and module dependencies. Later kernel/SCF label and comment work (`c815f26b`, `ef0e0252`, `5f26c97e`) makes implementation study easier. Large textual differences from `/dd/SOURCECODE/ASM/NITROS9/KERNEL` or `SCF` can result from this maintenance, without proving large behavioral changes.

Recent Wildbits/RP2040 commits and the Level I-specific IOMan deletion correction (`3a9cb46c`) must not be advertised as CoCo 3 Level II improvements merely because they touch shared files. Check conditional scope and recipe selection.

## Where EOU contains material not superseded by this checkout

1. **SDC variant with later fixes:** `/dd/SOURCECODE/ASM/NITROS9/GIMEX/llcocosdc_ver101.asm` adds `V.Response` storage before restoring MPI selection, then reads the buffered response in later operations. Its 2023-09-10 header attributes the correction to Darren Atkinson. Its 2022-08-26 notes and GIMEX branches implement speed handling. [level1/coco1/modules/llcocosdc.asm](/Volumes/SEDONA/Projects/nitros9-reference/level1/coco1/modules/llcocosdc.asm) lacks those particular changes. Upstream is not automatically the newer implementation along every axis. Upstream relocation **does** contain GIMEX branches: [level1/coco1/modules/rel.asm](/Volumes/SEDONA/Projects/nitros9-reference/level1/coco1/modules/rel.asm); absence must be scoped to the SDC driver.
2. **Verified Shell+ behavior:** `/dd/SOURCECODE/ASM/SHELL/shellplus2.2a.asm` and the live Shell fingerprint remain relevant to our command-completion protocol. [level1/cmds/shellplus.asm](/Volumes/SEDONA/Projects/nitros9-reference/level1/cmds/shellplus.asm) is related but differs in notes and instruction/addressing forms. Do not replace live sequencing/error-status evidence with assumptions from a current upstream file. See [Shell index](SHELL.md) and the linked live completion investigation.
3. **EOU distribution/application evidence:** `/dd/SOURCECODE/C/CONTROL` has explicit EOU attribution; installed startup/`montype r` and selected Clock2 are deployment facts. GUIB/GFX5/GSHELL and other collections retain their individual historical or third-party provenance, rather than becoming “EOU-authored” by inclusion. No maintained corresponding applications were established in this checkout; sibling application/language repositories remain unexamined.
4. **Clock provider identity:** the live Clock2 fingerprint matches bundled `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_messemu_6809` and `_6309` bytes. Upstream's `clock2_soft` recipe default is a different selection. The compared `clock2_messemu.asm` routine body is unchanged after normalization, but include/build context differs. A generic module name `Clock2` is not enough to identify its provider.

## ABI-sensitive review checklist grounded in the compared definitions

These are source observations and reuse limits, **not a declaration of complete binary compatibility**. EOU definitions came from `/dd/DEFS`; source callers and runtime matches are mapped in [the crosswalk](EOU_UPSTREAM_CROSSWALK.md).

| Area | What was observed | Implication for agents |
|---|---|---|
| Process descriptor | The inspected Level II declarations in EOU `/dd/DEFS/os9.d` and upstream [defs/os9.d](/Volumes/SEDONA/Projects/nitros9-reference/defs/os9.d) correspond: P$ID through process/queue/signal fields, DIO/Path arrays, P$DATImg (64-byte reservation), links and remaining fields through P$Size. Upstream also has other level/target definitions. | Select Level II before deriving offsets. Matching a declaration sequence is not proof that any arbitrary live process pointer or allocation is valid. Use the installed build's definitions and runtime evidence. |
| System-call register frame | Both inspected defs choose R$Size=14 for H6309 and 12 otherwise. The 6309 branch adds R$E/R$F and shifts DP/X/Y/U/PC; W/Q are aliases in that package. | Never use a 6809 frame layout for a 6309-native kernel merely because a module header labels code as 6809 object. Inspect the selected build symbols and wrapper code. `/dd/SOURCECODE/ASM/BASIC09/syscall_6309.asm` is an existing filename-versus-frame warning. |
| DAT/MMU | Compare EOU `KERNEL/fmapblk.asm`, `fdatlog.asm`, `fcpymem.asm` with upstream `level2/modules/kernel/` and selected defs. Shared routines include port/CPU conditions; much newer text is label/comment maintenance. | Logical addresses, a process DAT image and physical RAM remain separate concepts. No new hardware register map is inferred here. Do not import addresses or another port's mapping branch into an MCP memory reader. |
| Device table | `defs/os9.d` defines the device-table contract, including V$USRS; IOMan's shared Attach/Detach code is newer. A full instantiated live-table comparison was not performed. | Use symbols from one consistent revision; do not infer table ownership or entry lifetimes from a historical caller alone. |
| Path descriptors | Compared base PD.RGS/PD.BUF/PD.FST/PD.OPT declarations correspond; RBF definition differences inspected are principally guards/comments, while SCF adds level guards around window-related material. | Match the file manager and level before interpreting extension bytes. Similar base fields do not establish all driver/private extensions are compatible. |
| VTIO/CoWin private state | Upstream `defs/cocovtio.d` repurposes the earlier three-byte G001D reservation as two-byte G.HrsEnt plus one reserved byte, preserving the following G.CurDev position. It also introduces per-device V.HrBuf/V.HiRes and H$ entry definitions. | This is a concrete private contract change even where a following global offset stays unchanged. Keep VTIO, CoWin, Co3HiRes and private definitions coherent; do not infer all offsets shifted or all stayed compatible. |
| Driver/file-manager interface | Descriptors select SCF/VTIO or RBF/EmuDsk in the observed EOU boot. Upstream recipe selection and drivers are separate evidence. | Check entry tables, private storage, status functions and descriptor options together. Never infer an active driver from available source alone. |
| F$/I$ interfaces | I$ModDsc, SS.GIP2 and SS.Fill appear in both EOU and upstream os9.d. The EOU integration commit explicitly discusses added interfaces and conditional fixes. | Consult the installed implementation for support, upstream for current implementation, and manuals for documented contracts. A symbol's presence in defs does not prove every driver implements it. |
| 6309 instructions and assumptions | H6309 branches, 14-byte frame declarations and recipe flags supply actual selection evidence. Upstream SDC and EOU GIMEX variants differ in raw-byte versus mnemonic expression of transfers. | Study TFM/LDW and other 6309 uses in their guarded context. `--6309` assembler parser mode and a `_6309` filename alone do not certify CPU requirements or correctness. |

## Clearly older patterns and what to consult instead

- EOU RBF's partial retry restoration has a specific newer correction upstream; teach the complete saved-request restoration pattern, with the scope limit above.
- The older `/dd/SOURCECODE/ASM/NITROS9/MODS/llcocosdc.asm` is not the only EOU SDC reference. Compare both the later EOU GIMEX variant and upstream driver before discussing modern MPI transfers.
- Guest absolute `/dd/defs/deffile` and `pipedefs` includes in `/dd/SOURCECODE/ASM/NITROS9/PIPE/pipeman_beta6.asm` are not portable modern build recipes. Current PipeMan uses recipe `defsfile`, `pipe.d`, and `scf.d`; this is a build-context improvement, not proof the EOU pipe algorithm is obsolete.
- Current readable [level2/cmds/grfdrv.asm](/Volumes/SEDONA/Projects/nitros9-reference/level2/cmds/grfdrv.asm) fills an implementation-study gap left by the undecoded `/dd/SOURCECODE/ASM/grfdrv0724_1998.lzh`. It does not establish lineage to that archive or an exact match to resident GrfDrv.
- Historical MAMOU/RMA/RLINK examples remain useful for object formats. Current recipes use LWTOOLS; replacing a build invocation is different from rejecting the historical algorithms.

## Recommended source precedence

Precedence depends on the question, not a universal ranking of repositories:

1. **What does our frozen guest actually do?** Start with verified EOU runtime fingerprints, command experiments and boot configuration. Identify what was observed, when, and under which state/media. Preserve contrary observations even if upstream differs.
2. **How was that behavior likely implemented?** Use `/dd/SOURCECODE` candidates, provenance and dependency indexes. Exact source/runtime claims require a reproducible build or equivalent evidence; bundled binary equality is not editable-source equality.
3. **How is current NitrOS-9 implemented or built?** Use the pinned official upstream source and recipe for the correct port, level and CPU. Compare history and explicit patches before recommending a modern pattern. Check for EOU fixes missing upstream.
4. **What is the documented ABI/hardware contract?** Consult the applicable manual and revision. Implementation observations do not silently replace a published contract. If manuals are unavailable, state the gap and constrain the claim to the inspected implementation.

When these disagree, record both versions and the exact discrepancy. Resolve behavior with targeted read-only experiments or a future controlled build; do not silently “correct” the knowledge layer to the newest filename or timestamp.

### Proposed AGENTS.md rule (not applied)

> Before implementing NitrOS-9 behavior, consult `docs/source-index/README.md` and its topic and upstream crosswalk. Use verified EOU runtime evidence for the frozen guest, provenance-qualified `/dd/SOURCECODE` for candidate implementations, and a pinned official upstream commit plus the correct CoCo 3 Level II/CPU recipe for modern implementations. Verify ABI and hardware contracts against applicable manuals. Record conflicts explicitly; never equate filenames, editions or source similarity with an exact runtime match. Do not update guest media, the reference checkout, or the frozen baseline merely to align them with upstream.

## Follow-up gaps

- Applicable manuals are still needed for normative syscall/frame, SCF/RBF and hardware claims; source citations here are implementation evidence.
- Rebuilding selected EOU modules would establish or reject exact source/runtime matches. This requires separately authorized writable build outputs, historical definitions and tool versions, not writes to reference media.
- Audit `nitros9-languages` and `nitros9-apps` separately for interpreter, C/library and application lineage.
- Resolve GrfDrv LH1 archives with a suitable read-only extractor before claiming historical lineage.
- Inspect EOU Shell+ addressing differences and the two SDC branches in more depth before proposing replacements. No upgrade recommendation is made in this pass.
