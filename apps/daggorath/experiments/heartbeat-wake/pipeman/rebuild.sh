#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname "$0")" && pwd)
work=${TMPDIR:-/tmp}/daggorath-heartbeat-pipeman-rebuild
rm -rf "$work"
mkdir -p "$work"
cp "$root"/*.[ach] "$root"/defsfile "$root"/*.asm "$work"/

cd "$work"
lwasm -f os9 --pragma=forwardrefmax \
    -DOS9.D=0 -DSCF.D=0 -DCOCO.D=0 \
    --output=darmbeat-pipe driver.asm
lwasm -f os9 --pragma=forwardrefmax \
    -DOS9.D=0 -DSCF.D=0 -DCOCO.D=0 \
    --output=darm-pipe descriptor.asm
cat darmbeat-pipe darm-pipe > darmpack-pipe
lwasm -fobj --pragma=forwardrefmax -DOS9 --output=sleep0.o sleep0.asm
cmoc --os9 -I. -o wpipe81 wakekick.c ipc.c os9.c sleep0.o
cmoc --os9 -I. -o kickchild kickchild.c ipc.c os9.c sleep0.o

sha256sum -c "$root/results/accepted.sha256"
printf 'accepted PipeMan heartbeat artifacts reproduced in %s\n' "$work"
