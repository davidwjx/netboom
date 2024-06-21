import argparse
import platform
import binascii
import array
import os

#LRAM Slice sizes definition
num_slises_in_row=4
slice_lines=(16*1024)          # 16k lines
slice_width=4                  # 4 bytes per line
#LRAM mapping
lram_start_address = 0x2000000

def line_2_lram_slices(ln):
    int_line = int(ln,16)
    # print("0x{:x}".format(int_line))
    slice_0 = (int_line & 0x00000000ffffffff) >> 0
    slice_1 = (int_line & 0xffffffff00000000) >> 32
    return slice_0, slice_1

def line_2_ddr_slices(ln):
    int_line = int(ln,16)
    # print("0x{:x}".format(int_line))
    slice_0 = (int_line & 0x000000000000ffff) >> 0
    slice_1 = (int_line & 0x00000000ffff0000) >> 16
    slice_2 = (int_line & 0x0000ffff00000000) >> 32
    slice_3 = (int_line & 0xffff000000000000) >> 48
    return slice_0, slice_1, slice_2, slice_3

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="bin2hex in ROM format\n Split HEX file for LRAM slices\n")
    parser.add_argument('-hex', type = str, dest='input_file', help='Path to input HEX file')
    parser.add_argument('-bin', type=str, dest='bin_file', help='Path to input BIN file')
    parser.add_argument('-lram', type=bool, dest='lram_slice', help='Flag to generate LRAM slices')
    parser.add_argument('-ddr', type=bool, dest='ddr_slice', help='Flag to generate DDR slices')
    parser.add_argument('-sz', type=bool, dest='add_size', help='Add file size in index 0x00 (availabe for DDR only)')
    parser.add_argument('-boot', type = lambda x: int(x,16), dest='boot_point', help='Boot address of LRAM app. Default 0x2000000')
    parser.add_argument('-out', type=str, dest='out_dir', help='Path to output directory. Default is current dir')

    results = parser.parse_args()

    input_file = results.input_file
    bin_file = results.bin_file
    boot_point = results.boot_point
    if boot_point is None:
        boot_point = lram_start_address

    if platform.system() == 'Linux':
        print("Running on Linux")
        uint32_format = 'I'
    elif platform.system() == 'Windows':
        print("Running on Windows")
        uint32_format = 'L'
    else:
        uint32_format = 'L'
        print("OS is not supported")
        exit(0)

    ddr_file_size  = None

    if input_file is not None:
        input_hex = open(input_file, "r")
        print("Opened HEX file {0}".format(os.path.splitext(os.path.basename(bin_file))[0]))
    elif bin_file is not None:
        hex_data = array.array(uint32_format)
        input_bin = open(bin_file, "rb")
        input_bin_sz = os.path.getsize(bin_file)
        if results.add_size == 1:
            ddr_file_size = input_bin_sz
        print("Opened BIN file {0} , size {1}".format(os.path.splitext(os.path.basename(bin_file))[0], input_bin_sz))
        hex_file_name = os.path.splitext(os.path.basename(bin_file))[0] + ".hex"
        if results.out_dir is not None:
            hex_file_name = results.out_dir + hex_file_name
        input_hex = open(hex_file_name, "w+")

        for i in range(input_bin_sz>>2):
            # Read unit32
            read_hex_data = binascii.hexlify(input_bin.read(4))
            hex_data.append(int(str(read_hex_data)[2:-1],16))
        hex_data.byteswap()
        for i in range(len(hex_data)>>1):
            input_hex.write("{0:08x}{1:08x}\t\n".format(hex_data[i*2+1], hex_data[i*2]))
        input_hex.seek(0, os.SEEK_SET)
    else:
        input_hex = 0
        print("No input")
        exit(0)

    if results.lram_slice == 1:
        name_0_A = "lram_slice_0_A.hex"
        name_1_A = "lram_slice_1_A.hex"
        name_2_A = "lram_slice_2_A.hex"
        name_3_A = "lram_slice_3_A.hex"
        name_0_B = "lram_slice_0_B.hex"
        name_1_B = "lram_slice_1_B.hex"
        name_2_B = "lram_slice_2_B.hex"
        name_3_B = "lram_slice_3_B.hex"

        if results.out_dir is not None:
            name_0_A = results.out_dir + name_0_A
            name_1_A = results.out_dir + name_1_A
            name_2_A = results.out_dir + name_2_A
            name_3_A = results.out_dir + name_3_A
            name_0_B = results.out_dir + name_0_B
            name_1_B = results.out_dir + name_1_B
            name_2_B = results.out_dir + name_2_B
            name_3_B = results.out_dir + name_3_B

        slice_0_A_file = open(name_0_A, "w")
        slice_1_A_file = open(name_1_A, "w")
        slice_2_A_file = open(name_2_A, "w")
        slice_3_A_file = open(name_3_A, "w")
        slice_0_B_file = open(name_0_B, "w")
        slice_1_B_file = open(name_1_B, "w")
        slice_2_B_file = open(name_2_B, "w")
        slice_3_B_file = open(name_3_B, "w")

        in_file_lines = input_hex.readlines()

        start_offset = boot_point - lram_start_address
        start_line = int(start_offset / (slice_width * num_slises_in_row))
        print("offset 0x{0:x} line 0x{1:x}".format(start_offset, start_line))

        for i in range(start_line):
            slice_0_A_file.write("{0:08x}\n".format(0))
            slice_1_A_file.write("{0:08x}\n".format(0))
            slice_2_A_file.write("{0:08x}\n".format(0))
            slice_3_A_file.write("{0:08x}\n".format(0))

        for index, line in enumerate(in_file_lines):
            s_0, s_1 = line_2_lram_slices(line)
            # print("{0:x}\t{1:x}".format(s_0, s_1))

            if start_line + (index / 2) < slice_lines:
                if (index % 2) == 0:
                    #Even lines
                    slice_0_A_file.write("{0:08x}\n".format(s_0))
                    slice_1_A_file.write("{0:08x}\n".format(s_1))
                else:
                    #Odd lines
                    slice_2_A_file.write("{0:08x}\n".format(s_0))
                    slice_3_A_file.write("{0:08x}\n".format(s_1))
            else:
                if (index % 2) == 0:
                    #Even lines
                    slice_0_B_file.write("{0:08x}\n".format(s_0))
                    slice_1_B_file.write("{0:08x}\n".format(s_1))
                else:
                    #Odd lines
                    slice_2_B_file.write("{0:08x}\n".format(s_0))
                    slice_3_B_file.write("{0:08x}\n".format(s_1))

        input_hex.close()
        slice_0_A_file.close()
        slice_1_A_file.close()
        slice_2_A_file.close()
        slice_3_A_file.close()

        slice_0_B_file.close()
        slice_1_B_file.close()
        slice_2_B_file.close()
        slice_3_B_file.close()

    if results.ddr_slice == 1:
        # print("Opened BIN file {0}".format(os.path.splitext(os.path.basename(bin_file))[0]))
        name_slice_0 = os.path.splitext(os.path.basename(hex_file_name))[0] + "_0.hex"
        name_slice_1 = os.path.splitext(os.path.basename(hex_file_name))[0] + "_1.hex"

        if results.out_dir is not None:
            name_slice_0 = results.out_dir + name_slice_0
            name_slice_1 = results.out_dir + name_slice_1

        slice_0_file = open(name_slice_0, "w")
        slice_1_file = open(name_slice_1, "w")

        in_file_lines = input_hex.readlines()

        if ddr_file_size is not None:
            slice_0_file.write("{0:04x}\n".format(((ddr_file_size >>  0) & 0xFFFF)))
            slice_1_file.write("{0:04x}\n".format(((ddr_file_size >> 16) & 0xFFFF)))

        for index, line in enumerate(in_file_lines):
            s_0, s_1, s_2, s_3 = line_2_ddr_slices(line)
            # print("{4:} :: {0:x}\t{1:x}\t{2:x}\t{3:x}".format( s_0, s_1, s_2, s_3, line))

            #Even lines
            slice_0_file.write("{0:04x}\n".format(s_0))
            slice_0_file.write("{0:04x}\n".format(s_2))

            #Odd lines
            slice_1_file.write("{0:04x}\n".format(s_1))
            slice_1_file.write("{0:04x}\n".format(s_3))


        input_hex.close()
        slice_0_file.close()
        slice_1_file.close()

    #exit(0)
