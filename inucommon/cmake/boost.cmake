####################
# Boost definitions
####################

set(Boost_ARCHITECTURE -x64)

if (LOCAL_RESOURCES)
    set(Boost_NO_SYSTEM_PATHS OFF)
    set(Boost_NO_BOOST_CMAKE OFF)
    set(Boost_USE_STATIC_LIBS OFF)
else()
    set(Boost_NO_SYSTEM_PATHS ON)
    set(Boost_NO_BOOST_CMAKE ON)
    set(Boost_USE_STATIC_LIBS ON)

    set(Boost_VERSION "1.75")
    if(CMAKE_PLATFORM_NAME STREQUAL "linux_gcc-7.4_x86_64" OR
            CMAKE_PLATFORM_NAME STREQUAL "android_ndk23b_android-27-arm64-v8a" OR
            CMAKE_PLATFORM_NAME STREQUAL "linux_gcc-7.3_armv8")
        set(Boost_VERSION "1.67")
    endif()
    if(CMAKE_PLATFORM_NAME STREQUAL "linux_gcc-5.4_x86_64")
        set(Boost_VERSION "1.60")
    endif()
    if(CMAKE_PLATFORM_NAME STREQUAL "linux_gcc-5.4_armv8")
        set(Boost_VERSION "1.62")
    endif()
endif()    

if (NOT BOOST_ROOT AND NOT LOCAL_RESOURCES)
    string(TOUPPER ${CMAKE_PLATFORM_NAME} UPPER_PLATFORM)
    
    set(BOOST_ROOT "${CONAN_BOOST_${UPPER_PLATFORM}_ROOT}")

    if(CMAKE_VS_PLATFORM_NAME)
        find_package(Boost ${Boost_VERSION} REQUIRED ${Boost_Required_Libs} PATHS ${BOOST_ROOT}/lib/cmake)
        if(NOT Boost_FOUND)
            message(FATAL_ERROR "Failed to locate Boost, you can set the Boost path manually")
            use_package(${PRODUCT_NAME} "Boost" ${Boost_VERSION})
        endif()
    endif()

    set(Boost_INCLUDE_DIRS "${CONAN_INCLUDE_DIRS_BOOST_${UPPER_PLATFORM}}")

    set(Boost_LIBRARY_DIR "${CONAN_LIB_DIRS_BOOST_${UPPER_PLATFORM}}")

    target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS}) 
    target_link_directories(${PROJECT_NAME} PUBLIC ${Boost_LIBRARY_DIR})
    add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)
    
    if(NOT CMAKE_VS_PLATFORM_NAME)
        foreach(lib IN ITEMS ${Boost_Required_Libs})
            set(Boost_LIBRARIES ${Boost_LIBRARIES} "boost_${lib}")            
        endforeach()
    endif()
    target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES})

    # Reset, so it will not be used if boost is included again
    set(BOOST_ROOT "")
else()
    if (NOT LOCAL_RESOURCES)
        # Required here, since our Boost tree is not as expected. Otherwise find_package will fail. 
        set(Boost_LIBRARY_DIR "${BOOST_ROOT}/${CMAKE_PLATFORM_NAME}/lib")
    endif()

    if(NOT CMAKE_VS_PLATFORM_NAME)
        set(Boost_INCLUDE_DIRS "${BOOST_ROOT}")
    endif()

    if (LOCAL_RESOURCES)
        find_package(Boost COMPONENTS  ${Boost_Required_Libs})
    else()
        find_package(Boost ${Boost_VERSION} EXACT REQUIRED ${Boost_Required_Libs})
    endif()        

    if(Boost_FOUND)
        target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS}) 
        target_link_directories(${PROJECT_NAME} PUBLIC ${Boost_LIBRARY_DIR})
        add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)
        
        if(NOT CMAKE_VS_PLATFORM_NAME)
            target_link_libraries(${PROJECT_NAME} PUBLIC "${Boost_LIBRARIES}")
        endif()

    else()
        use_package(${PRODUCT_NAME} "Boost" ${Boost_VERSION})
    endif()
endif()

message("boost Root: " "${BOOST_ROOT}")
message("boost Version: " "${Boost_VERSION}")
message("boost includes: " "${Boost_INCLUDE_DIRS}")
message("boost libraries: " "${Boost_LIBRARIES}")

