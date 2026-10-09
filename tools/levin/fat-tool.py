from pathlib import Path
import sys
from pyfatfs.PyFat import PyFat
from pyfatfs.PyFatFS import PyFatFS
mode, *args = sys.argv[1:]
if mode == "mkfs.fat":
    assert args[:-1] == ["-v", "-r", "64"], args
    image = Path(args[-1])
    fat = PyFat()
    fat.mkfs(str(image), fat_type=PyFat.FAT_TYPE_FAT12, size=image.stat().st_size, label="RUSEFI")
    fat.close()
elif mode == "fatlabel":
    assert len(args) == 2 and args[1] == "RUSEFI", args
    data = Path(args[0]).read_bytes()
    assert data[43:54].rstrip() == b"RUSEFI" and data[510:512] == b"\x55\xaa"
elif mode == "mcopy":
    assert len(args) == 4 and args[0] == "-i" and args[3] == "::", args
    source = Path(args[2])
    with PyFatFS(args[1], preserve_case=True) as fs:
        fs.writebytes("/" + source.name, source.read_bytes())
    with PyFatFS(args[1], read_only=True) as fs:
        assert fs.readbytes("/" + source.name) == source.read_bytes()
else:
    raise ValueError(mode)
