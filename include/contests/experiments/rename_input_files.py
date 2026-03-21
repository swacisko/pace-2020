import os
from pathlib import Path

directory = Path("input")
extension = ".txt"

files = [f for f in directory.iterdir() if f.suffix == extension]

sizes = [1<<i for i in range(10,20)]
degs = [2.5, 3, 4, 5, 10]

for f in files:
    print(f)
    # f.rename( os.path.join( directory, f"{f.stem[5:]}{f.suffix}"))
    x = int(f.stem)
    if x < 50:
        name = 'treelike'
    elif x < 100:
        name = 'gridlike'
    elif x < 150:
        name = 'random'
    elif x < 200:
        name = 'small_sep'
    else:
        name = 'complex'

    x %= 50
    n = sizes[x//5]
    deg = degs[x%5]

    # fname = '_'.join([name, str(n), str(deg)])
    fname = '_'.join([name, str(10 + x//5), str(int(deg))])
    # print(fname)
    f.rename( os.path.join( directory, fname + f.suffix))