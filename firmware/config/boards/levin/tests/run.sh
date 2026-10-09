#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/../../../../.."
mkdir -p .build
${CXX:-g++} -std=c++17 -Wall -Wextra -Werror \
 -Ifirmware/config/boards/levin/tests/stubs -Ifirmware/config/boards/levin \
 -Ifirmware/init/sensor -Ifirmware/controllers/sensors/converters \
 firmware/config/boards/levin/tests/test_board.cpp -o .build/levin-host-tests
.build/levin-host-tests
