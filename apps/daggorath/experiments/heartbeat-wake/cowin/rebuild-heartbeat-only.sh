#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname "$0")" && pwd)
source_dir="$root/source"
work=${TMPDIR:-/tmp}/daggorath-heartbeat-cowin-heartbeat-only
rm -rf "$work"
mkdir -p "$work"
cp "$source_dir"/* "$work"/

cd "$work"
lwasm -f os9 --pragma=forwardrefmax -DOS9.D=0 -DSCF.D=0 -DCOCO.D=0 \
    --output=darmbeat-cowin driver.asm
lwasm -f os9 --pragma=forwardrefmax -DOS9.D=0 -DSCF.D=0 -DCOCO.D=0 \
    --output=darm-cowin descriptor.asm
cat darmbeat-cowin darm-cowin > darmpack-cowin
lwasm -fobj --pragma=forwardrefmax -DOS9 --output=sleep0.o sleep0.asm
cmoc --os9 -i -DHEARTBEAT_ONLY_GATE -o wcowin-map3 \
    wakekick.c graphics.c ipc.c os9.c sleep0.o
cmoc --os9 -o kickchild kickchild.c ipc.c os9.c sleep0.o

shasum -a 256 darmbeat-cowin darm-cowin darmpack-cowin \
    wcowin-map3 kickchild > artifacts-heartbeat-only.sha256
printf 'CoWin heartbeat-only gate built in %s\n' "$work"
