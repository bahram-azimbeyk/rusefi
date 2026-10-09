# Levin development checkout

This repository is based on official rusEFI release_20260812, commit
`a6d6d77b8876b5941c2ec12da8b28a00b9d7b538`. Do not silently rebase onto master.
The branch is `feature/levin-release-20260812` in bahram-azimbeyk/rusefi.

The board uses Pro 2.3 circuitry with Pro 2.4 pin labels. Pro SD/CAN/stepper wiring
is fixed; external pins remain user configurable. The older Lite SD layout differs.
Golf is a hardware reference only, not a bundled engine tune.

## Build on this Windows workstation

```powershell
.\build-levin.ps1 -Mode native
.\build-levin.ps1 -Mode uart
```

`native`: native USB tuning, no primary UART tuning thread.
`uart`: CH340/Bluetooth at 115200 baud, native USB also available.
Both include GM LT1 Optispark (8-slot only), without choosing it automatically or
assuming a measured cylinder-1 TDC offset. OpenBLT updating still uses native USB.

Outputs: `artifacts/levin-native-lt1-20260812.zip` and `artifacts/levin-uart-lt1-20260812.zip`.
Use each package's matching INI. The full binary starts at 0x08000000.

- [Board, wiring and bring-up details](firmware/config/boards/levin/README.md)
- [LT1 configuration and unresolved engine timing calibration](docs/triggers/gm-lt1-optispark-8.md)
- [Session work and validation report](docs/report.md)

The checkout has its own `.git` directory; it is not a linked worktree. Build tools
may be shared with the sibling `rusefi` directory; explicit paths are accepted by
the PowerShell helper. The previous hidden worktree is retained as a backup.
Generated files, local build tools and binaries must not be committed. Keep source
changes separate from engine-specific tunes. Hardware execution is not verified
by compilation or host unit tests.
