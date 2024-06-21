cmake_minimum_required(VERSION 3.6)
project(SandboxFW)
set(CMAKE_CXX_STANDARD 11)
set(prj_root ../../../../../..)
set(CMAKE_INSTALL_PREFIX ${prj_root}/..)
set(ROOT_BIN_DIR bin/${CMAKE_PLATFORM_NAME})

set(SOURCE_FILES
	post_process.cpp  runtime_commands.cpp  SandBox.cpp
    )

if("Android" MATCHES ${PLATFORM_ID})
	add_definitions(-DNO_ALGO -DNDEBUG -DLINUXDLL -D__ANDROID__ -DANDROID_SERVICE -DIAFSERVICES_EXPORTS -DLINUX_HOST -DOPENCV_VER=)
elseif("armv8" MATCHES ${CMAKE_SYSTEM_PROCESSOR})
	add_definitions(-DNDEBUG -D__linux__ -D__ARM__ -D__KERNEL__ -D_GNU_SOURCE -D__KERNEL_I2C__ -D__KERNEL_SPI__ -D__KERNEL_GPIO__ -DLINUX_TARGET -D__PL330_DMA__ -D__NU4000__ -D__HWLESS__)
endif()


include_directories(.
		${prj_root}/FW/nu4100/app/api/common
		${prj_root}/FW/nu4100/app/api/common/graph
		${prj_root}/FW/common/include
		${prj_root}/FW/nu4100/app/common/infrastructure/metadata/nanopb
        )
set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_STATIC_LIBS TRUE)

set(Boost_INCLUDE_DIR $ENV{BOOST_PATH})
set(Boost_LIBRARY_DIR "$ENV{BOOST_PATH}/${CMAKE_PLATFORM_NAME}/lib")
if("Android" MATCHES ${PLATFORM_ID})
	find_package(Boost 1.60 EXACT REQUIRED  system filesystem thread_pthread atomic chrono )
	find_library( # Sets the name of the path variable.
		           log-lib
	           # Specifies the name of the NDK library that
	           # you want CMake to locate.
	           log )

elseif("armv8" MATCHES ${CMAKE_SYSTEM_PROCESSOR})
	find_package(Boost 1.67 EXACT REQUIRED  system filesystem thread atomic chrono regex)
	LINK_DIRECTORIES(${prj_root}/bin/${CMAKE_PLATFORM_NAME} SandBoxFWLib/build)
endif()


add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# TODO: remove the section with SO files when cmake files are create for all projects


target_link_libraries(${PROJECT_NAME}
						SandBoxFW.a
						inu_host_lib_4100
						inu_usb_lib
						pthread
						usb-1.0
						rt
                   )

if(CMAKE_VS_PLATFORM_NAME)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        # On Windows need to set the Runtime folder in order to control the build path
        RUNTIME_OUTPUT_DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        ARCHIVE_OUTPUT_DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        DEBUG_POSTFIX "d"
    )
else()
	INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${ROOT_BIN_DIR})
endif()
