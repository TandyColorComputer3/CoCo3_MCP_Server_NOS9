#!/bin/sh
set -eu

root=$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)
work=${TMPDIR:-/tmp}/daggorath-heartbeat-wake-rebuild
rm -rf "$work"
mkdir -p "$work/arm-wake" "$work/arm-kick"

build_driver() {
    dir=$1
    stem=$2
    cd "$dir"
    lwasm -f os9 --pragma=forwardrefmax \
        -DOS9.D=0 -DSCF.D=0 -DCOCO.D=0 \
        --output="darmbeat-$stem" driver.asm
    lwasm -f os9 --pragma=forwardrefmax \
        -DOS9.D=0 -DSCF.D=0 -DCOCO.D=0 \
        --output="darm-$stem" descriptor.asm
    cat "darmbeat-$stem" "darm-$stem" > "darmpack-$stem"
    lwasm -fobj --pragma=forwardrefmax -DOS9 \
        --output=sleep0.o sleep0.asm
}

cp "$root"/arm-wake/*.[ach] "$root"/arm-wake/defsfile \
    "$work/arm-wake/"
cp "$root"/arm-wake/driver.asm "$root"/arm-wake/descriptor.asm \
    "$root"/arm-wake/sleep0.asm "$work/arm-wake/"
build_driver "$work/arm-wake" race
cd "$work/arm-wake"
cmoc --os9 --add-os9-stack-space=512 -I. -o wakerace \
    wakeone.c ipc.c os9.c sleep0.o

cp "$root"/arm-kick/*.[ach] "$root"/arm-kick/defsfile \
    "$work/arm-kick/"
cp "$root"/arm-kick/driver.asm "$root"/arm-kick/descriptor.asm \
    "$root"/arm-kick/sleep0.asm "$work/arm-kick/"
build_driver "$work/arm-kick" kick
cd "$work/arm-kick"
cmoc --os9 -I. -o wakekick wakekick.c ipc.c os9.c sleep0.o
cmoc --os9 -I. -o kickchild kickchild.c ipc.c os9.c sleep0.o

cd "$work"
printf '%s  %s\n' \
  7bdd98930980211aeb0c49695029f2fa755d3063771b7a868028d16ab3213ebc arm-wake/darmbeat-race \
  301b32f54d7f206ac0cb3cc0131afa62abb5e31426ebb0324e90c1ed1deb97bc arm-wake/wakerace \
  fa9278ca1c6237727b726c0ca417fb7509593d6544f6742e045a53e17cfbb09b arm-kick/darmbeat-kick \
  3c1f05777e3c7d2d1648c8475cacd6b7e9697f3506806d390d2c440d9dc25036 arm-kick/wakekick \
  b730263be76dfc1ca78e7e6ee963973d07ee92fb29914a7282713f9095042a73 arm-kick/kickchild \
  | sha256sum -c -

printf 'accepted heartbeat wake artifacts reproduced in %s\n' "$work"
