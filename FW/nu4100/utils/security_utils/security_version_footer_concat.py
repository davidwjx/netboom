#!/usr/bin/python

#How to use-
#python sign_script.py 'path to bin' fw_ver sec_ver tbd
#assumptions:
#Zero padding of 8 Bytes
#fw_ver is structured - 4.19.1.23 (major.minor.build.subbuild- 8bit,8bit,4bit,12bit)
#sec_ver is between 0-63 (1 byte)
#tbd - (3 bytes)
# python header_concat.py CM.pub.bin 4.19.1.23 3 0


import sys
import os
import shutil

def getSize(filename):
    st = os.stat(filename)
    return st.st_size



def convert_to_4_bytes( fw_ver,sec_ver,tbd):
    
    ind=0
    out_vec_4_bytes=bytearray(4*4) #8 Bytes of zeros + 4 bytes for fw_ver and 1 byte for sec_ver

    ll = int(0)
    out_vec_4_bytes[4 * ind] = ll & 0xFF
    out_vec_4_bytes[4 * ind + 1] = (ll >> 8) & 0xFF
    out_vec_4_bytes[4 * ind + 2] = (ll >> 16) & 0xFF
    out_vec_4_bytes[4 * ind + 3] = (ll >> 24) & 0xFF
    ind=ind+1

    ll = int(0)
    out_vec_4_bytes[4 * ind] = ll & 0xFF
    out_vec_4_bytes[4 * ind + 1] = (ll >> 8) & 0xFF
    out_vec_4_bytes[4 * ind + 2] = (ll >> 16) & 0xFF
    out_vec_4_bytes[4 * ind + 3] = (ll >> 24) & 0xFF
    ind=ind+1

    numbers=fw_ver.split('.')
    #print(numbers)
    ll=(int(numbers[0]))+(int(numbers[1])<<8)+(int(numbers[2])<<16)+(int(numbers[3])<<20)
    #print(ll)
    out_vec_4_bytes[4*ind]= ll&0xFF
    out_vec_4_bytes[4*ind+1]= (ll>>8)&0xFF
    out_vec_4_bytes[4*ind+2]=(ll>>16)&0xFF
    out_vec_4_bytes[4*ind+3]= (ll>>24)&0xFF
    ind=ind+1
    
    ll=(int(sec_ver))+(int(tbd)<<8)
    out_vec_4_bytes[4*ind]= ll&0xFF
    out_vec_4_bytes[4*ind+1]= (ll>>8)&0xFF
    out_vec_4_bytes[4*ind+2]=(ll>>16)&0xFF
    out_vec_4_bytes[4*ind+3]= (ll>>24)&0xFF
    return out_vec_4_bytes

def security_version_footer_concat(inputs_params):

    if len(inputs_params) != 6:
        print("***********problem with the inputs to the python script called 'header_concat.py' *****************")
        print(inputs_params)
        print("length of inputs_params="+str(len(inputs_params)))
        sys.exit()

    path_dir = inputs_params[1]
    file_name = inputs_params[2]
    fw_ver = inputs_params[3]
    sec_ver = inputs_params[4]
    tbd = inputs_params[5]

    path_to_file = path_dir + 'before_sign_' + file_name
    output_file_name = path_dir + file_name
    os.rename(output_file_name, path_to_file)

    f = open(output_file_name, 'w+b')
    # to view in linux  -  od -t u4 my_file
    byte_arr=convert_to_4_bytes(fw_ver, sec_ver,tbd)
    binary_format = bytearray(byte_arr)
    #f.write(binary_format)
    #f.close()
    write_file = "output"
    if os.path.exists(write_file):
        os.remove(write_file)
    if os.path.exists(output_file_name):
        os.remove(output_file_name)

    orig_size=getSize(path_to_file); print("orig_size", orig_size)
    if (orig_size%16 !=0):
        padding_size = 16 - (orig_size % 16);print("padding_size", padding_size)
        read_file1 = path_to_file

        with open(write_file, "ab") as file3:
            with open(read_file1, "rb") as file1:
                data = file1.read()
                file3.write(data)
            while padding_size > 0:
                file3.write(b'\x00')
                padding_size -= 1
        with open(output_file_name, "ab") as myfile, open(write_file, "rb") as file2:
            myfile.write(file2.read())
            myfile.write(binary_format)

        os.remove(write_file)
    else:
        #concat the two bins
        with open(output_file_name, "ab") as myfile, open(path_to_file, "rb") as file2:
            myfile.write(file2.read())
            myfile.write(binary_format)

    os.remove(path_to_file)
#assert 1>2, "stop"


if __name__ == "__main__":
    inputs_params=sys.argv
    security_version_footer_concat(inputs_params)