cmake_minimum_required(VERSION 3.6)
project(inu_host_lib)

set(CMAKE_CXX_STANDARD 11)
add_definitions(-D__ANDROID__ -DANDROID_HOST -Wno-format-security -MD -MP -O3)
set(INU_USB_LIB_ROOT "../inu_usb_lib")
set(INU_HOST_LIB_ROOT "./")
set(INU_HOST_LIB_SRC_ROOT ${INU_HOST_LIB_ROOT}/src)
set(INU_SRC_ROOT ${INU_HOST_LIB_ROOT}/../..)
set(INU_SRC_API_ROOT ${INU_SRC_ROOT}/api)
set(INU_SRC_COMMON_ROOT ${INU_SRC_ROOT}/common)

include_directories(
        ${LIBUSB_ROOT}/include                                                
        ${INU_USB_LIB_ROOT}/include                                           
        ${INU_SRC_API_ROOT}/common                                            
        ${INU_SRC_API_ROOT}/host                                              
        ${INU_SRC_COMMON_ROOT}/application/service/alg                        
        ${INU_SRC_COMMON_ROOT}/application/service/depth                      
        ${INU_SRC_COMMON_ROOT}/application/service/video                      
        ${INU_SRC_COMMON_ROOT}/application/service/webcam                     
        ${INU_SRC_COMMON_ROOT}/application/service/logger                     
        ${INU_SRC_COMMON_ROOT}/application/service/inject                     
        ${INU_SRC_COMMON_ROOT}/application/service/auxilary                   
        ${INU_SRC_COMMON_ROOT}/application/service/svc_mngr                   
        ${INU_SRC_COMMON_ROOT}/application/service/system                     
        ${INU_SRC_COMMON_ROOT}/application/service/client                     
        ${INU_SRC_COMMON_ROOT}/application/service/pos_sensors                
        ${INU_SRC_COMMON_ROOT}/application/service/audio                      
        ${INU_SRC_COMMON_ROOT}/application/version                            
        ${INU_SRC_COMMON_ROOT}/infrastructure/utils/inc                       
        ${INU_SRC_COMMON_ROOT}/infrastructure/service/inc                     
        ${INU_SRC_COMMON_ROOT}/infrastructure/os/inc                          
        ${INU_SRC_COMMON_ROOT}/infrastructure/cls/src/network                 
        ${INU_SRC_COMMON_ROOT}/infrastructure/cls/src/interface               
        ${INU_SRC_COMMON_ROOT}/infrastructure/cls/src/infrastructure          
        ${INU_SRC_COMMON_ROOT}/infrastructure/cls/inc                         
        ${INU_SRC_COMMON_ROOT}/infrastructure/inc                             
        ${INU_SRC_COMMON_ROOT}/infrastructure/marshal/inc                     
        ${INU_SRC_COMMON_ROOT}/infrastructure/marshal/                        
        ${INU_SRC_COMMON_ROOT}/infrastructure/mem/inc/                        
        ${INU_SRC_COMMON_ROOT}/infrastructure/stream
)

set(SOURCE_FILES
        ${INU_SRC_COMMON_ROOT}/api/src/inu.c                                          
        ${INU_SRC_COMMON_ROOT}/api/src/inu_sdk.c                                      
        ${INU_SRC_COMMON_ROOT}/application/service/inject/inject.c                    
        ${INU_SRC_COMMON_ROOT}/application/service/depth/depth.c                      
        ${INU_SRC_COMMON_ROOT}/application/service/alg/alg.c                          
        ${INU_SRC_COMMON_ROOT}/application/service/logger/logger.c                    
        ${INU_SRC_COMMON_ROOT}/application/service/svc_mngr/svc_mngr.c                
        ${INU_SRC_COMMON_ROOT}/application/service/svc_mngr/svc_cfg.c                 
        ${INU_SRC_COMMON_ROOT}/application/service/video/video.c                      
        ${INU_SRC_COMMON_ROOT}/application/service/webcam/webcam.c                    
        ${INU_SRC_COMMON_ROOT}/application/service/system/system.c                    
        ${INU_SRC_COMMON_ROOT}/application/service/client/client.c                    
        ${INU_SRC_COMMON_ROOT}/application/service/pos_sensors/pos_sensors.c          
        ${INU_SRC_COMMON_ROOT}/application/service/audio/audio.c              
        ${INU_SRC_COMMON_ROOT}/infrastructure/cls/src/infrastructure/serial.c         
        ${INU_SRC_COMMON_ROOT}/infrastructure/cls/src/interface/uart_com.c            
        ${INU_SRC_COMMON_ROOT}/infrastructure/cls/src/interface/cdc_com.c            
        ${INU_SRC_COMMON_ROOT}/infrastructure/cls/src/interface/usb_ctrl_com_host.c   
        ${INU_SRC_COMMON_ROOT}/infrastructure/cls/src/network/conn_lyr.c              
        ${INU_SRC_COMMON_ROOT}/infrastructure/cls/src/network/queue_lyr.c             
        ${INU_SRC_COMMON_ROOT}/infrastructure/marshal/src/marshal.c                   
        ${INU_SRC_COMMON_ROOT}/infrastructure/marshal/src/m_ioctl.c
        ${INU_SRC_COMMON_ROOT}/infrastructure/marshal/src/m_tbl.c
        ${INU_SRC_COMMON_ROOT}/infrastructure/os/src/os_lyr_android.c                 
        ${INU_SRC_COMMON_ROOT}/infrastructure/os/src/inu_queue.c                      
        ${INU_SRC_COMMON_ROOT}/infrastructure/os/src/linked_list.c                    
        ${INU_SRC_COMMON_ROOT}/infrastructure/os/src/mem_pool.c                       
        ${INU_SRC_COMMON_ROOT}/infrastructure/service/src/service.c                   
        ${INU_SRC_COMMON_ROOT}/infrastructure/service/src/svc_com.c                   
        ${INU_SRC_COMMON_ROOT}/infrastructure/utils/src/inu_str.c                     
        ${INU_SRC_COMMON_ROOT}/infrastructure/utils/src/hash.c                
        ${INU_SRC_COMMON_ROOT}/infrastructure/utils/src/log.c                         
        ${INU_SRC_COMMON_ROOT}/infrastructure/utils/src/utils.c                       
        ${INU_SRC_COMMON_ROOT}/infrastructure/utils/src/inu_profiler.c        
        ${INU_SRC_COMMON_ROOT}/infrastructure/utils/src/config.c              
        ${INU_SRC_COMMON_ROOT}/infrastructure/utils/src/inu_defs_params.c
        ${INU_SRC_COMMON_ROOT}/infrastructure/stream/stream.c                         
        )

find_library( # Sets the name of the path variable.
        log-lib
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log )



add_library(inu_host_lib SHARED ${SOURCE_FILES})

target_link_libraries(inu_host_lib inu_usb_lib usb-1.0 ${log-lib})