#!/bin/bash -x

########################################################################################
# Name:		~cmadmin/scripts/inumodeldb_per_type.sh
# Purpose:	Create per-type-group InuModelCSV files
# Run by:	cmadmin@UB05
# By:		Yariv Sheizaf
# Date:		29-Feb-2016
# Arguments:
########################################################################################

export PATH=$PATH:/usr/bin

if [ ! -f InuModelDB.csv ]
then
	echo "InuModelDB.csv does not exist. Exit..."
	exit 1
fi
	
rm -f InuModelDB_F*.csv
# dos2unix InuModelDB.csv
sed 's/ /@/g' InuModelDB.csv | tail -n +2 | grep -v Default | grep ",boot" > $$.tmp1

for i in `cat $$.tmp1`
do
	X=`echo $i | awk -F,boot '{print $2/100}' | awk -F. '{print $1}'`
	if [ "$X" = "0" ]
	then
		FFILE="InuModelDB_F0.csv"
	else
		FFILE="InuModelDB_F"$X"00.csv"
	fi

	if [ ! -f $FFILE ]
	then
		head -1 InuModelDB.csv > $FFILE
	fi

	echo $i | sed 's/@/ /g' >> $FFILE
done

### Special for boot400 that has not line in InuModelDB.csv
if [ ! -f InuModelDB_F400.csv ]
then
	head -1 InuModelDB.csv > InuModelDB_F400.csv
fi
	
for j in `ls InuModelDB_F*.csv`
do
	FNUM=`echo $j | awk -FF '{print $2}' | awk -F. '{print "boot"$1}'`
	grep Default InuModelDB.csv | sed 's/boot0/'$FNUM'/' >> $j
done

rm $$.*

exit 0
