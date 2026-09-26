 section __os9
 fcc /dodaudio/
 fcb 0
edition equ 1
rev equ 1
* lwtools 4.22 link.c check_os9_aux; kernel FLinkReEntrant rejects a
* second link to this non-reentrant module while an owner holds it.
attr equ 0
 endsection
