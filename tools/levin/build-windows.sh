#!/usr/bin/env bash
set -eo pipefail
cd "$LEVIN_BUILD_ROOT"
export JAVA_TOOL_OPTIONS="-Dfile.encoding=UTF-8"
export PATH="$LEVIN_BUILD_TOOLS/bin:$PATH"
chmod +x "$LEVIN_BUILD_TOOLS"/bin/*
bash firmware/config/boards/levin/compile_levin.sh "$1" CHECKSUM_ADDRESS=0x800801C "H2D=$LEVIN_HEX2DFU"
"$LEVIN_BUILD_PYTHON" "$LEVIN_BUILD_TOOLS/package.py" "$1"
