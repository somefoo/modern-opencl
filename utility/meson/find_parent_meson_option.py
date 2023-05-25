import os

# Get the current working directory
path = os.getcwd()
# Go up one
path = os.path.dirname(path)

# Flag to indicate if we found the file
found = False

# Check at most five directories
for _ in range(5):
    # Check if meson.options exists in this directory
    if os.path.isfile(os.path.join(path, "meson.options")):
        # Open the file and check if it contains the string
        with open(os.path.join(path, "meson.options")) as file:
            if "opencl_kernel_folder_path" in file.read():
                # Print the directory and set the flag to True
                print(path)
                found = True
                break

    # Go up one directory level
    path = os.path.dirname(path)

# If we didn't find the file, print a message
if not found:
    print("Not found")
