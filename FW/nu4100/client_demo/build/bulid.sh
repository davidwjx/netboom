#!/bin/bash
set -e
set -x
BUILD_DIR="$(dirname "$(readlink -f "$0")")"
APP_DIR=$BUILD_DIR/../../app
CLIENT_DEMOS_DIR=$BUILD_DIR/..


#Usage:
function func_usage
{
	echo "Usage of $0:"
	echo -e "\t-h : usage help"
	echo -e "\t-c (--clean) : clean build"
	echo -e "\t-v (--version) : version branch"
	echo -e "\t-d (--demo_type) : build platform (basic_demo)"	
	echo -e "\t-p (--platform) : build platform (0.0.0, ...)"	
	echo -e "\texample: ./bulid.sh -t gp -p 0.0.0"	
}
function func_parse_test_params
{
	while [[ $# > 0 ]]
	do
	key=$1
	shift

	case $key in		
		-h|--help)
		param_help=1
		;;
		-c|--clean)
		param_clean=1		
		;;
		-v|--version)
		param_version="$1"
		shift
		;;
		-p|--platform)
		param_platform="$1"
		shift
		;;
		-d|--demo_type)
		demo_type="$1"
		shift
		;;
		*)
				# unknown option
		;;
	esac
	done
}

function func_build_gp
{	
	pushd . > /dev/null
	cd $CLIENT_DEMOS_DIR/$1/gp

	if [ -z "$param_clean" ];then  
		make
	else
		make rebuild
	fi

	mkdir -p $BUILD_DIR/bin/$1/gp				
	cp -av  inu_target.out $BUILD_DIR/bin/$1/gp
	
	popd > /dev/null
}

function func_copy_libs
{
	pushd . > /dev/null
	
	if [ -e "$APP_DIR/target/gp/prj/make/libinu_target.a" ]; then
		cp -av $APP_DIR/target/gp/prj/make/libinu_target.a $BUILD_DIR/../libs/gp	
	fi
	
	popd > /dev/null
}

func_parse_test_params "$@"



if [ ! -z "$param_help" ]; then
    func_usage
    exit 10
fi

	func_copy_libs
	
if [ "$demo_type" == "all" ]; then
	echo "build all demos"
	func_build_gp   i2c_demo
	#func_build_gp   i2s_demo
	func_build_gp   imu_demo
	func_build_gp   stand_alone_demo
	func_build_gp   uart_demo
	func_build_gp   basic_demo
	#func_build_gp	sensors_api_demo
	func_build_gp	histogram_demo
	func_build_gp	cnn_demo
	func_build_gp   cnn_client_demo
	func_build_gp   stereo_demo
	func_build_gp	sansa_api_demo
	func_build_gp	sansa_api_demo_tcp_less
else
	func_build_gp $demo_type
fi
