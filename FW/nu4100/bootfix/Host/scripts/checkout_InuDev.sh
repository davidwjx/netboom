#!/bin/bash

##This file is located in the applications's source directory

HOST_BRANCH=trunk
FW_BRANCH=trunk
ALGO_BRANCH=trunk
COMMON_BRANCH=trunk

if [ -n "$1" ] 
then
    
    HOST_BRANCH="$1"
    FW_BRANCH="$1"
    ALGO_BRANCH="$1"
    COMMON_BRANCH="$1"

    if [ -n "$2" ]; 
    then
        FW_BRANCH="$2"
        ALGO_BRANCH="$2"
        COMMON_BRANCH="$2"

        if [ -n "$3" ]
        then

            ALGO_BRANCH="$3"
            COMMON_BRANCH="$3"

            if [ -n "$4" ]
            then

                COMMON_BRANCH="$4"

            fi
        fi
    fi
fi

WORK_DIRECTORY=.

echo HOST_BRANCH= $HOST_BRANCH
echo FW_BRANCH= $FW_BRANCH
echo ALGO_BRANCH= $ALGO_BRANCH
echo COMMON_BRANCH= $COMMON_BRANCH

SVN_PATH=http://svn/repos/inuitive/db/NU3000/project_ver0/sw

cd $WORK_DIRECTORY$

svn co --depth empty $SVN_PATH/$FW_BRANCH/FW $WORK_DIRECTORY/FW
svn update --set-depth empty     FW/app
svn update --set-depth infinity  FW/app/api
svn update --set-depth infinity  FW/app/build
svn update --set-depth infinity  FW/app/common
svn update --set-depth infinity  FW/app/host


svn co --depth empty $SVN_PATH/$HOST_BRANCH/Host $WORK_DIRECTORY/Host
svn update --set-depth empty     Host/dev
svn update --set-depth infinity  Host/scripts
svn update --set-depth infinity  Host/dev/3rdParty
svn update --set-depth infinity  Host/dev/InuDev

svn update --set-depth infinity  Host/dev/3rdParty

svn co --depth empty $SVN_PATH/$ALGO_BRANCH/Algo $WORK_DIRECTORY/Algo
svn update --set-depth empty     Algo/C++
svn update --set-depth infinity  Algo/C++/Common
svn update --set-depth infinity  Algo/C++/haarcascades
svn update --set-depth infinity  Algo/C++/DeployKit
svn update --set-depth infinity  Algo/C++/Tools

svn update --set-depth infinity  Algo/C++/CSE



svn co --depth empty $SVN_PATH/$COMMON_BRANCH/Common $WORK_DIRECTORY/Common
svn update --set-depth infinity  Common

