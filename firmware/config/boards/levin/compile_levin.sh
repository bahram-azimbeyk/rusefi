#!/usr/bin/env bash
set -eo pipefail
mode=${1:-uart}
if [ "$#" -gt 0 ]; then shift; fi
case "$mode" in
 native) meta=config/boards/levin/meta-info-native.env ;;
 uart) meta=config/boards/levin/meta-info.env ;;
 *) echo "Usage: $0 [native|uart] [make arguments]" >&2; exit 2 ;;
esac
cd "$(dirname "$0")/../../.."
source config/boards/common_script_read_meta_env.inc "$meta"
# Compiler flags are not dependencies in this release. Clean both images when switching.
if [ "$(cat .levin-transport 2>/dev/null || true)" != "$mode" ]; then
 make -j12 clean
fi
make -j12 config
make -j12 "$@" all deliver/rusefi.bin
printf '%s\n' "$mode" > .levin-transport
