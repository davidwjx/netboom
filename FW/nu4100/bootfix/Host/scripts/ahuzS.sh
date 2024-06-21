#!/bin/sh -x

cd version
sed 's/\./%s\./g' version.h > v1
mv v1 version.h

exit 0
