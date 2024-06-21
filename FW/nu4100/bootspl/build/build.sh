#!/bin/bash
set -e
SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
BOOTSPL_DIR=$SCRIPT_DIR/../

param_board_name=nu4000c1

# Usage:
function func_usage
{
        echo "Usage of $0:"
        echo -e "\t-h : usage help, no parameters"
        echo -e "\t-c (--clean) : clean build"
        echo -e "\t-p (--platform) : build platform (boot0, ...)"
        echo -e "\t-f (--ddrfreq) : ddr freq (533,800(default),1000,1067,1200,1334,1400(default boot 87))"
        echo -e "\t-b (--board) : (c0,c1)"
        echo -e "\t-s (--svnuse) : svn for version"
        echo -e "\t-z (--zebu) : Create Zebu loadable memory slices"
}
function func_parse_test_params
{
        while [[ $# > 0 ]]
        do
        key="$1"
        shift

        case $key in
                -h|--help)
                param_help=1
                ;;
                -c|--clean)
                param_clean=-c
                ;;
                -p|--platform)
                param_platform="$1"
                shift
                ;;
                -f|--ddrfreq)
                param_ddr_freq="$1"
                shift
                ;;
                -s|--svnuse)
                param_svn=1
                ;;
                -z|--zebu)
                zebu=1
                ;;
                -o|--appoffset)
                param_app_offset="$1"
                shift
                ;;
                -h|--halfpll)
                param_200mhz_pll="$1"
                shift
                ;;
                *)
                                # unknown option
                ;;
        esac
        done
}


func_parse_test_params "$@"

if [ ! -z "$param_help" ]; then
    func_usage
    exit 10
fi


[ -z "$param_platform" ] &&  param_platform="boot0"
if [ ! -d "$BOOTSPL_DIR/src/ddr/$param_platform" ]; then
        param_platform="boot0"
fi
export PLATFORM_NAME=$param_platform

if [ -z "$param_ddr_freq" ] 
then
       param_ddr_freq="1400"
fi

default_ddr_freq=1400

export DDR_FREQ=$param_ddr_freq
export DEFAULT_DDR_FREQ=$default_ddr_freq

if [ ! -z "$param_svn" ]; then
export SVN_USE=$param_svn
fi

[ ! -z "$param_app_offset" ] && export APP_OFFSET=$param_app_offset
[ ! -z "$param_200mhz_pll" ] && export DDR_PLL_200M=1

WORKING_DIR=${PWD}

echo "Building bootspl"
mkdir -p $BOOTSPL_DIR/bin
cd $BOOTSPL_DIR/src
BOOTID="$(echo $param_platform | sed 's/boot//g')"
        

if [ "$zebu" == 1 ]
then
        echo "Building BOOTSPL for Zebu"
        RELEASE=1 CHIP_ID=$param_board_name make -j EXTCDEFINES="-D_ZEBU -DBOOT_$BOOTID" clean 
        RELEASE=1 CHIP_ID=$param_board_name make -j EXTCDEFINES="-D_ZEBU -DBOOT_$BOOTID" bootrom_include 
        RELEASE=1 CHIP_ID=$param_board_name make -j EXTCDEFINES="-D_ZEBU -DBOOT_$BOOTID" all 
        RELEASE=1 CHIP_ID=$param_board_name make -j EXTCDEFINES="-D_ZEBU -DBOOT_$BOOTID" binary 
        RELEASE=1 CHIP_ID=$param_board_name make -j EXTCDEFINES="-D_ZEBU -DBOOT_$BOOTID" hex 
else
        if [ $param_platform == "boot86" ]
        then
                export BOOT_ID=86
        fi
        if [ $param_platform == "boot87" ]
        then
                export BOOT_ID=87
        fi
        if [ $param_platform == "boot500" ]
        then
                export BOOT_ID=500
        fi
        if [ $param_platform == "boot51" ]
        then
                echo "Building BOOTSPL boot51"
                export BOOT_ID=51
                RELEASE=1 CHIP_ID=$param_board_name make -j EXTCDEFINES="-D__NAND_FLASH__ -DBOOT_$BOOTID" clean 
                RELEASE=1 CHIP_ID=$param_board_name make -j EXTCDEFINES="-D__NAND_FLASH__ -DBOOT_$BOOTID" bootrom_include 
                RELEASE=1 CHIP_ID=$param_board_name make -j EXTCDEFINES="-D__NAND_FLASH__ -DBOOT_$BOOTID" all 
                RELEASE=1 CHIP_ID=$param_board_name make -j EXTCDEFINES="-D__NAND_FLASH__ -DBOOT_$BOOTID" binary 
                RELEASE=1 CHIP_ID=$param_board_name make -j EXTCDEFINES="-D__NAND_FLASH__ -DBOOT_$BOOTID" hex 
        else
                echo "Building BOOTSPL default "
                RELEASE=1 CHIP_ID=$param_board_name make -j EXTCDEFINES=-DBOOT_$BOOTID clean 
                RELEASE=1 CHIP_ID=$param_board_name make -j EXTCDEFINES=-DBOOT_$BOOTID bootrom_include 
                RELEASE=1 CHIP_ID=$param_board_name make -j EXTCDEFINES=-DBOOT_$BOOTID all 
                RELEASE=1 CHIP_ID=$param_board_name make -j EXTCDEFINES=-DBOOT_$BOOTID binary 
                RELEASE=1 CHIP_ID=$param_board_name make -j EXTCDEFINES=-DBOOT_$BOOTID hex 
       fi
fi

echo "DEFAULT_DDR_FREQ=$default_ddr_freq "

if [ $param_ddr_freq -eq $default_ddr_freq ]
then
  cp -av bootspl-$param_board_name.bin $BOOTSPL_DIR/bin/bootspl-nu4100.bin
  mv bootspl-$param_board_name.bin bootspl-nu4100.bin
else
  cp -av bootspl-$param_board_name"_freq"$param_ddr_freq.bin $BOOTSPL_DIR/bin/bootspl-nu4100"_freq"$param_ddr_freq.bin
fi

cd $WORKING_DIR

if [ "$zebu" == 1 ]
then
    python3 pysplit.py -bin $BOOTSPL_DIR/bin/bootspl-nu4100.bin -lram 1 -boot 0x2030000
fi
