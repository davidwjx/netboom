cmake_minimum_required(VERSION 3.6)
project(SandBoxFW)
set(prj_root ../../../../../../..)
set(CMAKE_INSTALL_PREFIX .)
set(CMAKE_CXX_FLAGS "-fPIC")
set(CMAKE_CXX_STANDARD 11)
if("Android" MATCHES ${PLATFORM_ID})
	add_definitions(-D_REENTRANT -DNO_ALGO -DANDROID_HOST)
elseif("armv8" MATCHES ${CMAKE_SYSTEM_PROCESSOR})
	add_definitions(-D_REENTRANT -DNO_ALGO -D__aarch64__ -DLINUX_GP_HOST -DDEFSG_IS_UART_MIPI -D_GNU_SOURCE -DLINUX_MAKE_BUILD -Wno-format-security -MD -MP -Wno-unused-parameter -Wall)
endif()
include_directories(. 
		${prj_root}/FW/nu4100/app/api/common
		${prj_root}/FW/nu4100/app/api/common/graph
		${prj_root}/FW/common/include
        #$ENV{BOOST_PATH}
		${prj_root}/FW/nu4100/app/common/infrastructure/metadata/nanopb
        )
set(SOURCE_FILES SandBoxLib.cpp)

#[[set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_STATIC_LIBS TRUE)
set(Boost_INCLUDE_DIR $ENV{BOOST_PATH})
set(Boost_LIBRARY_DIR "$ENV{BOOST_PATH}/${CMAKE_PLATFORM_NAME}/lib")
find_package(Boost 1.67 EXACT REQUIRED  system filesystem thread atomic chrono )
]]

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

link_directories(${prj_root}/bin/${CMAKE_PLATFORM_NAME})
target_link_libraries(${PROJECT_NAME} inu_host_lib_4100)
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION .)
