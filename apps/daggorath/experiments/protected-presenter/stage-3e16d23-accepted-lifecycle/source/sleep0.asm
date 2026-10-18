 section code
_sleep_signal export
_sleep_signal
 pshs y,u
 ldx #2
 os9 $0a
 bcs done
 clrb
done
 puls u,y,pc
 endsection
