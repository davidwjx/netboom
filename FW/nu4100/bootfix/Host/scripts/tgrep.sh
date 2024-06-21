#!/bin/sh

/usr/bin/grep -v log bcreport.txt > b1.txt
/usr/bin/mv b1.txt bcreport.txt

if [ -s bcreport.txt ]
then
	/usr/bin/cat bcreport.txt	
	echo 1 > tmpFile
else
	echo 0 > tmpFile
fi
