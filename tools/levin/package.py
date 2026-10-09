from pathlib import Path
import hashlib, re, shutil, struct, zlib, zipfile, sys, subprocess, json
root=Path(__file__).resolve().parents[2]
mode=sys.argv[1]
assert mode in ("native", "uart")
name="levin_native" if mode=="native" else "levin"
fw=root/'firmware'
full=(fw/'deliver/rusefi.bin').read_bytes()
app=(fw/'build/rusefi_crc32.bin').read_bytes()
assert full[0x8000:] == app, 'full/update application mismatch'
assert full[0x24:0x28] == b'BL09', 'bootloader marker mismatch'
stored,length=struct.unpack_from('<II',app,0x1c)
assert length == len(app)
assert stored == zlib.crc32(app[0x20:], zlib.crc32(app[:0x1c])), 'application CRC mismatch'
# Independently check every byte defined by Intel HEX (except patched CRC/size).
base=0
for line in (fw/'build/rusefi.hex').read_text().splitlines():
    raw=bytes.fromhex(line[1:]); assert sum(raw)%256==0
    n,addr,kind=raw[0],int.from_bytes(raw[1:3],'big'),raw[3]
    if kind==4: base=int.from_bytes(raw[4:6],'big')<<16
    elif kind==0:
        for i,v in enumerate(raw[4:4+n]):
            offset=base+addr+i-0x08008000
            if not 0x1c<=offset<0x24: assert app[offset]==v, hex(offset)
# Verify S-record update data is the same CRC-patched application.
srec={}
for line in (fw/'build/rusefi.srec').read_text().splitlines():
    if line[:2] not in ('S1','S2','S3'): continue
    raw=bytes.fromhex(line[2:]); assert sum(raw)%256==255
    width=int(line[1])+1; addr=int.from_bytes(raw[1:1+width],'big')
    for i,v in enumerate(raw[1+width:-1]): srec[addr+i]=v
assert bytes(srec[0x08008000+i] for i in range(len(app)))==app
ini=fw/('tunerstudio/generated/rusefi_'+name+'.ini')
text=ini.read_bytes().decode("latin-1")
assert 'PWM OUT 1' in text
signature=re.search(r'^\s*signature\s*=\s*"([^"]+)"',text,re.M).group(1)
assert signature.encode() in app, 'INI firmware signature mismatch'

stack, reset = struct.unpack_from('<II',app)
assert 0x20000000 <= stack <= 0x20020000 or 0x10000000 <= stack <= 0x10010000
assert reset & 1 and 0x08008000 <= (reset & ~1) < 0x08008000+len(app)
print('Verified application CRC, full/HEX/SREC consistency, flash vectors, and INI signature.')
print('Signature:', signature)
print('Application bytes:', len(app), 'full image bytes:',len(full))

assert 'GM LT1 Optispark (8-slot only)' in text
assert b'USB Console' in app, 'Native USB tuning channel missing'
subprocess.run([sys.executable,str(fw/'config/boards/levin/tests/check_ini.py'),str(ini)],check=True,cwd=root)
base='a6d6d77b8876b5941c2ec12da8b28a00b9d7b538'
subprocess.run(['git','merge-base','--is-ancestor',base,'HEAD'],check=True,cwd=root)
# Independently verify that the requested runtime transport exists in the linked firmware.
assert b'Primary TS Channel' in app if mode=='uart' else b'Primary TS Channel' not in app
out=root/'artifacts'/('levin-'+mode+'-lt1-20260812');out.mkdir(parents=True,exist_ok=True)
for src,dst in [(fw/'deliver/rusefi.bin','levin-full.bin'),(fw/'build/rusefi.srec','levin-update.srec'),(ini,ini.name),(fw/'config/boards/levin/README.md','README.md'),(root/'docs/triggers/gm-lt1-optispark-8.md','LT1.md')]:shutil.copy2(src,out/dst)
mp=(fw/'build/rusefi.map').read_text(encoding='utf-8',errors='replace')
def sym(n):return int(re.search(r'0x([0-9a-f]+)\s+'+n+r'\s*=',mp)[1],16)
info={'base_tag':'release_20260812','base_commit':base,'source_commit':subprocess.check_output(['git','rev-parse','HEAD'],cwd=root,text=True).strip(),'source_dirty':bool(subprocess.check_output(['git','diff','HEAD','--','firmware/config/boards/levin','firmware/controllers/trigger/decoders','firmware/controllers/algo/engine_types.h','firmware/integration/rusefi_config.txt'],cwd=root)),'mode':mode,'lt1_decoder':True,'signature':signature,'application_bytes':len(app),'main_heap_before_runtime_bytes':sym('__heap_end__')-sym('__heap_base__'),'ccm_heap_before_runtime_bytes':sym('__heap_ccm_end__')-sym('__heap_ccm_base__'),'full_image_address':'0x08000000','hardware_tested':False,'validation':['Application CRC','Full image/HEX/SREC consistency','Vectors','Exact INI signature','Runtime UART presence/absence','INI pins and visibility','Release base ancestry']}
(out/'manifest.json').write_text(json.dumps(info,indent=2)+'\n',encoding='utf-8')
(out/'SHA256SUMS.txt').write_text(''.join(hashlib.sha256(p.read_bytes()).hexdigest()+'  '+p.name+'\n' for p in sorted(out.iterdir()) if p.is_file() and p.name!='SHA256SUMS.txt'),encoding='ascii')
archive=out.with_suffix('.zip')
with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED) as z:
 for p in sorted(out.iterdir()):
  if p.is_file():z.write(p,p.name)
with zipfile.ZipFile(archive) as z:assert z.testzip() is None
print('Package:',archive)
