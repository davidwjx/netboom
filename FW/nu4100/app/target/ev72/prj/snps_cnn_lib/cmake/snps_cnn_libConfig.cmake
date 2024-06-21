# Normally, this file is used to find the dependencies of this library when
# find_package is used; find_dependency will then call the next Config.cmake
# file recursively, such that the CMake definitions of all dependencies are
# imported.
#
# In 2018.09 this is not yet required for cnn_tools-examples-based applications,
# because cnn_tools doesn't use CMake packages yet, and lists all dependencies
# exhaustively as a list of library file names.
#
include(CMakeFindDependencyMacro)
# Add find_dependency() calls for all find_package() calls used for building
# this target, for example: find_dependency(openvx 1.0)
#
find_dependency(evcnndev        1.0)
find_dependency(evlayers        1.0)

if(DEFINED USE_OPENCV)
    find_dependency(cnnimageopencv  1.0)
else()
    find_dependency(cnnimagebmetal  1.0)
endif()

if(DEFINED EVSS_CFG_HAS_OCL)
    find_dependency(ocl_kernels     1.0)
endif()

if(DEFINED USE_OPENVX)
    find_dependency(evcnnovx        1.0)
endif()

if(DEFINED ARC)
    find_dependency(evcnnhwutil     1.0)
endif()
# Import libTargets
include("${CMAKE_CURRENT_LIST_DIR}/snps_cnn_libTargets.cmake")
