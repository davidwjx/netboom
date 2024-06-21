#!/bin/bash
set -e
set -x
SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
APP_DIR=$SCRIPT_DIR/../../app
CLIENT_DEMO_DIR=$SCRIPT_DIR/../../client_demo


#Usage:
function func_usage
{
	echo "Usage of $0:"
	echo -e "\t-h : usage help"
	echo -e "\t-c (--clean) : clean build"
	echo -e "\t-v (--version) : version branch"
	echo -e "\t-o (--outdir) : build platform (0.0.0, ...)"	
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
		-o|--outdir)
		param_outdir="$1"
		shift
		;;
		*)
				# unknown option
		;;
	esac
	done
}

function func_export
{
    pushd . > /dev/null
	
	[ ! -z "$param_clean" ] && rm -rf "$param_outdir"
	mkdir -p "$param_outdir"
	mkdir -p "$param_outdir/bin"
	mkdir -p "$param_outdir/src/app/api"

	cp -av $APP_DIR/api	 $param_outdir/src/app
	cp -av $CLIENT_DEMO_DIR/ $param_outdir/src
	#rm -r $param_outdir/src/client_demo/cdnn_demo
	# rm -r $param_outdir/src/client_demo/basic_demo/ev/prj/build_nu4000b0_release
	# rm -r $param_outdir/src/client_demo/cnn_demo/ev/prj/build_nu4000b0_release
	# rm -r $param_outdir/src/client_demo/cnn_client_demo/ev/prj/build_nu4000b0_release
	
        if [ -e "$param_outdir/docs" ]; then
        rm -fr $param_outdir/docs
        fi
        mv $param_outdir/src/client_demo/docs $param_outdir/.
	cp -av $CLIENT_DEMO_DIR/build/bin $param_outdir
	
	[ -d "$param_outdir/src/client_demo/build/bin" ] && rm -rf $param_outdir/src/client_demo/build/bin	
	find $param_outdir/src -name "*.o*" -type f -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src -name '"inu_app_lasp.tmp"' -type f -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src -name "*.A*" -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src/client_demo/basic_demo -name "*.a*" -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src/client_demo/matcher_demo -name "*.a*" -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src -name "*.B*" -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src -name "*.bfs*" -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src -name "*.bin*" -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src -name "*.inl" -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src -name "*.cva*" -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src -name "*.dis*" -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src -name "*.map*" -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src -name "*.bfs*" -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src -name "*.o*" -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src -name "*.i" -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src -name "*.d" -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src -name "*.dbg" -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src -name "*.lin" -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src -name "*.lst" -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src -name "*ceva.s*" -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src -name "*.switch*" -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src -name "cva_log.txt" -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src/app/api/host -name "inu.h" -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/src/app/api/host -name "inu_profiler.h" -print0 | xargs -0 /bin/rm -rf
	find $param_outdir/docs -name "*.docx" -print0 | xargs -0 /bin/rm -rf

	popd > /dev/null

}
func_parse_test_params "$@"



if [ ! -z "$param_help" ]; then
    func_usage
    exit 10
fi

if [ -z "$param_outdir" ]; then
    param_outdir=$SCRIPT_DIR/../../client_demo_export
fi

func_export


