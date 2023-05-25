# Copies all files (recursively) from source to destination
# Usage: python3 copy_all_files_from_source_to_destination.py <source> <destination>
# Ensures the source exists, if the destination doesn't exist, create it

import os
import sys
import shutil

def copy_all_files_from_source_to_destination(source, destination):
    if not os.path.exists(source):
        print("Source does not exist")
        sys.exit(1)
    if not os.path.exists(destination):
        os.makedirs(destination)
    for root, dirs, files in os.walk(source):
        for file in files:
            source_file = os.path.join(root, file)
            destination_file = os.path.join(destination, file)
            shutil.copyfile(source_file, destination_file)

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python3 copy_all_files_from_source_to_destination.py <source> <destination>")
        sys.exit(1)
    source = sys.argv[1]
    destination = sys.argv[2]
    copy_all_files_from_source_to_destination(source, destination)
