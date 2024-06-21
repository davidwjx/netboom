
####################
# OpenCV definitions
####################

if (NOT LOCAL_RESOURCES)
	if (NOT OPENCV_VER)
		set(OpenCV_VERSION "4.5.1")
		if(CMAKE_PLATFORM_NAME STREQUAL "linux_gcc-5.4_x86_64" OR CMAKE_PLATFORM_NAME STREQUAL "linux_gcc-5.4_armv8")
			set(OpenCV_VERSION "3.0.0")
		endif()
	elseif (NOT LOCAL_RESOURCES)
		set(OpenCV_VERSION ${OPENCV_VER})
	else()
		message("Using the OpenCV version that is installed on current machine")
		# Do nothing - use the installed opencv
	endif()


	if ((NOT OPENCV_ROOT) AND (NOT OPENCV_VER))

		string(TOUPPER ${CMAKE_PLATFORM_NAME} UPPER_PLATFORM)
		
		if (CMAKE_VS_PLATFORM_NAME)
			find_package(OpenCV ${OpenCV_VERSION} EXACT REQUIRED ${OpenCV_Required_Libs} PATHS ${CONAN_OPENCV_${UPPER_PLATFORM}_ROOT}/cmake)
		endif()

		if(NOT OpenCV_FOUND)
			if (CMAKE_VS_PLATFORM_NAME) 
				use_package(${PRODUCT_NAME} "OpenCV" ${OpenCV_VERSION})
			else()
				set(OpenCV_INCLUDE_DIRS "${CONAN_INCLUDE_DIRS_OPENCV_${UPPER_PLATFORM}}")
			endif()
		endif()
		
		message( STATUS "opencv libs: " "${OpenCV_LIBS}")
		set(OpenCV_LIBS ${OpenCV_LIBS} ${OpenCV_Required_Libs})
		# Add 3rdparty libraries for Android
		if(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
			set(OpenCV_LIBS ${OpenCV_LIBS} "tbb;tegra_hal;ittnotify")
		endif()
		
		set(OpenCV_LIBRARY_DIRS ${CONAN_LIB_DIRS_OPENCV_${UPPER_PLATFORM}} ${CONAN_LIB_DIRS_OPENCV_${UPPER_PLATFORM}}/3rdparty)
		if (CMAKE_VS_PLATFORM_NAME)
			set(OpenCV_BIN_DIRS ${CONAN_BIN_DIRS_OPENCV_${UPPER_PLATFORM}})
		else()
			set(OpenCV_BIN_DIRS ${OpenCV_LIBRARY_DIRS})
		endif()

		target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})
		target_link_directories(${PROJECT_NAME} PUBLIC ${OpenCV_LIBRARY_DIRS})
		target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})

		message("OpenCV includes: " "${OpenCV_INCLUDE_DIRS}")
		message("OpenCV lib directory: " "${OpenCV_LIBRARY_DIRS}")
		message("OpenCV bin directory: " "${OpenCV_BIN_DIRS}")
		message("OpenCV libraries: " "${OpenCV_LIBS}")

	else()
		if (OPENCV_ROOT)
			set(OpenCV_DIR ${OPENCV_ROOT}/build/cmake)
		endif()

		find_package(OpenCV ${OpenCV_VERSION} EXACT REQUIRED ${OpenCV_Required_Libs})

		if(NOT OpenCV_FOUND)
			if (CMAKE_VS_PLATFORM_NAME)
				use_package(${PRODUCT_NAME} "OpenCV" ${OpenCV_VERSION})
			else()
				set(OpenCV_LIBS ${OpenCV_LIBS} ${OpenCV_Required_Libs})
				set(OpenCV_INCLUDE_DIRS "${OPENCV_ROOT}/build/include")

				if (IS_DIRECTORY ${OPENCV_ROOT}/build/${CMAKE_PLATFORM_NAME})
					set(OpenCV_LIBRARY_DIRS "${OPENCV_ROOT}/build/${CMAKE_PLATFORM_NAME}" "${OPENCV_ROOT}/build/3rdparty/${CMAKE_PLATFORM_NAME}")
					set(OpenCV_BIN_DIRS "${OPENCV_ROOT}/build/${CMAKE_PLATFORM_NAME}" "${OPENCV_ROOT}/build/3rdparty/${CMAKE_PLATFORM_NAME}")
				else()
					if (NOT IS_DIRECTORY ${OPENCV_ROOT}/build/lib} )
						message("FATAL ERROR: Can't find opencv libraries")
					else()
						set(OpenCV_LIBRARY_DIRS "${OPENCV_ROOT}/build/lib" "${OPENCV_ROOT}/build/3rdparty/lib")
						set(OpenCV_BIN_DIRS "${OPENCV_ROOT}/build/bin/" "${OPENCV_ROOT}/build/3rdparty/bin")
					endif()
				endif()
			endif()
		else()
			# On Windows these variables are not defined
			set(OpenCV_LIBRARY_DIRS "${OpenCV_INSTALL_PATH}/lib" "${OpenCV_INSTALL_PATH}/3rdparty/lib")
			set(OpenCV_BIN_DIRS "${OpenCV_INSTALL_PATH}/bin" "${OpenCV_INSTALL_PATH}/3rdparty/bin")
		endif()
	endif()
else()
	find_package(OpenCV ${OpenCV_VERSION} EXACT REQUIRED ${OpenCV_Required_Libs})
endif()

# Add 3rdparty libraries for Android
if(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
	set(OpenCV_LIBS ${OpenCV_LIBS} "tbb;tegra_hal;ittnotify")
endif()

message("OpenCV includes: " "${OpenCV_INCLUDE_DIRS}")
message("OpenCV lib directory: " "${OpenCV_LIBRARY_DIRS}")
message("OpenCV bin directory: " "${OpenCV_BIN_DIRS}")
message("OpenCV libraries: " "${OpenCV_LIBS}")

target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS}) 
target_link_directories(${PROJECT_NAME} PUBLIC ${OpenCV_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})

