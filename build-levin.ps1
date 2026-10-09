param(
    [ValidateSet('native','uart')][string]$Mode = 'native',
    [string]$PixiManifest,
    [string]$PixiExecutable,
    [string]$PythonExecutable,
    [string]$Hex2DfuExecutable
)
$ErrorActionPreference = 'Stop'
$repoRoot = $PSScriptRoot
$sharedTools = Join-Path (Split-Path $repoRoot -Parent) 'rusefi'
if (!$PixiManifest) {
    $PixiManifest = Join-Path $repoRoot 'pixi.toml'
    if (!(Test-Path (Join-Path $repoRoot '.pixi/envs/default')) -and (Test-Path (Join-Path $sharedTools '.pixi/envs/default'))) {
        $PixiManifest = Join-Path $sharedTools 'pixi.toml'
    }
}
if (!$PixiExecutable) {
    $installed = Get-Command pixi -ErrorAction SilentlyContinue
    if ($installed) { $PixiExecutable = $installed.Source }
    else { $PixiExecutable = Join-Path $sharedTools '.tools/pixi/pixi.exe' }
}
if (!$PythonExecutable) {
    $pioPython = Join-Path $env:USERPROFILE '.platformio/penv/Scripts/python.exe'
    if (Test-Path $pioPython) { $PythonExecutable = $pioPython }
    else { $PythonExecutable = (Get-Command python -ErrorAction Stop).Source }
}
if (!$Hex2DfuExecutable) { $Hex2DfuExecutable = Join-Path $sharedTools '.tools/hex2dfu.exe' }
foreach ($tool in @($PixiExecutable,$PixiManifest,$PythonExecutable,$Hex2DfuExecutable)) {
    if (!(Test-Path -LiteralPath $tool)) { throw "Missing tool: $tool. Supply the corresponding script parameter." }
}
$deps = Join-Path $repoRoot '.build/pydeps'
$env:PYTHONPATH = $deps
& $PythonExecutable -c 'import importlib.util,sys; sys.exit(0 if importlib.util.find_spec("pyfatfs") else 1)'
if ($LASTEXITCODE -ne 0) {
    & $PythonExecutable -m pip install --target $deps -r (Join-Path $repoRoot 'tools/levin/requirements.txt')
    if ($LASTEXITCODE -ne 0) { throw 'Could not install local FAT image dependencies.' }
}
$env:LEVIN_BUILD_ROOT = $repoRoot.Replace('\','/')
$env:LEVIN_BUILD_TOOLS = (Join-Path $repoRoot 'tools/levin').Replace('\','/')
$env:LEVIN_BUILD_PYTHON = $PythonExecutable.Replace('\','/')
$env:LEVIN_HEX2DFU = $Hex2DfuExecutable.Replace('\','/')
Push-Location $repoRoot
try {
    & $PixiExecutable run --manifest-path $PixiManifest bash (Join-Path $repoRoot 'tools/levin/build-windows.sh') $Mode
    if ($LASTEXITCODE -ne 0) { throw "Levin $Mode build failed ($LASTEXITCODE)." }
} finally { Pop-Location }
