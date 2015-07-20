import struct
import pathlib
import os
import imghdr

#Code stolen from
#http://stackoverflow.com/a/20380514/1739913

def get_image_size(fname):
    '''Determine the image type of fhandle and return its size.
    from draco'''
    with open(fname, 'rb') as fhandle:
        head = fhandle.read(24)
        if len(head) != 24:
            return
        if imghdr.what(fname) == 'png':
            check = struct.unpack('>i', head[4:8])[0]
            if check != 0x0d0a1a0a:
                return
            width, height = struct.unpack('>ii', head[16:24])
        elif imghdr.what(fname) == 'gif':
            width, height = struct.unpack('<HH', head[6:10])
        elif imghdr.what(fname) == 'jpeg':
            try:
                fhandle.seek(0) # Read 0xff next
                size = 2
                ftype = 0
                while not 0xc0 <= ftype <= 0xcf:
                    fhandle.seek(size, 1)
                    byte = fhandle.read(1)
                    while ord(byte) == 0xff:
                        byte = fhandle.read(1)
                    ftype = ord(byte)
                    size = struct.unpack('>H', fhandle.read(2))[0] - 2
                # We are at a SOFn block
                fhandle.seek(1, 1)  # Skip `precision' byte.
                height, width = struct.unpack('>HH', fhandle.read(4))
            except Exception: #IGNORE:W0703
                return
        else:
            return
        return width, height

#My code
abspath = os.path.abspath(__file__)
script_loc = os.path.dirname(abspath)

wallpaperpath = open('wallpaperlocation.txt').readline().strip()
with open('screenres.txt') as f:
    screendim = (int(f.readline()), int(f.readline()))

toresize = []
failed = []
os.chdir(wallpaperpath)

p = pathlib.Path()
wallpaperlist = list(p.rglob('*.jpg')) + list(p.rglob('*.jpeg')) + \
                list(p.rglob('*.png')) + list(p.rglob('*.gif'))
wallpaperlist = [str(f) for f in wallpaperlist]
for i, f in enumerate(wallpaperlist):
    img_loc = os.path.abspath(f)
    with open(f, 'rb') as fbyte:
        size = get_image_size(f)
        if size == None:
            failed.append(img_loc)
        elif screendim != size:
            toresize.append(img_loc)

os.chdir(script_loc)

fout = open('toresize.txt', 'w')
for f in toresize:
    fout.write(str(f) + '\n')
fout.close()

if len(failed) != 0:
    ffailed = open('failed.txt', 'w')
    for f in failed:
        ffailed.write(str(f) + '\n')
    ffailed.close()