# Converts an input .spv file into a c++ header file containing the SPIR-V bytecode

import sys
import os
import argparse
from pathlib import Path

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input", help="Input file path", nargs='+', type=Path)
    parser.add_argument("output", help="Output file path", type=Path)

    args = parser.parse_args()

    print("SPIR-V to header")
    print("  Output: %s" % args.output)
    print("  Input: %s" % args.input)

    # Check if input files exit
    for input_file in args.input:
        if not input_file.exists():
            print("File %s does not exist" % input_file)
            exit(1)



    input_data = []
    for input_file in args.input:
        with open(input_file, "rb") as f:
            name_without_extensions = input_file.stem
            # Replace all special characters with underscores
            name_without_extensions = "".join([c if c.isalnum() else "_" for c in name_without_extensions])
            input_data.append((name_without_extensions, f.read()))

        #input_data.append(f.read())

    with open(args.output, "w") as f:
        f.write("// This file is automatically generated, do not edit!\n\n")
        f.write("#include <cstdint>\n")
        f.write("#include <vector>\n\n")
        f.write("namespace\n")
        f.write("{\n")
        for name_data_pair in input_data:
            #f.write(f"  static const uint8_t {name_data_pair[0]}[] = {{")
            f.write(f"  static const std::vector<uint8_t> {name_data_pair[0]} = {{")
            data = name_data_pair[1]
            for i in range(0, len(data), 1):
                if i % 12 == 0:
                    f.write("\n    ")

                # Don't use comma on last entry
                if i == len(data) - 1:
                    f.write("0x%02x" % (data[i]))
                else:
                    f.write("0x%02x, " % (data[i]))

            f.write("  };\n")
        f.write("}\n")

        # String containing all program names separated by commas
        valid_names = ", ".join([f"{name_data_pair[0]}" for name_data_pair in input_data])
        p = []
        p.append("#include <string>")
        p.append("#include <stdexcept>")
        p.append("")
        p.append("namespace clw_generated")
        p.append("{")
        #p.append("  const uint8_t[] get_program_bytecode(std::string name)")
        p.append("  const std::vector<uint8_t> get_program_bytecode(std::string name)")
        p.append("  {")
        p.append(f"    if (name == \"{input_data[0][0]}\")")
        p.append("    {")
        p.append(f"      return {input_data[0][0]};")
        p.append("    }")
        p.append("")
        for name_data_pair in input_data[1:]:
            p.append(f"    else if (name == \"{name_data_pair[0]}\")")
            p.append("    {")
            p.append(f"      return {name_data_pair[0]};")
            p.append("    }")
            p.append("")
        p.append("    else")
        p.append("    {")
        p.append(f"      throw std::invalid_argument(\"Unknown program name, available: {valid_names}\");")
        p.append("    }")
        p.append("  }")
        p.append("}")

        f.write("\n".join(p))

    

if __name__ == "__main__":
    main()
