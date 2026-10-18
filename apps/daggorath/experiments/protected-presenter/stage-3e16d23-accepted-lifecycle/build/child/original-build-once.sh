#!/bin/sh
set -eu
src=/private/tmp/dag-3e16d3-ack-read/src
out=$1
cd "$out"
lwasm -fobj --pragma=forwardrefmax -DOS9 --output=sleep0.o "$src/sleep0.asm"
cmoc --os9 -i -o ppown3 "$src/ppown3.c" "$src/ppgraphics.c" "$src/ipc.c" "$src/os9.c" "$src/fileio.c" sleep0.o
cmoc --os9 -i -o ppclient3 "$src/ppclient3.c" "$src/ackread.c" "$src/ipc.c" "$src/os9.c" "$src/fileio.c" sleep0.o
