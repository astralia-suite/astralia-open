#!/bin/sh
set -e
cd "$(dirname "$0")"

[ -d build ] || meson setup build --prefix=/usr
meson compile -C build

if [ "$1" = install ]; then
    sudo meson install -C build --no-rebuild
fi
