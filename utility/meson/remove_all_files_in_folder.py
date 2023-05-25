# Remove all files in directory
# Usage: python remove_all_files_in_folder.py <folder_path>
# Check if folder exists and is a folder
# The deletion is recursive

import sys
import os

if len(sys.argv) < 2:
    print("Usage: python remove_all_files_in_folder.py <folder_path>")
    sys.exit(1)

folder_path = sys.argv[1]
if not os.path.exists(folder_path):
    print("Folder does not exist")
    sys.exit(0)

if not os.path.isdir(folder_path):
    print("Path is not a folder")
    sys.exit(1)

# Check if all files in all folders are either .clcpp, .cl, .clhpp, .clh.
# If not, exit
for root, dirs, files in os.walk(folder_path):
    for f in files:
        if not f.endswith(".clcpp") and not f.endswith(".cl") and not f.endswith(".clhpp") and not f.endswith(".clh"):
            print("File does not have a valid extension (.clcpp, .cl, .clhpp, .clh): " + f)
            print("The risk is too high that this folder is wrong, exiting")
            sys.exit(1)



for root, dirs, files in os.walk(folder_path):
    for f in files:
        os.unlink(os.path.join(root, f))
    for d in dirs:
        os.rmdir(os.path.join(root, d))
