#!/usr/bin/python

# How to use-
# python sign_script.py 'full path to dir of binaries to sign' 'full path to dir of output signed binaries' 'full path to sign private key.der'
# python sign_script.py /space/users/alons/trunk_4100/host-utils/secureBootTools/SignTool/build_linux/to_sign/ /space/users/alons/trunk_4100/host-utils/secureBootTools/SignTool/build_linux/signed_bins/ /space/users/alons/trunk_4100/host-utils/secureBootTools/SignTool/build_linux/DM.der


import sys
import os
#import shutil
from security_version_footer_concat import *


inputs_params = sys.argv

if len(inputs_params) != 7:
    print("***********problem with the inputs to the python script called 'sign_script.py' *****************")
    print(inputs_params)
    print("length of inputs_params=" + str(len(inputs_params)))
    sys.exit()

dir_bin_to_sign_fullpath = inputs_params[1]
dir_signed_bins_fullpath = inputs_params[2]
sign_key_fullpath = inputs_params[3]
fw_ver = inputs_params[4]#"4.19.0002.29"#
sec_ver = inputs_params[5]#0#
tbd = inputs_params[6]#0#

os.system("rm -f " + dir_signed_bins_fullpath + "*")

str1 = './signtool sign -type BLp -pubkeytype otp -prikey '

for subdir, dirs, files in os.walk(dir_bin_to_sign_fullpath):
    for file in files:
        print(file, subdir)
        input_params_footer = [0, subdir, file, fw_ver, sec_ver, tbd]
        security_version_footer_concat(input_params_footer)

for subdir, dirs, files in os.walk(dir_bin_to_sign_fullpath):
    for file in files:
        if file.__contains__(".bin"):
            sign_command_string = str1 + sign_key_fullpath + " -infile " + subdir + file + " -outfile " + dir_signed_bins_fullpath + file
        else:
            sign_command_string = str1 + sign_key_fullpath + " -infile " + subdir + file + " -outfile " + dir_signed_bins_fullpath + file + ".bin"
        # print(sign_command_string)
        os.system(sign_command_string)
        # assert 1>2, "stop"
