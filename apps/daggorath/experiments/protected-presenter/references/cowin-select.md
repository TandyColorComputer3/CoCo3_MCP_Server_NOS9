# Installed EOU CoWin Select: verification boundary

The local EOU boot image `OS9Boot` identifies `CoWin`: edition 2, 7,537 bytes,
CRC `3C70C2` Good, type/attributes `$C1/$80`. Its module bytes (boot image
offset 18,700) match the stored `/dd/SOURCECODE/ASM/NITROS9/SCF/cowin_beta6`
binary exactly; that binary's SHA-256 is
`e2f2f9f0aad6d9898d2602715e2ad27891c3f77526a9b3dfa37a89536166c698`.
The binary is not included here under the generated-module exclusion.

The `cowin_beta6.asm` and `cowin_beta6.listing` files here are byte-preserved
copies from the EOU VHD. The Select listing at module offsets `$0559–$05A1`
matches the corresponding installed binary bytes (including `$0559: AE26A601`,
`$0573: BC1020`, `$0578: 2626`, `$058E: FE1020FF1022BF1020`, and `$05A0:
5F39`). This is **binary-verified** for the Select instruction range. The
source is a correlated preserved source, not a claim that the full source was
rebuilt byte-identically.

At `$0559`, Select reads the path argument from the caller's register stack,
then uses `D.Proc` and `P$SelP` to identify the caller's previous selected
path. At `$0573`, it compares the old path's device static pointer to global
`G.CurDev`. The `$0578` branch can bypass the screen handoff if these differ.
The `$058E–$0597` path updates `G.PrWMPt`, `G.CurDev`, and the screen-change
flag. The bypass reaches `$05A0` (`clrb; rts`), so Select may return without
an error while the visible display remains unchanged. This does not establish
a universal prohibition on child processes selecting their own windows.

**Live-unresolved:** No verified runtime module base, caller PID, `P$SelP`,
`G.CurDev`, actual branch, or raw return was captured for the protected
presenter. The prior display ledger reported 1,499 completed frames with
Shell+ text values `$FF98=$04`, `$FF99=$75`; it did not prove the Select branch.
Historical production game graphics used registers 8/9 `80/34`, but that is
not evidence of this presenter's display activation.
