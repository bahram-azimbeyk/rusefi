from pathlib import Path
import sys, re, gzip
assert len(sys.argv)==3 and sys.argv[1]=="-i"
p=Path(sys.argv[2]); data=p.read_bytes()
raw=gzip.decompress(data) if p.suffix==".gz" else data
assert raw[510:512]==b"\x55\xaa"
name=re.sub(r"[^a-zA-Z0-9_]", "_", str(p))
print("unsigned char "+name+"[] = {")
for start in range(0,len(data),12):
    print("  "+", ".join("0x%02x" % v for v in data[start:start+12])+",")
print("};\nunsigned int "+name+"_len = "+str(len(data))+";")
