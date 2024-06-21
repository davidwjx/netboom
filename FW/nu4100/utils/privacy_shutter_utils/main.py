# This is a sample Python script.

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.

# See PyCharm help at https://www.jetbrains.com/help/pycharm/

import numpy as np
pathHD = 'privacy_16x9 (002) YUV 720p.raw'
pathfullHD = 'privacy_16x9 (002) YUV 1080p.raw'

white=33020
grey1=32788
grey2=32980
grey3=32852
grey4=32836
grey5=32804
grey6=33019


###################################################HD picture
fd = open(pathHD, 'rb')
rows = 720
cols = 1280
f = np.fromfile(fd, dtype=np.uint16,count=rows*cols)
im = f.reshape((rows,cols)) #notice row, column format


#file mng:
file = open('arrays_HD.txt', 'w')
print("###################################################HD picture")
whites_rows, whites_cols = np.where(im == white)
file.write('const unsigned short white_HD[] =\n{\n')
i=0
lines=0;
while (i <len(whites_rows) -1):
     count=1
     j=i
     while (( (j<(len(whites_rows)-2) ) & (whites_rows[j]==whites_rows[j+1]) )  & (whites_cols[j+1]==whites_cols[j]+1 )   ):
         count=count+1
         j=j+1
     file.write(str(whites_rows[i]) + "," + str(whites_cols[i]) + "," + str(count) + ",")
     i=i+count
     lines=lines+1
file.write('\n};\n')
print("white ",lines*3)

grey=grey1;
grey_rows, grey_cols = np.where(im == grey)
file.write('const unsigned short grey1_HD[] =\n{\n')
i=0
lines=0;
while (i <len(grey_rows) -1):
     count=1
     j=i
     while (( (j<(len(grey_rows)-2) ) & (grey_rows[j]==grey_rows[j+1]) )  & (grey_cols[j+1]==grey_cols[j]+1 )   ):
         count=count+1
         j=j+1
     file.write(str(grey_rows[i])+","+str(grey_cols[i])+","+str(count)+",")
     i=i+count
     lines=lines+1
file.write('\n};\n')
print("grey1 ",lines*3)

grey=grey2;
grey_rows, grey_cols = np.where(im == grey)
file.write('const unsigned short grey2_HD[] =\n{\n')
i=0
lines=0;
while (i <len(grey_rows) -1):
     count=1
     j=i
     while (( (j<(len(grey_rows)-2) ) & (grey_rows[j]==grey_rows[j+1]) )  & (grey_cols[j+1]==grey_cols[j]+1 )   ):
         count=count+1
         j=j+1
     file.write(str(grey_rows[i])+","+str(grey_cols[i])+","+str(count)+",")
     i=i+count
     lines=lines+1
file.write('\n};\n')
print("grey2 ",lines*3)

grey=grey3;
grey_rows, grey_cols = np.where(im == grey)
file.write('const unsigned short grey3_HD[] =\n{\n')
i=0
lines=0;
while (i <len(grey_rows) -1):
     count=1
     j=i
     while (( (j<(len(grey_rows)-2) ) & (grey_rows[j]==grey_rows[j+1]) )  & (grey_cols[j+1]==grey_cols[j]+1 )   ):
         count=count+1
         j=j+1
     file.write(str(grey_rows[i])+","+str(grey_cols[i])+","+str(count)+",")
     i=i+count
     lines=lines+1
file.write('\n};\n')
print("grey3 ",lines*3)

grey=grey4;
grey_rows, grey_cols = np.where(im == grey)
file.write('const unsigned short grey4_HD[] =\n{\n')
i=0
lines=0;
while (i <len(grey_rows) -1):
     count=1
     j=i
     while (( (j<(len(grey_rows)-2) ) & (grey_rows[j]==grey_rows[j+1]) )  & (grey_cols[j+1]==grey_cols[j]+1 )   ):
         count=count+1
         j=j+1
     file.write(str(grey_rows[i])+","+str(grey_cols[i])+","+str(count)+",")
     i=i+count
     lines=lines+1
file.write('\n};\n')
print("grey4 ",lines*3)

grey=grey5;
grey_rows, grey_cols = np.where(im == grey)
file.write('const unsigned short grey5_HD[] =\n{\n')
i=0
lines=0;
while (i <len(grey_rows) -1):
     count=1
     j=i
     while (( (j<(len(grey_rows)-2) ) & (grey_rows[j]==grey_rows[j+1]) )  & (grey_cols[j+1]==grey_cols[j]+1 )   ):
         count=count+1
         j=j+1
     file.write(str(grey_rows[i])+","+str(grey_cols[i])+","+str(count)+",")
     i=i+count
     lines=lines+1
file.write('\n};\n')
print("grey5 ",lines*3)


#close arrays file
file.close() #close file

fd = open(pathHD, 'rb')
rows = 720
cols = 1280*2
f_present = np.fromfile(fd, dtype=np.uint8,count=rows*cols)
im_present = f_present.reshape((rows,cols)) #notice row, column format
fd.close()
import cv2
cv2.imshow('', im_present)
cv2.waitKey()
cv2.destroyAllWindows()



###################################################now full HD picture
fd = open(pathfullHD, 'rb')
rows = 1080
cols = 1920
f = np.fromfile(fd, dtype=np.uint16,count=rows*cols)
im = f.reshape((rows,cols)) #notice row, column format

#file mng:
file = open('arrays_fullHD.txt', 'w')
print("###################################################full HD picture")
whites_rows, whites_cols = np.where(im == white)
file.write('const unsigned short white_FullHD[] =\n{\n')
i=0
lines=0;
while (i <len(whites_rows) -1):
     count=1
     j=i
     while (( (j<(len(whites_rows)-2) ) & (whites_rows[j]==whites_rows[j+1]) )  & (whites_cols[j+1]==whites_cols[j]+1 )   ):
         count=count+1
         j=j+1
     file.write(str(whites_rows[i]) + "," + str(whites_cols[i]) + "," + str(count) + ",")
     i=i+count
     lines=lines+1
file.write('\n};\n')
print("white ",lines*3)

grey=grey1;
grey_rows, grey_cols = np.where(im == grey)
file.write('const unsigned short grey1_FullHD[] =\n{\n')
i=0
lines=0;
while (i <len(grey_rows) -1):
     count=1
     j=i
     while (( (j<(len(grey_rows)-2) ) & (grey_rows[j]==grey_rows[j+1]) )  & (grey_cols[j+1]==grey_cols[j]+1 )   ):
         count=count+1
         j=j+1
     file.write(str(grey_rows[i])+","+str(grey_cols[i])+","+str(count)+",")
     i=i+count
     lines=lines+1
file.write('\n};\n')
print("grey1 ",lines*3)

grey=grey2;
grey_rows, grey_cols = np.where(im == grey)
file.write('const unsigned short grey2_FullHD[] =\n{\n')
i=0
lines=0;
while (i <len(grey_rows) -1):
     count=1
     j=i
     while (( (j<(len(grey_rows)-2) ) & (grey_rows[j]==grey_rows[j+1]) )  & (grey_cols[j+1]==grey_cols[j]+1 )   ):
         count=count+1
         j=j+1
     file.write(str(grey_rows[i])+","+str(grey_cols[i])+","+str(count)+",")
     i=i+count
     lines=lines+1
file.write('\n};\n')
print("grey2 ",lines*3)

grey=grey3;
grey_rows, grey_cols = np.where(im == grey)
file.write('const unsigned short grey3_FullHD[] =\n{\n')
i=0
lines=0;
while (i <len(grey_rows) -1):
     count=1
     j=i
     while (( (j<(len(grey_rows)-2) ) & (grey_rows[j]==grey_rows[j+1]) )  & (grey_cols[j+1]==grey_cols[j]+1 )   ):
         count=count+1
         j=j+1
     file.write(str(grey_rows[i])+","+str(grey_cols[i])+","+str(count)+",")
     i=i+count
     lines=lines+1
file.write('\n};\n')
print("grey3 ",lines*3)

grey=grey4;
grey_rows, grey_cols = np.where(im == grey)
file.write('const unsigned short grey4_FullHD[] =\n{\n')
i=0
lines=0;
while (i <len(grey_rows) -1):
     count=1
     j=i
     while (( (j<(len(grey_rows)-2) ) & (grey_rows[j]==grey_rows[j+1]) )  & (grey_cols[j+1]==grey_cols[j]+1 )   ):
         count=count+1
         j=j+1
     file.write(str(grey_rows[i])+","+str(grey_cols[i])+","+str(count)+",")
     i=i+count
     lines=lines+1
file.write('\n};\n')
print("grey4 ",lines*3)

grey=grey5;
grey_rows, grey_cols = np.where(im == grey)
file.write('const unsigned short grey5_FullHD[] =\n{\n')
i=0
lines=0;
while (i <len(grey_rows) -1):
     count=1
     j=i
     while (( (j<(len(grey_rows)-2) ) & (grey_rows[j]==grey_rows[j+1]) )  & (grey_cols[j+1]==grey_cols[j]+1 )   ):
         count=count+1
         j=j+1
     file.write(str(grey_rows[i])+","+str(grey_cols[i])+","+str(count)+",")
     i=i+count
     lines=lines+1
file.write('\n};\n')
print("grey5 ",lines*3)


grey=grey6;
grey_rows, grey_cols = np.where(im == grey)
file.write('const unsigned short grey6_FullHD[] =\n{\n')
i=0
lines=0;
while (i <len(grey_rows) -1):
     count=1
     j=i
     while (( (j<(len(grey_rows)-2) ) & (grey_rows[j]==grey_rows[j+1]) )  & (grey_cols[j+1]==grey_cols[j]+1 )   ):
         count=count+1
         j=j+1
     file.write(str(grey_rows[i])+","+str(grey_cols[i])+","+str(count)+",")
     i=i+count
     lines=lines+1
file.write('\n};\n')
print("grey6 ",lines*3)


grey=33017;
grey_rows, grey_cols = np.where(im == grey)
file.write('const unsigned short grey7_FullHD[] =\n{\n')
i=0
lines=0;
while (i <len(grey_rows) -1):
     count=1
     j=i
     while (( (j<(len(grey_rows)-2) ) & (grey_rows[j]==grey_rows[j+1]) )  & (grey_cols[j+1]==grey_cols[j]+1 )   ):
         count=count+1
         j=j+1
     file.write(str(grey_rows[i])+","+str(grey_cols[i])+","+str(count)+",")
     i=i+count
     lines=lines+1
file.write('\n};\n')
print("grey7 ",lines*3)



grey=33004
grey_rows, grey_cols = np.where(im == grey)
file.write('const unsigned short grey8_FullHD[] =\n{\n')
i=0
lines=0;
while (i <len(grey_rows) -1):
     count=1
     j=i
     while (( (j<(len(grey_rows)-2) ) & (grey_rows[j]==grey_rows[j+1]) )  & (grey_cols[j+1]==grey_cols[j]+1 )   ):
         count=count+1
         j=j+1
     file.write(str(grey_rows[i])+","+str(grey_cols[i])+","+str(count)+",")
     i=i+count
     lines=lines+1
file.write('\n};\n')
print("grey8 ",lines*3)


grey=32988
grey_rows, grey_cols = np.where(im == grey)
file.write('const unsigned short grey9_FullHD[] =\n{\n')
i=0
lines=0;
while (i <len(grey_rows) -1):
     count=1
     j=i
     while (( (j<(len(grey_rows)-2) ) & (grey_rows[j]==grey_rows[j+1]) )  & (grey_cols[j+1]==grey_cols[j]+1 )   ):
         count=count+1
         j=j+1
     file.write(str(grey_rows[i])+","+str(grey_cols[i])+","+str(count)+",")
     i=i+count
     lines=lines+1
file.write('\n};\n')
print("grey9 ",lines*3)


grey=32920
grey_rows, grey_cols = np.where(im == grey)
file.write('const unsigned short grey10_FullHD[] =\n{\n')
i=0
lines=0;
while (i <len(grey_rows) -1):
     count=1
     j=i
     while (( (j<(len(grey_rows)-2) ) & (grey_rows[j]==grey_rows[j+1]) )  & (grey_cols[j+1]==grey_cols[j]+1 )   ):
         count=count+1
         j=j+1
     file.write(str(grey_rows[i])+","+str(grey_cols[i])+","+str(count)+",")
     i=i+count
     lines=lines+1
file.write('\n};\n')
print("grey10 ",lines*3)

grey=32774
grey_rows, grey_cols = np.where(im == grey)
file.write('const unsigned short grey11_FullHD[] =\n{\n')
i=0
lines=0;
while (i <len(grey_rows) -1):
     count=1
     j=i
     while (( (j<(len(grey_rows)-2) ) & (grey_rows[j]==grey_rows[j+1]) )  & (grey_cols[j+1]==grey_cols[j]+1 )   ):
         count=count+1
         j=j+1
     file.write(str(grey_rows[i])+","+str(grey_cols[i])+","+str(count)+",")
     i=i+count
     lines=lines+1
file.write('\n};\n')
print("grey11 ",lines*3)

grey=32784
grey_rows, grey_cols = np.where(im == grey)
file.write('const unsigned short grey12_FullHD[] =\n{\n')
i=0
lines=0;
while (i <len(grey_rows) -1):
     count=1
     j=i
     while (( (j<(len(grey_rows)-2) ) & (grey_rows[j]==grey_rows[j+1]) )  & (grey_cols[j+1]==grey_cols[j]+1 )   ):
         count=count+1
         j=j+1
     file.write(str(grey_rows[i])+","+str(grey_cols[i])+","+str(count)+",")
     i=i+count
     lines=lines+1
file.write('\n};\n')
print("grey12 ",lines*3)

grey=32816
grey_rows, grey_cols = np.where(im == grey)
file.write('const unsigned short grey13_FullHD[] =\n{\n')
i=0
lines=0;
while (i <len(grey_rows) -1):
     count=1
     j=i
     while (( (j<(len(grey_rows)-2) ) & (grey_rows[j]==grey_rows[j+1]) )  & (grey_cols[j+1]==grey_cols[j]+1 )   ):
         count=count+1
         j=j+1
     file.write(str(grey_rows[i])+","+str(grey_cols[i])+","+str(count)+",")
     i=i+count
     lines=lines+1
file.write('\n};\n')
print("grey13 ",lines*3)

grey=32896
grey_rows, grey_cols = np.where(im == grey)
file.write('const unsigned short grey14_FullHD[] =\n{\n')
i=0
lines=0;
while (i <len(grey_rows) -1):
     count=1
     j=i
     while (( (j<(len(grey_rows)-2) ) & (grey_rows[j]==grey_rows[j+1]) )  & (grey_cols[j+1]==grey_cols[j]+1 )   ):
         count=count+1
         j=j+1
     file.write(str(grey_rows[i])+","+str(grey_cols[i])+","+str(count)+",")
     i=i+count
     lines=lines+1
file.write('\n};\n')
print("grey14 ",lines*3)

grey=32966
grey_rows, grey_cols = np.where(im == grey)
file.write('const unsigned short grey15_FullHD[] =\n{\n')
i=0
lines=0;
while (i <len(grey_rows) -1):
     count=1
     j=i
     while (( (j<(len(grey_rows)-2) ) & (grey_rows[j]==grey_rows[j+1]) )  & (grey_cols[j+1]==grey_cols[j]+1 )   ):
         count=count+1
         j=j+1
     file.write(str(grey_rows[i])+","+str(grey_cols[i])+","+str(count)+",")
     i=i+count
     lines=lines+1
file.write('\n};\n')
print("grey15 ",lines*3)


#close arrays file
file.close() #close file

fd.close()
fd = open(pathfullHD, 'rb')
rows = 1080
cols = 1920 * 2
f_present = np.fromfile(fd, dtype=np.uint8,count=rows*cols)
im_present = f_present.reshape((rows,cols)) #notice row, column format
fd.close()
import cv2
cv2.imshow('', im_present)
cv2.waitKey()
cv2.destroyAllWindows()
