#!/bin/bash
set -e
set -x

SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
KERNEL_DIR=$SCRIPT_DIR/../kernel
ISP_KERNEL=$SCRIPT_DIR/../3rd_party/vsi/vvcam/native/


# load environmet for production build
if [ "$USER" = "cmadmin" ]; then
	source /home/cmadmin/.profile 
fi
# Usage:
function func_usage
{
	echo "Usage of $0:"
	echo -e "\t-h : usage help"
	echo -e "\t-c (--clean) : clean build"
	echo -e "\t-v (--version) : version branch"
	echo -e "\t-p (--platform) : build platform ("boot0", ..., "bootall")"
	echo -e "\t-r (--revision) : build chip revision ("nu3000","nu4000","nu4000b0","nu4000c0","nu4100")"
	echo -e "\t-o (--outdir) : out directory for the target build process"
	echo -e "\t-a (--app) : build only app"
	echo -e "\t-b (--bootspl) : build only bootspl"
	echo -e "\t-t (--boottpp) : build only client tpp"
	echo -e "\t-d (--adapter) : build for Helsinki adapter hardware"
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
		set +x
		;;
		-c|--clean)
		param_clean=-c
		;;
		-a|--app)
		param_app=1
		;;
		-b|--bootspl)
		param_bootspl=1
		;;
		-t|--boottpp)
		param_client=1
		;;
		-d|--adapterhw)
		param_adapterhw=1	
		;;
		-p|--platform)
		param_platform="$1"
		shift
		;;
		-v|--version)
		param_version="$1"
		shift
		;;
		-o|--outdir)
		param_outdir="$1"
		shift
		;;
		-r|--revision)
		param_chip_revision="$1"
		shift
		;;
		*)
				# unknown option
		;;
	esac
	done
}

# $1 - directory path
function func_check_for_files
{
	if [ -d "$1" ]; then
		if [ "$(ls -A "$1")" ]; then
			echo "0"
		fi
	fi
}

# $1 - platform
function func_build_kernel
{
	pushd . > /dev/null
	cd $KERNEL_DIR/build
	chmod +x generate_kernel.sh
	TMP_KERNEL_DIR=`mktemp -d`

	# setup build path for xn02 server
	if [ "$USER" == "cmadmin" ]; then
		LINUX_BUILD_DIR=$SCRIPT_DIR/../../../FW_GIT/
	else 
		LINUX_BUILD_DIR=$HOME/inu_kernel_git_repo
	fi

	if [ ! -z "$param_version" ]; then
		curr_version=Versions/$param_version
	else
		curr_version=master
	fi

	if [ "$param_chip_revision" == "nu3000" ]; then
		LINUX_GIT_REPO=git@git.il.inuitive-tech.com:sw/kernel.git
	else 
		LINUX_GIT_REPO=git@git.il.inuitive-tech.com:sw/kernel-v4.14.git
	fi

	# allow co-existense of mulitple repositories
	GIT_REPO_FOLDER=`echo $LINUX_GIT_REPO | cut -d'/' -f2`
	LINUX_BUILD_DIR=$LINUX_BUILD_DIR/$GIT_REPO_FOLDER
	bash -x -e ./generate_kernel.sh -b $LINUX_BUILD_DIR  -v $curr_version	-g $LINUX_GIT_REPO $param_clean
	cd $LINUX_BUILD_DIR/$curr_version
	PLATFORM=$1
	if [ "$1" == "bootall" ]; then
		bash -x -e ./build_kernel.sh -o $TMP_KERNEL_DIR -p all -d -t $param_chip_revision
		mkdir -p $BIN_DIR/target/$1/boot0/kernel
		for PLATFORM_DTB in `find "$TMP_KERNEL_DIR" -type f -name "$param_chip_revision-boot*" -exec basename {} \;` 
		do
			PLATFORM=`echo $PLATFORM_DTB | cut -d "." -f1 | cut -d "-" -f2`
			mkdir -p $BIN_DIR/target/$1/$PLATFORM/kernel
			cp -av $TMP_KERNEL_DIR/$PLATFORM_DTB $BIN_DIR/target/$1/$PLATFORM/kernel
			echo $PLATFORM >> $BIN_DIR/target/$1/boot0/kernel/platforms_supported.txt
		done
		cp -av $TMP_KERNEL_DIR/zImage $BIN_DIR/target/$1/boot0/kernel
		if [ -e "$TMP_KERNEL_DIR/zImage_boot51" ]; then
			cp -av $TMP_KERNEL_DIR/zImage_boot51 $BIN_DIR/target/$1/boot0/kernel
		fi
		if [ "$param_chip_revision" == "nu4000c0" ] || [ "$param_chip_revision" = "nu4100" ]; then
			make clean; make distclean
			bash -x -e ./build_kernel.sh -o $TMP_KERNEL_DIR -t $param_chip_revision -s cdc
			cp -av $TMP_KERNEL_DIR/zImage $BIN_DIR/target/$1/boot0/kernel/zImage_cdc
		fi
	else
		mkdir -p $BIN_DIR/target/$PLATFORM/kernel
		bash -x -e ./build_kernel.sh -o $BIN_DIR/target/$PLATFORM/kernel -p $PLATFORM -d -t $param_chip_revision
	fi
	popd > /dev/null
}

function func_build_rswifi
{
	if [ "$param_chip_revision" == "nu4000b0" ]; then

		pushd . > /dev/null
		cd $KERNEL_DIR/build
		chmod +x generate_rswifi.sh
		TMP_KERNEL_DIR=`mktemp -d`
		
		# setup build path for xn02 server
		if [ "$USER" == "cmadmin" ]; then
			LINUX_BUILD_DIR=$SCRIPT_DIR/../../../FW_GIT/
		else 
			LINUX_BUILD_DIR=$HOME/inu_kernel_git_repo
		fi
		
		if [ ! -z "$param_version" ]; then
			curr_version=Versions/$param_version
		else
			curr_version=master
		fi
		
		WIFI_GIT_REPO=git@git.il.inuitive-tech.com:sw/RS9116_WIFI.git
		KERNEL_GIT_REPO=git@git.il.inuitive-tech.com:sw/kernel-v4.14.git
		
		# allow co-existense of mulitple repositories
		GIT_REPO_FOLDER=`echo $WIFI_GIT_REPO | cut -d'/' -f2`
		KERNEL_REPO_FOLDER=`echo $KERNEL_GIT_REPO | cut -d'/' -f2`
		WIFI_BUILD_DIR=$LINUX_BUILD_DIR/$GIT_REPO_FOLDER
			KERNEL_BUILD_DIR=$LINUX_BUILD_DIR/$KERNEL_REPO_FOLDER
		bash -x -e ./generate_rswifi.sh -b $WIFI_BUILD_DIR	-v $curr_version	-g $WIFI_GIT_REPO $param_clean
		cd $WIFI_BUILD_DIR/$curr_version/source/host/
		PLATFORM=$1
		KERNELDIR=$KERNEL_BUILD_DIR/$curr_version make -j20
		popd > /dev/null
	fi
}

# $1 - platform
function func_build_isp_kernel
{
	if [ "$param_chip_revision" == "nu4100" ]; then
		pushd . > /dev/null

		export ispKernelPath=$LINUX_BUILD_DIR/$curr_version
		cd $ISP_KERNEL
		make clean; make -j20
		#cp -av $TMP_KERNEL_DIR/$PLATFORM_DTB $BIN_DIR/target/bootall/$1/kernel

		popd > /dev/null
	fi
}

# $1 - platform
function func_build_bootspl
{
	BOOTSPL_DIR=$SCRIPT_DIR/../$param_chip_revision/bootspl

    pushd . > /dev/null
	cd $BOOTSPL_DIR/build

	chmod +x build.sh
	chmod +x $BOOTSPL_DIR/src/build/common.mk
	chmod +x $BOOTSPL_DIR/src/build/rules.mk
	if [ "$param_chip_revision" = "nu4000b0" ] || [ "$param_chip_revision" = "nu4000c0" ] || [ "$param_chip_revision" = "nu4100" ]; then
		DDR_FREQ=("533" "800" "1000" "1200" "1400")
		else
		DDR_FREQ=("0" "1")
	fi

	if [ "$1" == "bootall" ]; then
		for PLATFORM in `find "$BOOTSPL_DIR/src/ddr/" -type d -name "boot*" -exec basename {} \;` 
		do
           for ddrFreq in ${DDR_FREQ[@]}  
           do
           
                 [ ! -z $param_clean ] && rm -rf $BOOTSPL_DIR/bin/*

                if [ "$param_chip_revision" = "nu4000c0" ]; then
			bash -x ./build.sh -s -b c0 -p $PLATFORM -f $ddrFreq
			bash -x ./build.sh -s -b c1 -p $PLATFORM -f $ddrFreq
                else
			bash -x ./build.sh -s -p $PLATFORM -f $ddrFreq
                fi
				
                if [ $(func_check_for_files $BOOTSPL_DIR/bin) ]; then
					mkdir -p $BIN_DIR/target/$1/$PLATFORM/bootspl
                 	cp -av  $BOOTSPL_DIR/bin/* $BIN_DIR/target/$1/$PLATFORM/bootspl/
                fi
                 
                 echo "$ddrFreq"  
           done

		done
	else
        for ddrFreq in ${DDR_FREQ[@]}   
        do
             [ ! -z $param_clean ] && rm -rf $BOOTSPL_DIR/bin/*
             bash -x ./build.sh -s -b c0 -p $1 -f $ddrFreq
             bash -x ./build.sh -s -b c1 -p $1 -f $ddrFreq
             if [ $(func_check_for_files $BOOTSPL_DIR/bin) ]; then
             		mkdir -p $BIN_DIR/target/$1/bootspl
             		cp -av  $BOOTSPL_DIR/bin/* $BIN_DIR/target/$1/bootspl/
             fi
             echo "$ddrFreq"  
        done
	fi

	popd > /dev/null
}

function func_build_bootfix
{
	BOOTFIX_DIR=$SCRIPT_DIR/../$param_chip_revision/bootfix

    pushd . > /dev/null
	cd $BOOTFIX_DIR/build

	chmod +x build.sh
	chmod +x $BOOTFIX_DIR/src/build/common.mk

	if [ "$1" == "bootall" ]; then
		for PLATFORM in `find "$BOOTSPL_DIR/src/ddr/" -type d -name "boot*" -exec basename {} \;` 
		do
			[ ! -z $param_clean ] && rm -rf $BOOTFIX_DIR/bin/*
			bash -x ./build.sh -s -q -p $PLATFORM
			if [ $(func_check_for_files $BOOTFIX_DIR/bin) ]; then
					mkdir -p $BIN_DIR/target/$1/$PLATFORM/bootfix
					cp -av  $BOOTFIX_DIR/bin/* $BIN_DIR/target/$1/$PLATFORMbootfix/
			fi
		done
	else
		[ ! -z $param_clean ] && rm -rf $BOOTFIX_DIR/bin/*
		bash -x ./build.sh -s -q -p $1
		if [ $(func_check_for_files $BOOTFIX_DIR/bin) ]; then
			mkdir -p $BIN_DIR/target/$1/bootfix
			cp -av  $BOOTFIX_DIR/bin/* $BIN_DIR/target/$1/bootfix/
		fi
	fi

	popd > /dev/null
}

function func_build_client
{
	CLIENT_DIR=$SCRIPT_DIR/../$param_chip_revision/client
	pushd . > /dev/null

	chmod +x $CLIENT_DIR/build/bulid.sh
	PLATFORM=$1
	if [ "$1" == "bootall" ]; then
		for PLATFORM in `find "$CLIENT_DIR/" -type d -name "boot*" -exec basename {} \;`
		do
			[ ! -z $param_clean ] && rm -rf $CLIENT_DIR/build/bin/$PLATFORM
			$CLIENT_DIR/build/bulid.sh -t gp -p $PLATFORM
			if [ $(func_check_for_files $CLIENT_DIR/build/bin/$PLATFORM/gp) ]; then
				mkdir -p $BIN_DIR/target/$1/$PLATFORM/client/gp
				cp -av $CLIENT_DIR/build/bin/$PLATFORM/gp/* $BIN_DIR/target/$1/$PLATFORM/client/gp
			fi

			if [ $(func_check_for_files "$CLIENT_DIR/$PLATFORM/ceva") ]; then
			$CLIENT_DIR/build/bulid.sh -t ceva -p $PLATFORM
			if [ $(func_check_for_files $CLIENT_DIR/build/bin/$PLATFORM/ceva) ]; then
				mkdir -p $BIN_DIR/target/$1/$PLATFORM/client/ceva
				cp -av $CLIENT_DIR/build/bin/$PLATFORM/ceva/* $BIN_DIR/target/$1/$PLATFORM/client/ceva
			fi
		fi
		done
	else
		[ ! -z $param_clean ] && rm -rf $CLIENT_DIR/build/bin/$PLATFORM
		if [ $(func_check_for_files "$CLIENT_DIR/$PLATFORM/gp") ]; then
			$CLIENT_DIR/build/bulid.sh -t gp -p $PLATFORM
			if [ $(func_check_for_files $CLIENT_DIR/build/bin/$PLATFORM/gp) ]; then
				mkdir -p $BIN_DIR/target/$PLATFORM/client/gp
				cp -av $CLIENT_DIR/build/bin/$PLATFORM/gp/* $BIN_DIR/target/$PLATFORM/client/gp
			fi
		fi
		if [ $(func_check_for_files "$CLIENT_DIR/$PLATFORM/ceva") ]; then
			$CLIENT_DIR/build/bulid.sh -t ceva -p $PLATFORM
			if [ $(func_check_for_files $CLIENT_DIR/build/bin/$PLATFORM/ceva) ]; then
				mkdir -p $BIN_DIR/target/$PLATFORM/client/ceva
				cp -av $CLIENT_DIR/build/bin/$PLATFORM/ceva/* $BIN_DIR/target/$PLATFORM/client/ceva
			fi
		fi
	fi

	popd > /dev/null
}

function func_build_client_demos
{
	CLIENT_DEMO_DIR=$SCRIPT_DIR/../$param_chip_revision/client_demo
	pushd . > /dev/null

	[ ! -z $param_clean ] && rm -rf "$TARGET_FDK_DIR"
	mkdir -p "$TARGET_FDK_DIR"
	chmod +x $CLIENT_DEMO_DIR/build/bulid.sh $CLIENT_DEMO_DIR/build/export.sh
	$CLIENT_DEMO_DIR/build/bulid.sh -d all
	$CLIENT_DEMO_DIR/build/export.sh -o "$TARGET_FDK_DIR"

	popd > /dev/null
}

function func_build_inu_target
{
	APP_DIR=$SCRIPT_DIR/../$param_chip_revision/app
	pushd . > /dev/null

	cd $APP_DIR/build/gp/
	chmod +x build.sh

	PLATFORM=$1
	if [ "$1" == "bootall" ]; then
		#build GP
		PLATFORM=boot0
		[ ! -z $param_clean ] && rm -rf $APP_DIR/build/gp/bin/$PLATFORM/*
		
		if [  -z "$param_adapterhw" ]; then
			echo "Building for S1 hardware"
			bash -x ./build.sh -p $PLATFORM $param_clean
		else
			echo "Building for adapter hardware"
			bash -x ./build.sh -p $PLATFORM $param_clean -a
		fi

		#copy GP file
		if [ -e "$APP_DIR/build/gp/bin/$PLATFORM/inu_target.out" ]; then
			mkdir -p $BIN_DIR/target/$1/$PLATFORM/gp
			cp -av  $APP_DIR/build/gp/bin/$PLATFORM/inu_target.* $BIN_DIR/target/$1/$PLATFORM/gp
		fi
	else
		[ ! -z $param_clean ] && rm -rf $APP_DIR/build/gp/bin/$PLATFORM/*
		#build GP
		if [  -z "$param_adapterhw" ]; then
			echo "Building for S1 hardware"
			bash -x ./build.sh -p $PLATFORM $param_clean
		else
			echo "Building for adapter hardware"
			bash -x ./build.sh -p $PLATFORM $param_clean -a
		fi

		#copy GP file
		mkdir -p $BIN_DIR/target/$PLATFORM/gp
		if [ -e "$APP_DIR/build/gp/bin/$PLATFORM/inu_target.out" ]; then
			cp -av  $APP_DIR/build/gp/bin/$PLATFORM/inu_target.* $BIN_DIR/target/$PLATFORM/gp
		else
			cp -av  $APP_DIR/build/gp/bin/boot0/inu_target.* $BIN_DIR/target/$PLATFORM/gp
		fi
	fi
	popd > /dev/null
}

function func_export_target
{
	APP_DIR=$SCRIPT_DIR/../$param_chip_revision/app
	pushd . > /dev/null

	cd $APP_DIR/build/gp
	chmod +x export.sh

	bash -x ./export.sh 

	PLATFORM=$1
	mkdir -p $BIN_DIR/target/$PLATFORM/export
	if [ -e "$APP_DIR/build/gp/export.tar.gz" ]; then
		cp -av  $APP_DIR/build/gp/export.tar.gz $BIN_DIR/target/$PLATFORM/export
	fi
}

function func_build_inu_ceva
{
	APP_DIR=$SCRIPT_DIR/../$param_chip_revision/app
	pushd . > /dev/null

	cd $APP_DIR/build/ceva
	chmod +x build.sh

	PLATFORM=$1
	if [ "$1" == "bootall" ]; then
		#build CEVA
		[ ! -z $param_clean ] && rm -rf $APP_DIR/build/ceva/bin/boot/*
		bash -x ./build.sh -p boot $param_clean

		#copy CEVA file
		if [ -e "$APP_DIR/build/ceva/bin/boot/inu_app_A.cva" ] || [ -e "$APP_DIR/build/ceva/bin/boot/inu_app.cva" ]; then
			for PLATFORM in boot0 boot300
			do
				mkdir -p $BIN_DIR/target/$1/$PLATFORM/ceva
				cp -av  $APP_DIR/build/ceva/bin/boot/inu_app*.cva $BIN_DIR/target/$1/$PLATFORM/ceva
			done
		fi
	else
		#build CEVA
		[ ! -z $param_clean ] && rm -rf $APP_DIR/build/ceva/bin/$PLATFORM/*
		bash -x ./build.sh -p $PLATFORM $param_clean

		#copy CEVA file
		mkdir -p $BIN_DIR/target/$PLATFORM/ceva
		if [ -e "$APP_DIR/build/ceva/bin/$PLATFORM/inu_app_A.cva" ] || [ -e "$APP_DIR/build/ceva/bin/$PLATFORM/inu_app.cva" ]; then
			cp -av  $APP_DIR/build/ceva/bin/$PLATFORM/inu_app*.cva $BIN_DIR/target/$PLATFORM/ceva
		else
			cp -av  $APP_DIR/build/ceva/bin/boot0/inu_app*.cva $BIN_DIR/target/$PLATFORM/gp
		fi
	fi
	popd > /dev/null
}


function func_build_targetfs
{
	TARGETFS_DIR=$SCRIPT_DIR/../$param_chip_revision/targetfs
    pushd . > /dev/null

	PLATFORM=$1
	if [ "$1" == "bootall" ]; then
		for PLATFORM in `find "$TARGETFS_DIR/src" -type d -name "boot*" -exec basename {} \;`
		do
			mkdir -p $BIN_DIR/target/$1/$PLATFORM/targetfs
			# copy targetfs to the bin folder
			cp -av  $TARGETFS_DIR/src/$PLATFORM/* $BIN_DIR/target/$1/$PLATFORM/targetfs
		done
	else
			mkdir -p $BIN_DIR/target/$PLATFORM/targetfs
			if [ $(func_check_for_files "$TARGETFS_DIR/src/$PLATFORM") ]; then
				# copy targetfs to the bin folder
				cp -av  $TARGETFS_DIR/src/$PLATFORM/* $BIN_DIR/target/$PLATFORM/targetfs
			else
				cp -av  $TARGETFS_DIR/src/boot0/* $BIN_DIR/target/$PLATFORM/targetfs
			fi
	fi

	popd > /dev/null
}

function func_pack_zip
{
    pushd . > /dev/null

	TMP_BUILD_DIR=`mktemp -d`

	PLATFORM=$1
	if [ "$1" == "bootall" ]; then
		for PLATFORM in `cat $BIN_DIR/target/$1/boot0/kernel/platforms_supported.txt`
		do
			rm -rf $TMP_BUILD_DIR/*

			# copy inu_target to the bin folder
			if [ $(func_check_for_files "$BIN_DIR/target/$1/$PLATFORM/gp/") ]; then
				cp -av  $BIN_DIR/target/$1/$PLATFORM/gp/inu_target.out $TMP_BUILD_DIR
			else
				cp -av  $BIN_DIR/target/$1/boot0/gp/inu_target.out $TMP_BUILD_DIR
			fi

			# copy general inu_app.cva to the bin folder
			if [ $(func_check_for_files "$BIN_DIR/target/$1/$PLATFORM/ceva/") ]; then
				cp -av  $BIN_DIR/target/$1/$PLATFORM/ceva/inu_app*.cva $TMP_BUILD_DIR
			else
				cp -av  $BIN_DIR/target/$1/boot0/ceva/inu_app*.cva $TMP_BUILD_DIR
			fi

			# copy targetfs to the bin folder
			#if [ $(func_check_for_files "$BIN_DIR/target/$1/$PLATFORM/targetfs/") ]; then
			#	cp -av  $BIN_DIR/target/$1/$PLATFORM/targetfs/* $TMP_BUILD_DIR
			#else
			#	cp -av  $BIN_DIR/target/$1/boot0/targetfs/* $TMP_BUILD_DIR
			#fi

			# copy targetfs to the bin folder
			if [ $(func_check_for_files "$KERNEL_DIR/initramfs/packing/scripts/$PLATFORM/media/inuitive/") ]; then
				cp -av  $KERNEL_DIR/initramfs/packing/scripts/$PLATFORM/media/inuitive/* $TMP_BUILD_DIR
			else
				cp -av  $KERNEL_DIR/initramfs/packing/scripts/boot0/media/inuitive/* $TMP_BUILD_DIR
			fi

			# copy client tpp
			if [ $(func_check_for_files "$BIN_DIR/target/$1/$PLATFORM/client/gp/") ]; then
				cp -av $BIN_DIR/target/$1/$PLATFORM/client/gp/inu_tpp.out $TMP_BUILD_DIR
			fi

			# copy client CEVA
			if [ $(func_check_for_files "$BIN_DIR/target/$1/$PLATFORM/client/ceva/") ]; then
				cp -av $BIN_DIR/target/$1/$PLATFORM/client/ceva/inu_app*.cva $TMP_BUILD_DIR
			fi

			if [ $(func_check_for_files "$TMP_BUILD_DIR") ]; then
				mkdir -p $BIN_DIR/target/$1/$PLATFORM/zip
				#zip nu3000.zip
				cd $TMP_BUILD_DIR
				zip -9 -r $BIN_DIR/target/$1/$PLATFORM/zip/$param_chip_revision.zip *
			fi
		done
	else

		# copy inu_target to the bin folder
		if [ $(func_check_for_files "$BIN_DIR/target/$PLATFORM/gp/") ]; then
			cp -av  $BIN_DIR/target/$PLATFORM/gp/inu_target.out $TMP_BUILD_DIR
		fi

		# copy generic inu_app to the bin folder
		if [ $(func_check_for_files "$BIN_DIR/target/$PLATFORM/ceva/") ]; then
			cp -av  $BIN_DIR/target/$PLATFORM/ceva/inu_app*.cva $TMP_BUILD_DIR
		fi

		# copy targetfs to the bin folder
		if [ $(func_check_for_files "$BIN_DIR/target/$PLATFORM/targetfs/") ]; then
			cp -av  $BIN_DIR/target/$PLATFORM/targetfs/* $TMP_BUILD_DIR
		fi

		# copy client tpp
		if [ $(func_check_for_files "$BIN_DIR/target/$PLATFORM/client/gp/") ]; then
			cp -av $BIN_DIR/target/$PLATFORM/client/gp/inu_tpp.out $TMP_BUILD_DIR
		fi

		# copy client CEVA
		if [ $(func_check_for_files "$BIN_DIR/target/$1/$PLATFORM/client/ceva/") ]; then
			cp -av $BIN_DIR/target/$PLATFORM/client/ceva/inu_app*.cva $TMP_BUILD_DIR
		fi

		if [ $(func_check_for_files "$TMP_BUILD_DIR") ]; then
			mkdir -p $BIN_DIR/target/$PLATFORM/zip
			#zip nu3000.zip
			cd $TMP_BUILD_DIR
			zip -9 -r $BIN_DIR/target/$PLATFORM/zip/$param_chip_revision.zip *
		fi
	fi

	popd > /dev/null
}

function func_pack_zip_no_ceva
{
    pushd . > /dev/null

	TMP_BUILD_DIR=`mktemp -d`

	PLATFORM=$1
	if [ "$1" == "bootall" ]; then
		for PLATFORM in `cat $BIN_DIR/target/$1/boot0/kernel/platforms_supported.txt`
		do
			rm -rf $TMP_BUILD_DIR/*

			# copy inu_target to the bin folder
			if [ $(func_check_for_files "$BIN_DIR/target/$1/$PLATFORM/gp/") ]; then
				cp -av  $BIN_DIR/target/$1/$PLATFORM/gp/inu_target.out $TMP_BUILD_DIR
			else
				cp -av  $BIN_DIR/target/$1/boot0/gp/inu_target.out $TMP_BUILD_DIR
			fi

			if [ "$param_chip_revision" == "nu4100" ]; then
				if [ $(func_check_for_files "$ISP_KERNEL/bin") ]; then
					cp -av  $ISP_KERNEL/bin/vvcam_isp.ko $TMP_BUILD_DIR
				fi
			fi

			# copy general inu_app.cva to the bin folder
			#if [ $(func_check_for_files "$BIN_DIR/target/$1/$PLATFORM/ceva/") ]; then
			#	cp -av  $BIN_DIR/target/$1/$PLATFORM/ceva/inu_app*.cva $TMP_BUILD_DIR
			#else
			#	cp -av  $BIN_DIR/target/$1/boot0/ceva/inu_app*.cva $TMP_BUILD_DIR
			#fi

			# copy targetfs to the bin folder
			#if [ $(func_check_for_files "$BIN_DIR/target/$1/$PLATFORM/targetfs/") ]; then
			#	cp -av  $BIN_DIR/target/$1/$PLATFORM/targetfs/* $TMP_BUILD_DIR
			#else
			#	cp -av  $BIN_DIR/target/$1/boot0/targetfs/* $TMP_BUILD_DIR
			#fi

			# copy targetfs to the bin folder
			if [ $(func_check_for_files "$KERNEL_DIR/initramfs/packing/scripts/$PLATFORM/media/inuitive/") ]; then
				cp -av  $KERNEL_DIR/initramfs/packing/scripts/$PLATFORM/media/inuitive/* $TMP_BUILD_DIR
			else
				cp -av  $KERNEL_DIR/initramfs/packing/scripts/boot0/media/inuitive/* $TMP_BUILD_DIR
			fi

			# copy client CEVA
			#if [ $(func_check_for_files "$BIN_DIR/target/$1/$PLATFORM/client/ceva/") ]; then
			#	cp -av $BIN_DIR/target/$1/$PLATFORM/client/ceva/inu_app*.cva $TMP_BUILD_DIR
			#fi

			if [ $(func_check_for_files "$TMP_BUILD_DIR") ]; then
				mkdir -p $BIN_DIR/target/$1/$PLATFORM/zip
				#zip nu3000.zip
				cd $TMP_BUILD_DIR
				zip -9 -r $BIN_DIR/target/$1/$PLATFORM/zip/$param_chip_revision.zip *
			fi
		done
	else

		# copy inu_target to the bin folder
		if [ $(func_check_for_files "$BIN_DIR/target/$PLATFORM/gp/") ]; then
			cp -av  $BIN_DIR/target/$PLATFORM/gp/inu_target.out $TMP_BUILD_DIR
		fi

			if [ "$param_chip_revision" == "nu4100" ]; then
				if [ $(func_check_for_files "$ISP_KERNEL/bin") ]; then
					cp -av  $ISP_KERNEL/bin/vvcam_isp.ko $TMP_BUILD_DIR
				fi
			fi

		# copy generic inu_app to the bin folder
		#if [ $(func_check_for_files "$BIN_DIR/target/$PLATFORM/ceva/") ]; then
		#	cp -av  $BIN_DIR/target/$PLATFORM/ceva/inu_app*.cva $TMP_BUILD_DIR
		#fi

		# copy targetfs to the bin folder
		if [ $(func_check_for_files "$BIN_DIR/target/$PLATFORM/targetfs/") ]; then
			cp -av  $BIN_DIR/target/$PLATFORM/targetfs/* $TMP_BUILD_DIR
		fi

		# copy client CEVA
		#if [ $(func_check_for_files "$BIN_DIR/target/$1/$PLATFORM/client/ceva/") ]; then
		#	cp -av $BIN_DIR/target/$PLATFORM/client/ceva/inu_app*.cva $TMP_BUILD_DIR
		#fi

		if [ $(func_check_for_files "$TMP_BUILD_DIR") ]; then
			mkdir -p $BIN_DIR/target/$PLATFORM/zip
			#zip nu3000.zip
			cd $TMP_BUILD_DIR
			zip -9 -r $BIN_DIR/target/$PLATFORM/zip/$param_chip_revision.zip *
		fi
	fi

	popd > /dev/null
}

function func_pack_initramfs
{
    pushd . > /dev/null

	TMP_BUILD_DIR=`mktemp -d`

	PLATFORM=$1
	if [ "$1" == "bootall" ]; then
		for PLATFORM in `cat $BIN_DIR/target/$1/boot0/kernel/platforms_supported.txt`
		do
			rm -rf $TMP_BUILD_DIR/*
			#copy initial files
			cp -r $KERNEL_DIR/initramfs/packing $TMP_BUILD_DIR/packing



			if [ "$param_chip_revision" == "nu4000b0" ]; then
				if [ "$PLATFORM" == "boot51" ]; then
					cp -av $WIFI_BUILD_DIR/$curr_version/source/host/release/*.ko $TMP_BUILD_DIR/packing/scripts/boot51/media/redpine
					cp -av $WIFI_BUILD_DIR/$curr_version/source/host/release/firmware $TMP_BUILD_DIR/packing/scripts/boot51/media/redpine
					cp -av $WIFI_BUILD_DIR/$curr_version/source/host/release/onebox_util $TMP_BUILD_DIR/packing/scripts/boot51/media/redpine
					cp -av $WIFI_BUILD_DIR/$curr_version/source/host/release/wpa_cli $TMP_BUILD_DIR/packing/scripts/boot51/media/redpine
					cp -av $WIFI_BUILD_DIR/$curr_version/source/host/release/wpa_supplicant $TMP_BUILD_DIR/packing/scripts/boot51/media/redpine
				fi
			fi

			chmod u=rwx $TMP_BUILD_DIR/packing/scripts/*

			# copy inu_target to the bin folder
			if [ $(func_check_for_files "$BIN_DIR/target/$1/$PLATFORM/gp/") ]; then
				cp -av  $BIN_DIR/target/$1/$PLATFORM/gp/inu_target.out $TMP_BUILD_DIR/packing/scripts/$PLATFORM/media/inuitive
			else
				cp -av  $BIN_DIR/target/$1/boot0/gp/inu_target.out $TMP_BUILD_DIR/packing/scripts/boot0/media/inuitive
			fi

			#if [ "$param_chip_revision" == "nu4100" && "$1" == "boot0" ]; then
			if [ "$param_chip_revision" == "nu4100" ]; then
				if [ $(func_check_for_files "$ISP_KERNEL/bin") ]; then
					cp -av  $ISP_KERNEL/bin/vvcam_isp.ko $TMP_BUILD_DIR/packing/scripts/boot0/media/inuitive
				fi
			fi

			if [ $(func_check_for_files "$TMP_BUILD_DIR") ]; then
				mkdir -p $BIN_DIR/target/$1/initramfs
				#call script to pack the files
				cd $TMP_BUILD_DIR/packing
				chmod +x build.sh
				bash -x ./build.sh -p $PLATFORM
				if [ $PLATFORM = "boot51" ]; then
				    cp initramfs.gz $BIN_DIR/target/$1/initramfs/initramfs_$PLATFORM.gz
				else 
				    cp initramfs.gz $BIN_DIR/target/$1/initramfs/
				fi
			fi
		done
	else
		rm -rf $TMP_BUILD_DIR/*

		#copy initial files
		cp -r $KERNEL_DIR/initramfs/packing $TMP_BUILD_DIR/packing

		chmod u=rwx $TMP_BUILD_DIR/packing/scripts/*

		# copy inu_target to the bin folder
		if [ $(func_check_for_files "$BIN_DIR/target/$PLATFORM/gp/") ]; then
			cp -av  $BIN_DIR/target/$PLATFORM/gp/inu_target.out $TMP_BUILD_DIR/packing/scripts/media/inuitive
		fi

		if [ "$param_chip_revision" == "nu4100" ]; then
			if [ $(func_check_for_files "$ISP_KERNEL/bin") ]; then
				cp -av  $ISP_KERNEL/bin/vvcam_isp.ko $TMP_BUILD_DIR/packing/scripts/$PLATFORM/media/inuitive
			fi
		fi

		if [ $(func_check_for_files "$TMP_BUILD_DIR") ]; then
			mkdir -p $BIN_DIR/target/$1/initramfs
			#call script to pack the files
			cd $TMP_BUILD_DIR/packing
			chmod +x build.sh
			bash -x ./build.sh -p $PLATFORM
			if [ $PLATFORM = "boot51" ]; then
				cp initramfs.gz $BIN_DIR/target/$1/initramfs/initramfs_$PLATFORM.gz
			else 
				cp initramfs.gz $BIN_DIR/target/$1/initramfs/
			fi
		fi
	fi

	popd > /dev/null
}

function func_rename_zip
{
	pushd . > /dev/null

	echo "Rename zip file"
	PLATFORM=$1
	if [ "$1" == "bootall" ]; then
		INU_DEFSG_BOOT_FACTOR=10000
		for PLATFORM in `cat $BIN_DIR/target/$1/boot0/kernel/platforms_supported.txt`
		do
			# Get major client number from PLATFORM name
			CLIENT_MAJOR=${PLATFORM#boot}
			CLIENT_MAJOR=$((CLIENT_MAJOR%INU_DEFSG_BOOT_FACTOR))
			if [ "${#CLIENT_MAJOR}" == "1" ] || [ "${#CLIENT_MAJOR}" == "2" ]; then
				CLIENT=boot0
			else 
				CLIENT=${CLIENT_MAJOR::${#CLIENT_MAJOR}-2}
				CLIENT=boot${CLIENT}00
			fi

			if [ -d "$BIN_DIR/target/$1/$PLATFORM/targetfs/" ] || [ -d "$BIN_DIR/target/$1/$PLATFORM/client/gp" ]; then
				if [ "$CLIENT" != "boot0" ];then
					if [ -e "$BIN_DIR/target/$1/boot/$CLIENT/$param_chip_revision-$PLATFORM.zip" ]; then
						mv $BIN_DIR/target/$1/boot/$CLIENT/$param_chip_revision-$PLATFORM.zip $BIN_DIR/target/$1/boot/$CLIENT/$param_chip_revision-$PLATFORM.zip_
					fi
				else
					if [ -e "$BIN_DIR/target/$1/boot/$CLIENT/$param_chip_revision.zip" ]; then
						mv $BIN_DIR/target/$1/boot/$CLIENT/$param_chip_revision.zip $BIN_DIR/target/$1/boot/$CLIENT/$param_chip_revision.zip_
					fi
				fi
			else
				if [ -e "$BIN_DIR/target/$1/boot/$CLIENT/$param_chip_revision.zip" ]; then
					mv $BIN_DIR/target/$1/boot/$CLIENT/$param_chip_revision.zip $BIN_DIR/target/$1/boot/$CLIENT/$param_chip_revision.zip_
				fi
			fi
		done
	else
		if [ -e "$BIN_DIR/target/$PLATFORM/boot/$param_chip_revision.zip" ]; then
			mv $BIN_DIR/target/$PLATFORM/boot/$param_chip_revision.zip $BIN_DIR/target/$PLATFORM/boot/$param_chip_revision.zip_
		fi
	fi

	popd > /dev/null
}

function func_create_boot_folder
{
    pushd . > /dev/null

	PLATFORM=$1
	if [ "$1" == "bootall" ]; then
		INU_DEFSG_BOOT_FACTOR=10000
		for PLATFORM in `cat $BIN_DIR/target/$1/boot0/kernel/platforms_supported.txt`
		do
			# Get major client number from PLATFORM name
			CLIENT_MAJOR=${PLATFORM#boot}
			CLIENT_MAJOR=$((CLIENT_MAJOR%INU_DEFSG_BOOT_FACTOR))
			if [ "${#CLIENT_MAJOR}" == "1" ] || [ "${#CLIENT_MAJOR}" == "2" ]; then
				CLIENT=boot0
			else 
				CLIENT=${CLIENT_MAJOR::${#CLIENT_MAJOR}-2}
				CLIENT=boot${CLIENT}00
			fi

			mkdir -p "$BIN_DIR/target/$1/boot/$CLIENT"

			#copy client generic kernel
			if [ -e "$BIN_DIR/target/$1/$CLIENT/kernel/zImage" ]; then
				cp -av $BIN_DIR/target/$1/$CLIENT/kernel/zImage $BIN_DIR/target/$1/boot/$CLIENT
			else
				cp -av $BIN_DIR/target/$1/boot0/kernel/zImage $BIN_DIR/target/$1/boot/$CLIENT
			fi

			if [ $PLATFORM = "boot51" ]; then
				if [ -e "$BIN_DIR/target/$1/$CLIENT/kernel/zImage_boot51" ]; then
					cp -av $BIN_DIR/target/$1/$CLIENT/kernel/zImage_boot51 $BIN_DIR/target/$1/boot/$CLIENT
				else
					if [ -e "$BIN_DIR/target/$1/boot0/kernel/zImage_boot51" ]; then
						cp -av $BIN_DIR/target/$1/boot0/kernel/zImage_boot51 $BIN_DIR/target/$1/boot/$CLIENT
					fi
				fi
			fi

			if [ -e "$BIN_DIR/target/$1/$CLIENT/kernel/zImage_cdc" ]; then
				cp -av $BIN_DIR/target/$1/$CLIENT/kernel/zImage_cdc $BIN_DIR/target/$1/boot/$CLIENT
			fi

			#copy dtbs
			if [ -d "$BIN_DIR/target/$1/$PLATFORM/kernel" ]; then
				cp -av $BIN_DIR/target/$1/$PLATFORM/kernel/*.dtb $BIN_DIR/target/$1/boot/$CLIENT
			fi

			#copy client generic bootspl
			if [ -d "$BIN_DIR/target/$1/$PLATFORM/bootspl" ]; then
				if [ "$PLATFORM" == "$CLIENT" ]; then
					cp -av $BIN_DIR/target/$1/$CLIENT/bootspl/* $BIN_DIR/target/$1/boot/$CLIENT
				else
					if [ "$param_chip_revision" == "nu4000" ]; then
						cp -av $BIN_DIR/target/$1/$PLATFORM/bootspl/bootspl-nu4000a0.bin $BIN_DIR/target/$1/boot/$CLIENT/bootspl-nu4000a0-$PLATFORM.bin
						else if [ "$param_chip_revision" == "nu3000" ]; then
							cp -av $BIN_DIR/target/$1/$PLATFORM/bootspl/bootspl-nu3000b0.bin $BIN_DIR/target/$1/boot/$CLIENT/bootspl-nu3000b0-$PLATFORM.bin
							cp -av $BIN_DIR/target/$1/$PLATFORM/bootspl/bootspl-nu3000b0_ddr200mhz.bin $BIN_DIR/target/$1/boot/$CLIENT/bootspl-nu3000b0_ddr200mhz-$PLATFORM.bin
							else if [ "$param_chip_revision" == "nu4000c0" ]; then
								cp -av $BIN_DIR/target/$1/$PLATFORM/bootspl/bootspl-nu4000c0.bin $BIN_DIR/target/$1/boot/$CLIENT/bootspl-nu4000c0-$PLATFORM.bin
								cp -av $BIN_DIR/target/$1/$PLATFORM/bootspl/bootspl-nu4000c1.bin $BIN_DIR/target/$1/boot/$CLIENT/bootspl-nu4000c1-$PLATFORM.bin
								else if [ "$param_chip_revision" == "nu4100" ]; then
									cp -av $BIN_DIR/target/$1/$PLATFORM/bootspl/bootspl-nu4100.bin $BIN_DIR/target/$1/boot/$CLIENT/bootspl-nu4100-$PLATFORM.bin
								else 
									cp -av $BIN_DIR/target/$1/$PLATFORM/bootspl/bootspl-$param_chip_revision.bin $BIN_DIR/target/$1/boot/$CLIENT/bootspl-$param_chip_revision-$PLATFORM.bin
								fi
							fi
						fi
					fi
				fi
			else
				if [ -d "$BIN_DIR/target/$1/$CLIENT/bootspl" ]; then
					cp -av $BIN_DIR/target/$1/$CLIENT/bootspl/* $BIN_DIR/target/$1/boot/$CLIENT
				else
					cp -av $BIN_DIR/target/$1/boot0/bootspl/* $BIN_DIR/target/$1/boot/$CLIENT
				fi
			fi

			#copy client generic zips
			if [ -d "$BIN_DIR/target/$1/$PLATFORM/targetfs/" ] || [ -d "$BIN_DIR/target/$1/$PLATFORM/client/gp" ]; then
				if [ "$CLIENT" != "boot0" ];then
					cp -av $BIN_DIR/target/$1/$PLATFORM/zip/$param_chip_revision.zip $BIN_DIR/target/$1/boot/$CLIENT/$param_chip_revision-$PLATFORM.zip
				else
					cp -av $BIN_DIR/target/$1/boot0/zip/*$param_chip_revision* $BIN_DIR/target/$1/boot/$CLIENT/$param_chip_revision.zip
				fi
			else
				cp -av $BIN_DIR/target/$1/boot0/zip/*$param_chip_revision* $BIN_DIR/target/$1/boot/$CLIENT/$param_chip_revision.zip
			fi

			#copy initramfs
			if [ $PLATFORM = "boot51" ]; then
				cp -av $BIN_DIR/target/$1/initramfs/initramfs_$PLATFORM.gz $BIN_DIR/target/$1/boot/$CLIENT/initramfs_$PLATFORM.gz
			else
			    if [ -e "$BIN_DIR/target/$1/initramfs/initramfs.gz" ]; then
			 	   cp -av $BIN_DIR/target/$1/initramfs/initramfs.gz $BIN_DIR/target/$1/boot/$CLIENT/initramfs.gz
			    fi
			fi


			#copy CEVA
			if [ -d "$BIN_DIR/target/$1/$PLATFORM/ceva" ]; then
				cp -av $BIN_DIR/target/$1/$PLATFORM/ceva/* $BIN_DIR/target/$1/boot/$CLIENT
			fi

		done
	else
			echo "Building boot directory"
			mkdir -p $BIN_DIR/target/$PLATFORM/boot


			if [ $PLATFORM = "boot51" ]; then
				cp -av $BIN_DIR/target/$PLATFORM/kernel/zImage_boot51 $BIN_DIR/target/$PLATFORM/boot
			else
			    cp -av  $BIN_DIR/target/$PLATFORM/kernel/zImage $BIN_DIR/target/$PLATFORM/boot
			fi
			# Rename custom dtb name to default (currently nu3000-m3)
			cp -av  $BIN_DIR/target/$PLATFORM/kernel/*.dtb $BIN_DIR/target/$PLATFORM/boot/$param_chip_revision.dtb

			cp -av $BIN_DIR/target/$PLATFORM/zip/$param_chip_revision.zip $BIN_DIR/target/$PLATFORM/boot/
			cp -av $BIN_DIR/target/$PLATFORM/bootspl/* $BIN_DIR/target/$PLATFORM/boot/

			#copy initramfs
			if [ -e "$BIN_DIR/target/$1/initramfs/initramfs.gz" ]; then
				cp -av $BIN_DIR/target/$PLATFORM/initramfs/initramfs.gz $BIN_DIR/target/$PLATFORM/boot/initramfs.gz
			fi

			#copy CEVA
			if [ -d "$BIN_DIR/target/$PLATFORM/ceva" ]; then
				cp -av $BIN_DIR/target/$1/$PLATFORM/ceva/* $BIN_DIR/target/$1/boot/
			fi
	fi

	popd > /dev/null
}

function func_copy_xml_to_bin_folders
{
	pushd . > /dev/null

	PLATFORM=$1
	if [ "$1" == "bootall" ]; then

		for PLATFORM in `find "$BIN_DIR/target/$1/boot/" -type d -name "boot*" -exec basename {} \;`
		do
			#echo $PLATFORM
			echo $SCRIPT_DIR/../$param_chip_revision/xmldb/$param_chip_revision'_soc_xml'/$PLATFORM
			DIR=$SCRIPT_DIR/../$param_chip_revision/xmldb/$param_chip_revision'_soc_xml'/$PLATFORM
			if [ -d "$DIR" ] 
			then
				echo "$PLATFORM exist"
				cp $DIR/* $BIN_DIR/target/$1/boot/$PLATFORM
			else
				echo "$PLATFORM not exist"
			fi

			#echo $PLATFORM
			echo $SCRIPT_DIR/../$param_chip_revision/xmldb/$param_chip_revision'_sw_xml'/$PLATFORM
			DIR=$SCRIPT_DIR/../$param_chip_revision/xmldb/$param_chip_revision'_sw_xml'/$PLATFORM
			if [ -d "$DIR" ] 
			then
				echo "$PLATFORM exist"
				cp $DIR/* $BIN_DIR/target/$1/boot/$PLATFORM
			else
				echo "$PLATFORM not exist"
			fi
		done
	else
		#echo $PLATFORM
		DIR=$SCRIPT_DIR/../$param_chip_revision/xmldb/$param_chip_revision'_soc_xml'/$PLATFORM
		if [ -d "$DIR" ] 
		then
			echo "$PLATFORM exist"
			cp $DIR/* $BIN_DIR/target/$PLATFORM/boot
		else
			echo "$PLATFORM not exist"
		fi

		#echo $PLATFORM
		DIR=$SCRIPT_DIR/../$param_chip_revision/xmldb/$param_chip_revision'_sw_xml'/$PLATFORM
		if [ -d "$DIR" ] 
		then
			echo "$PLATFORM exist"
			cp $DIR/* $BIN_DIR/target/$PLATFORM/boot
		else
			echo "$PLATFORM not exist"
		fi
	fi

	popd > /dev/null
}

function func_build_chip_rev
{
    pushd . > /dev/null

	if [ "$param_chip_revision" == "nu3000" ]; then
		func_build_kernel $1
		func_build_bootspl $1
		#func_build_bootfix $1
		func_build_inu_target $1
		func_build_inu_ceva $1
		func_build_targetfs $1
		func_pack_zip_no_ceva $1
		func_pack_initramfs $1
		func_create_boot_folder $1
		func_export_target $1
		func_build_client_demos $1
	elif [ "$param_chip_revision" == "nu4100" ]; then
		#func_build_kernel $1
		func_build_isp_kernel $1
		func_build_bootspl $1
		#func_build_bootfix $1
		func_build_inu_target $1
		#func_build_inu_ceva $1 
		func_build_targetfs $1
		func_pack_zip_no_ceva $1
		func_pack_initramfs $1
		func_create_boot_folder $1
		func_rename_zip $1
		#func_export_target $1
		func_build_client_demos $1
		func_copy_xml_to_bin_folders $1
	else
		func_build_kernel $1
		func_build_bootspl $1
		#func_build_bootfix $1
		func_build_inu_target $1
		func_build_inu_ceva $1
		func_build_targetfs $1
		func_pack_zip_no_ceva $1
		func_build_rswifi $1
		func_pack_initramfs $1
		func_create_boot_folder $1
		func_rename_zip $1
		#func_export_target $1
		func_build_client_demos $1
		func_copy_xml_to_bin_folders $1
	fi

	popd > /dev/null
}

func_parse_test_params "$@"

BIN_DIR=$SCRIPT_DIR/bin/$param_chip_revision
TARGET_FDK_DIR=$BIN_DIR/target/FDK

if [ ! -z "$param_help" ]; then
    func_usage
    exit 10
fi

[ -z "$param_platform" ] && param_platform="boot0"
[ -z "$param_chip_revision" ] && param_chip_revision="nu3000"

echo "Building target directory"
[ ! -z "$param_clean" ] && rm -rf $BIN_DIR/target/$param_platform
[ -z "$param_outdir" ] && param_outdir=$BIN_DIR
mkdir -p $BIN_DIR/target/$param_platform

if [ ! -z "$param_app" ]; then
	func_build_inu_target $param_platform
	func_build_targetfs $param_platform
	func_pack_zip_no_ceva $param_platform
else
	if [ ! -z "$param_bootspl" ]; then
		func_build_bootspl $param_platform
	else
		func_build_chip_rev $param_platform
	fi
fi
