# Levin

Custom rusEFI target for Levin Pro 2.3 circuitry with Pro 2.4 connector names.
STM32F407VGT6, 8 MHz HSE. Based on official `release_20260812`, commit
`a6d6d77b8876b5941c2ec12da8b28a00b9d7b538`.

## Connections

Two transport builds are available. `native` uses native USB only for tuning;
`uart` additionally enables CH340 or Bluetooth on USART1 (PA9 TX / PA10 RX),
115200 baud. The two bridges share the same UART: use one host connection at a time.
Native USB remains available. The bundled OpenBLT updater uses native USB, not CH340;
CH340 support here is for runtime tuning, not a new UART bootloader.

Internal wiring is restored after loading a tune and is hidden from pin selectors:

| Function | MCU connection |
| --- | --- |
| Stepper DIR / STEP / ENABLE | PB10 / PB11 / PA15 |
| CAN RX / TX | PD0 / PD1 |
| SD SPI3 SCK / MISO / MOSI / CS | PC10 / PC11 / PC12 / PD2 |
| Battery ADC | PA4 |
| SMD284 barometer ADC | PA5 |

Stepper direction defaults to inverted, following the working hardware-reference tune.
Stepper enable is normal polarity. Mode, travel, direction and operating settings remain
user adjustable. SD is present; external flash is not fitted. CAN protocol remains selectable.

All exposed outputs and inputs remain assignable to other compatible functions. Names
such as `PWM OUT 1`, `INJ 8`, and `AUX PULSE 1` refer to physical connector channels.
Only exposed digital pins appear in generic digital selectors. Normal rusEFI pin ownership
checks remain in force; an already claimed pin is not made shareable by this board definition.
To reuse an injector or coil output, remove its old assignment if that channel is claimed.
Electrical capabilities still follow the board circuit, regardless of the selected function.

The current Lite revision has different SD wiring and is not covered by this target.
The target name is intentionally `levin` for a future Lite revision with matching wiring.

## Sensor defaults

- CLT and IAT pull-ups: 2490 ohm. These are defaults, not forced values: recalibrate if a
  sensor is moved to an input with a different pull-up.
- ADC reference: 3.3 V; external analog divider: 1.5151515; battery divider: 7.0.
  The divider values follow the working Golf hardware-reference tune and need voltage
  verification on the physical board. Battery scaling remains adjustable.
- SMD284: `Vout / Vdd = 0.007895 * pressure_kPa`, using the SMD084/SMD284 column
  of the supplied Bosch datasheet. Custom calibration endpoints: 0 kPa at 0 V and
  approximately 126.662 kPa at 5 V. Check against local absolute ambient pressure.
- This release's custom-barometer converter incorrectly used manifold MAP calibration.
  The included small sensor fix separates barometer endpoints from MAP endpoints.

No Golf fuel, ignition, injector, trigger or engine calibration is included. The LT1 8-slot trigger backport is included in both builds; selecting it and calibrating
the trigger offset is still required (see docs/triggers/gm-lt1-optispark-8.md). The default
engine type is rusEFI's generic MINIMAL_PINS, with suggested board assignments. Flashing
firmware does not automatically replace compatible settings already stored in the ECU:
review existing settings or start with clean configuration before importing an engine tune.
Fixed internal pins and serial baud are enforced; external assignments survive tune import.

## Build

On this Windows workstation, run from repository root:

```powershell
.\build-levin.ps1 -Mode native
.\build-levin.ps1 -Mode uart
```

The Windows helper uses Pixi, Python and native hex2dfu. Tool paths can be passed as
parameters. It reuses a sibling `rusefi` toolchain when present, without depending on
that repository's source files or Git objects. Local Python FAT dependencies are
installed under `.build/pydeps`. Packages go to `artifacts/levin-<mode>-lt1-20260812.zip`.

On Linux with standard rusEFI dependencies and initialized submodules:

```sh
bash firmware/config/boards/levin/compile_levin.sh native
bash firmware/config/boards/levin/compile_levin.sh uart
```

The entry point cleans application and bootloader objects when switching transport.
Do not run two builds in this checkout concurrently. Native and UART have distinct INI
signatures; use `rusefi_levin_native.ini` and `rusefi_levin.ini`, respectively.

Equivalent low-level commands for the UART build:

```sh
cd firmware
source config/boards/common_script_read_meta_env.inc config/boards/levin/meta-info.env
make -j12 config
make -j12 all deliver/rusefi.bin
```

The separate `config` step is necessary on a clean checkout of this release when adding
new connector YAMLs: its Makefile otherwise attempts to resolve the generated pin-name
source before that source exists. Inputs must have LF line endings.

For this Windows build, local FAT12/xxd wrappers and `CHECKSUM_ADDRESS=0x800801C` were used
with ARM GCC 14.2.1, Java UTF-8 output and native hex2dfu. They change packaging tooling, not ECU behavior.
The full image includes OpenBLT at 0x08000000 and the CRC-patched application at 0x08008000.
Use the full `.bin` at 0x08000000 for an initial ST-Link/DFU flash, and the `.srec` with
OpenBLT for updates. Use the matching `rusefi_levin.ini` in TunerStudio.

## Validation

```sh
bash firmware/config/boards/levin/tests/run.sh
python firmware/config/boards/levin/tests/check_ini.py
```

The native test executes the actual board hooks, actual MAP/barometer initialization and
actual linear converter. Lightweight stubs replace the configuration container, hardware,
and sensor registration. It checks default wiring, restoring fixed wiring after a foreign
tune, retaining external reassignment, default pull-ups, and independent custom/preset
barometer conversion. It is not a hardware simulator or the complete upstream test suite.
The INI test checks all exposed pin names and numeric enum IDs, excluded internal pins,
and menu visibility. Binary validation separately checks CRC, vectors, full/HEX/SREC
consistency and exact firmware/INI signature matching.

Hardware checks still required: CH340/BT connection and reconnection, power-cycle persistence,
SD writes, CAN traffic, stepper direction/enable/homing, ADC scaling, output polarity, and
pin-conflict reporting. Perform initial powered checks with engine actuators disconnected;
this package has not been flashed or run on the user's ECU during this task.
