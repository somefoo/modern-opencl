# Given a path glob, return a list of all files matching glob pattern
# The script takes the glob pattern as an argument and prints the list of files

import os
from glob import glob

def path_glob(glob_pattern):
    return glob(glob_pattern)

if __name__ == '__main__':
    import sys
    if len(sys.argv) != 2:
        print("Usage: python glob.py <glob_pattern>")
        sys.exit(1)
    glob_pattern = sys.argv[1]
    print(','.join(path_glob(glob_pattern)))
