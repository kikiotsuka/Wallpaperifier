import sys

if sys.version_info[0] < 3: #py2
    from Tkinter import *
    from tkFileDialog import *
else: #py3
    from tkinter import *
    from tkinter.filedialog import *

import os

root = Tk()
root.withdraw()
home = os.path.expanduser('~')
dirname = askdirectory(parent=root, initialdir=home, title="Select wallpaper directory", mustexist=True)

if os.name == 'nt': #windows
    dirname = dirname.replace('/', '\\')

if len(dirname) > 0:
    fout = open('wallpaperlocation.txt', 'w')
    fout.write(str(dirname) + '\n')
    fout.close()