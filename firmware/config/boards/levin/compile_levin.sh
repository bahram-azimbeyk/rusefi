#!/usr/bin/env bash
set -e
cd "$(dirname "$0")/../../.."
source config/boards/common_script_read_meta_env.inc config/boards/levin/meta-info.env
make -j12 config
bash bin/compile.sh config/boards/levin/meta-info.env -j12 "$@"
