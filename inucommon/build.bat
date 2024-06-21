set root_dir=%~dp0
set "root_dir=%root_dir:\=/%"
echo "root dir: " %root_dir%
::set THIRD_PARTY_ROOT=%root_dir%../3rdParty
::set BOOST_PATH=D:/works/boost_1_75_0
::set OPENCV_VER=451
::set OPENCV_DIR=D:/works/opencv-4.5.1
::cmake -G "Visual Studio 17 2022" -S ./src -DBUILD_LOCAL=True -DUSE_CONAN=FALSE -DCMAKE_BUILD_TYPE=Release -B src/out/build/windows
cmake . -G "Visual Studio 17 2022" -DOPEN_SOURCE=TRUE -DUSE_CONAN=FALSE -DCMAKE_BUILD_TYPE=Release -DTHIRD_PARTY_ROOT=D:\works\3rdparty -DBOOST_ROOT=D:\works\boost_1_75_0  -DOPENCV_ROOT=D:\works\opencv-4.5.1 -DOPENCV_VER=4.5.1 -Bout/build/Release_x64
cd out/build/Release_x64
cmake --build . --config release 

