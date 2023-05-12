# Converts an input .spv file into a c++ header file containing the SPIR-V bytecode

import sys
import os
import argparse

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input", help="Input file path")
    parser.add_argument("output", help="Output file path")
    args = parser.parse_args()

    print("SPIR-V to header")
    print("  Input: %s" % args.input)
    print("  Output: %s" % args.output)

    with open(args.input, "rb") as f:
        data = f.read()

    with open(args.output, "w") as f:
        f.write("#pragma once\n\n")
        f.write("static const uint32_t bytecode[] = {")
        for i in range(0, len(data), 4):
            if i % 16 == 0:
                f.write("\n  ")

            # Don't use comma on last entry
            if i == len(data) - 4:
                f.write("0x%08x" % (data[i] | (data[i+1] << 8) | (data[i+2] << 16) | (data[i+3] << 24)))
            else:
                f.write("0x%08x, " % (data[i] | (data[i+1] << 8) | (data[i+2] << 16) | (data[i+3] << 24)))

        f.write("};\n")

if __name__ == "__main__":
    main()
