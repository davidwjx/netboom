set EXIT_FLAG=0
rem ###################- Definitions -###################
set BRANCH_NAME=%1
set WORKSPACE=%2

set PROD_DIR=\\bkp01\space\users\cmadmin\Versions\Common\%BRANCH_NAME%
set CNN_DIR=\\bkp01\home\cmadmin\jenkins\workspace\sw\branches\Versions\%BRANCH_NAME%\FW
set opencv_dir_env=D:\NewWork\opencv451
set opencv_ver_env=451
set boost_path_env=D:\NewWork\boost_1_75_0
set devenv_env="C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\IDE\devenv.com"

set OPENCV_DIR=%opencv_dir_env%
set OPENCV_VER=%opencv_ver_env%
set BOOST_PATH=%boost_path_env%
set DEVENV=%devenv_env%

echo OPENCV_DIR - %OPENCV_DIR%
echo OPENCV_VER - %OPENCV_VER%
echo BOOST_PATH - %BOOST_PATH%


rem ###################- Build sanity_ev_multi -###################

%DEVENV% /clean "Release|x64" "%WORKSPACE%\AIF\WINDOWS\sanity_ev_multi\sanity_ev_multi.sln"
if not %errorlevel%==0 set EXIT_FLAG=1

del "%WORKSPACE%\build.log"

%DEVENV% /build "Release|x64" "%WORKSPACE%\AIF\WINDOWS\sanity_ev_multi\sanity_ev_multi.sln"
if not %errorlevel%==0 set EXIT_FLAG=1

if not exist %PROD_DIR%\x64 mkdir %PROD_DIR%\x64


rem ###################- Copy sanity_ev_multi -###################

xcopy /E /Y /i AIF\WINDOWS\sanity_ev_multi\googlenet_places\*.* %PROD_DIR%\x64\cnn_sanity\googlenet_places
xcopy /E /Y /i AIF\WINDOWS\sanity_ev_multi\googlenet_places\*.* cnnUploadToConan\cnn_sanity\googlenet_places

xcopy /E /Y /i AIF\WINDOWS\sanity_ev_multi\mobilenet_v2\*.* %PROD_DIR%\x64\cnn_sanity\mobilenet_v2
xcopy /E /Y /i AIF\WINDOWS\sanity_ev_multi\mobilenet_v2\*.* cnnUploadToConan\cnn_sanity\mobilenet_v2

xcopy /E /Y /i AIF\WINDOWS\sanity_ev_multi\resnet_50\*.* %PROD_DIR%\x64\cnn_sanity\resnet_50
xcopy /E /Y /i AIF\WINDOWS\sanity_ev_multi\resnet_50\*.* cnnUploadToConan\cnn_sanity\resnet_50

xcopy /E /Y /i AIF\WINDOWS\sanity_ev_multi\test_images\*.* %PROD_DIR%\x64\cnn_sanity\test_images
xcopy /E /Y /i AIF\WINDOWS\sanity_ev_multi\test_images\*.* cnnUploadToConan\cnn_sanity\test_images


xcopy /E /Y /i AIF\WINDOWS\sanity_ev_multi\x64\Release\sanity_ev_multi.exe %PROD_DIR%\x64\cnn_sanity
xcopy /E /Y /i AIF\WINDOWS\sanity_ev_multi\x64\Release\sanity_ev_multi.exe cnnUploadToConan\cnn_sanity

xcopy /E /Y /i AIF\WINDOWS\sanity_ev_multi\sw_graph_example.xml %PROD_DIR%\x64\cnn_sanity
xcopy /E /Y /i AIF\WINDOWS\sanity_ev_multi\sw_graph_example.xml cnnUploadToConan\cnn_sanity

xcopy %CNN_DIR%\nu4000b0\app\target\ev61\graphs\%SYNOPSYS_EVP_VER%\%CNN_GRAPH%\generated_obj\nu4000b0_release_unmerged_large\nn_bin_googlenet.bin %PROD_DIR%\x64\cnn_sanity\cnn_bins
xcopy %CNN_DIR%\nu4000b0\app\target\ev61\graphs\%SYNOPSYS_EVP_VER%\%CNN_GRAPH%\generated_obj\nu4000b0_release_unmerged_large\nn_bin_googlenet.bin cnnUploadToConan\cnn_sanity\cnn_bins

xcopy %CNN_DIR%\nu4000b0\app\target\ev61\graphs\%SYNOPSYS_EVP_VER%\%CNN_GRAPH%\generated_obj\nu4000b0_release_unmerged_large\nn_bin_mobilenet.bin %PROD_DIR%\x64\cnn_sanity\cnn_bins
xcopy %CNN_DIR%\nu4000b0\app\target\ev61\graphs\%SYNOPSYS_EVP_VER%\%CNN_GRAPH%\generated_obj\nu4000b0_release_unmerged_large\nn_bin_mobilenet.bin cnnUploadToConan\cnn_sanity\cnn_bins

xcopy %CNN_DIR%\nu4000b0\app\target\ev61\graphs\%SYNOPSYS_EVP_VER%\%CNN_GRAPH%\generated_obj\nu4000b0_release_unmerged_large\nn_bin_resnet_50.bin %PROD_DIR%\x64\cnn_sanity\cnn_bins
xcopy %CNN_DIR%\nu4000b0\app\target\ev61\graphs\%SYNOPSYS_EVP_VER%\%CNN_GRAPH%\generated_obj\nu4000b0_release_unmerged_large\nn_bin_resnet_50.bin cnnUploadToConan\cnn_sanity\cnn_bins


rem ###################- Build example_ev_multi -###################

%DEVENV% /clean "Release|x64" "%WORKSPACE%\AIF\WINDOWS\VS_2019\example_ev_multi\example_ev_multi.sln"
if not %errorlevel%==0 set EXIT_FLAG=1

del "%WORKSPACE%\build.log"   

%DEVENV% /build "Release|x64" "%WORKSPACE%\AIF\WINDOWS\VS_2019\example_ev_multi\example_ev_multi.sln"
if not %errorlevel%==0 set EXIT_FLAG=1

xcopy /E /Y /i AIF\WINDOWS\VS_2019\example_ev_multi\x64\Release\example_ev_multi.exe cnnUploadToConan\example_ev_multi


rem ###################- Build example_ev_ssd -###################

%DEVENV% /clean "Release|x64" "%WORKSPACE%\AIF\WINDOWS\VS_2019\example_ev_ssd\example_ev_ssd.sln"
if not %errorlevel%==0 set EXIT_FLAG=1

del "%WORKSPACE%\build.log"

%DEVENV% /build "Release|x64" "%WORKSPACE%\AIF\WINDOWS\VS_2019\example_ev_ssd\example_ev_ssd.sln"
if not %errorlevel%==0 set EXIT_FLAG=1

xcopy /E /Y /i AIF\WINDOWS\VS_2019\example_ev_ssd\x64\Release\example_ev_ssd.exe cnnUploadToConan\example_ev_ssd

exit %EXIT_FLAG%

