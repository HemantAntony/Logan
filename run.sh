#!/usr/bin/env bash
set -e

if [ ! -d build ]; then
  cmake -S . -B build -G Ninja
fi

cmake --build build
./build/logan

