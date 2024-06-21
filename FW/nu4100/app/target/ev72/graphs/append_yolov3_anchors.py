#!/usr/bin/python

# How to use-
# python append_yolov3_anchors.py  /tools/cnn_models/caffe_models/yolov3_alpine/caffe_model/yolov3_alpine_deconv.prototxt ../graphs/2019_06/inu_multi_lib/cnn_obj/nu4000b0_release_unmerged_large/cnn_bin_yolo.bin

import sys
import os
import shutil
#import struct
#import fileinput
#import numpy as np



def convert_to_4_bytes( vec, nms_th, conf_th, num_classes, ptx_version):
	
	ind=0
	out_vec_4_bytes=bytearray(4*len(vec)+6*4)
	for i in vec:
			out_vec_4_bytes[4*ind]= i&0xFF
			out_vec_4_bytes[4*ind+1]= (i>>8)&0xFF
			out_vec_4_bytes[4*ind+2]=(i>>16)&0xFF
			out_vec_4_bytes[4*ind+3]= (i>>24)&0xFF
			ind=ind+1

	ll=len(vec)			#num of anchors
	out_vec_4_bytes[4*ind]= ll&0xFF
	out_vec_4_bytes[4*ind+1]= (ll>>8)&0xFF
	out_vec_4_bytes[4*ind+2]=(ll>>16)&0xFF
	out_vec_4_bytes[4*ind+3]= (ll>>24)&0xFF
	ind=ind+1
	
	ll=nms_th			#nms_th (multiplied by 100)
	out_vec_4_bytes[4*ind]= ll&0xFF
	out_vec_4_bytes[4*ind+1]= (ll>>8)&0xFF
	out_vec_4_bytes[4*ind+2]=(ll>>16)&0xFF
	out_vec_4_bytes[4*ind+3]= (ll>>24)&0xFF
	ind=ind+1
	
	ll=conf_th			#conf_th (multiplied by 100)
	out_vec_4_bytes[4*ind]= ll&0xFF
	out_vec_4_bytes[4*ind+1]= (ll>>8)&0xFF
	out_vec_4_bytes[4*ind+2]=(ll>>16)&0xFF
	out_vec_4_bytes[4*ind+3]= (ll>>24)&0xFF
	ind=ind+1
	
	ll=num_classes		#num of classes
	out_vec_4_bytes[4*ind]= ll&0xFF
	out_vec_4_bytes[4*ind+1]= (ll>>8)&0xFF
	out_vec_4_bytes[4*ind+2]=(ll>>16)&0xFF
	out_vec_4_bytes[4*ind+3]= (ll>>24)&0xFF
	ind=ind+1
	
	ll=ptx_version		#prototxt version it was generated from
	out_vec_4_bytes[4*ind]= ll&0xFF
	out_vec_4_bytes[4*ind+1]= (ll>>8)&0xFF
	out_vec_4_bytes[4*ind+2]=(ll>>16)&0xFF
	out_vec_4_bytes[4*ind+3]= (ll>>24)&0xFF
	ind=ind+1
	
	ll=3405695742		#CAFECAFE
	out_vec_4_bytes[4*ind]= ll&0xFF
	out_vec_4_bytes[4*ind+1]= (ll>>8)&0xFF
	out_vec_4_bytes[4*ind+2]=(ll>>16)&0xFF
	out_vec_4_bytes[4*ind+3]= (ll>>24)&0xFF
	return out_vec_4_bytes


inputs_params=sys.argv

if len(inputs_params)!=3:
	print("**********************************************************************************************************")
	print("***********problem with the inputs to the python script called 'append_yolov3_achors.py' *****************")
	print(inputs_params)
	print("length of inputs_params="+str(len(inputs_params)))
	sys.exit() 
	
yolov3_prototxt_fullpath=inputs_params[1]
cnn_binary_name=inputs_params[2]


#open prototxt file
anchor_exist=0
no_anchor_print = 0	
with open(yolov3_prototxt_fullpath) as search:
	for line in search:
		line = line.rstrip()  # remove '\n' at end of line
		if line.find('num_anchors_element') != -1:
			#print(line )
			ii=line[22:].split(',')
			num_anc=int(ii[0])
			#print(num_anc) 
			anchor_exist=anchor_exist+1
		elif line.find('anchors') != -1:
			anchor_exist=anchor_exist+1
			#print(line )
			jj=line[11:].split(',')
			ind=0
			anc=[0] * num_anc
			for str in jj:
				anc[ind]=int(str)
				ind=ind+1
		elif line.find('nms_th') != -1:
			zz=line[9:].split(',')
			nms_th=int(zz[0])
			anchor_exist=anchor_exist+1
		elif line.find('conf_th') != -1:
			zz1=line[10:].split(',')
			conf_th=int(zz1[0])
			anchor_exist=anchor_exist+1
		elif line.find('num_classes') != -1:
			zz2=line[14:].split(',')
			num_classes=int(zz2[0])
			anchor_exist=anchor_exist+1
		elif line.find('ptx_version') != -1:
			zz3=line[14:].split(',')
			ptx_version=int(zz3[0])
			anchor_exist=anchor_exist+1
			
		
with open(cnn_binary_name, mode='rb') as binaryfile: # b is important -> binary
	myArr = bytearray(binaryfile.read())			
if ((hex(myArr[-4]) == hex(0xfe))  & (hex(myArr[-3]) == hex(0xca) ) & (hex(myArr[-2]) == hex(0xfe) ) & ((hex(myArr[-1]) == hex(0xca)))):
	anchor_exist = 0
	offset= cnn_binary_name.rfind("/")
	local_bin_name = cnn_binary_name[offset+1:]
	
	print(local_bin_name + " CNN binary already includes piggyback yolo info - No action required")
	no_anchor_print = 1
		
if anchor_exist==6:
	f = open('piggy.bin', 'w+b')

	# to view in linux  -  od -t u4 my_file
	byte_arr=convert_to_4_bytes(anc, nms_th, conf_th, num_classes, ptx_version)
	binary_format = bytearray(byte_arr)
	f.write(binary_format)
	f.close()


	with open(cnn_binary_name, "ab") as myfile, open("piggy.bin", "rb") as file2:
		myfile.write(file2.read())
		
	#delete piggy.bin
	os.system("rm piggy.bin")
else:
	if no_anchor_print == 0:
		print ("Warning: no anchors data was found in the prototxt file in: " +yolov3_prototxt_fullpath )
		if anchor_exist>0:
			print (" missing data: anchor_exists="+str(anchor_exist))
		
		
print (" -----Python script finished-----")		

#assert 1>2, "stop"
