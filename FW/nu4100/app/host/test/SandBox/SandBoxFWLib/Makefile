PROJ_BASE=../../../../

TARGET_=libsandbox_fw_lib
#TARGET_TEST=inu_host_test
#CROSS_COMPILE_DIR = /tools/gcc-arm/gcc-linaro/4.7-2012.12
CROSS_COMPILE_DIR = /opt/inuitive/toolchain/linaro/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf
#PLATFORM_NAME ?= tizen_gcc-4.8_armv7a
#CROSS_COMPILE_LIB = /tools/gcc-arm/gcc-linaro/4.7-2012.12/arm-linux-gnueabihf/libc/lib/arm-linux-gnueabihf
CROSS_COMPILE_LIB = /opt/inuitive/toolchain/linaro/4.7-2012.12/arm-linux-gnueabihf/libc/lib/arm-linux-gnueabihf
################################################################################
##	Cross-compiler common defines
################################################################################
CROSS_COMPILE ?= $(CROSS_COMPILE_DIR)/bin/arm-linux-gnueabihf-
CC=$(CROSS_COMPILE)gcc
AS=$(CROSS_COMPILE)as
AR=$(CROSS_COMPILE)ar
LD=$(CROSS_COMPILE)ld
ELD=$(CROSS_COMPILE)objdump
EL2H=$(CROSS_COMPILE)objcopy
SZ=$(CROSS_COMPILE)size
NM=$(CROSS_COMPILE)nm
STRIP=$(CROSS_COMPILE)strip

TARGET_LIB=../../../../target/gp/prj/make/libinu_target

#IDE_LIBRARIES = -static -pthread -static -lrt
#IDE_LIBRARIES = -lrt -lpthread
IDE_LIBRARIES = -static $(TARGET_LIB).a -lpthread -lm -ldl -lstdc++ -lguvc
#IDE_LIBRARIES_TEST = -Wl,-rpath,./ -static -linu_host_lib $(IDE_LIBRARIES)
#IDE_LIBRARIES_TEST = -linu_host_lib $(IDE_LIBRARIES)
PATH_LIBRARIES = -L $(CROSS_COMPILE_LIB) -L  ../../../../../../../3rd_party/libguvc-main/libguvc/src/lib/.libs/
#PATH_LIBRARIES_TEST = -L . $(PATH_LIBRARIES)
ifeq ($(AT),)
AT:=@
else
AT:=
endif

INCLUDE_DIRS=								\
	$(PROJ_BASE)api/common					\
	$(PROJ_BASE)api/common/graph			\
	$(PROJ_BASE)../../common/include		\


	
SRC_DIRS=								\
        $(PROJ_BASE)api                                                 \
        $(PROJ_BASE)common/application/service/inject             	\
		$(PROJ_BASE)host/inu_host_lib/src				\
		$(PROJ_BASE)common/infrastructure/stream			\
		$(PROJ_BASE)host/inu_host_lib/test

vpath %.c $(SRC_DIRS)
vpath %.h $(SRC_DIRS)
vpath %.h $(INCLUDE_DIRS)

#CPPFLAGS=$(addprefix -I,$(INCLUDE_DIRS)) -DLINUX_HOST -D_GNU_SOURCE -Wno-format-security -MD -MP -Wno-unused-parameter -mcpu=cortex-a7 -fPIC -Wall
CPPFLAGS=$(addprefix -I,$(INCLUDE_DIRS)) -DLINUX_GP_HOST -DDEFSG_IS_UART_MIPI -D_GNU_SOURCE -DLINUX_MAKE_BUILD -Wno-format-security -MD -MP -fPIC -Wno-unused-parameter -Wall

#OBJS=$(patsubst %.c,%.o,$(filter-out $(SRCS_FILTER_OUT),$(SRCS)))
OBJS=					\
	SandBoxLib.obj		\
#OBJS_TEST=$(patsubst %.c,%.o,$(filter-out $(SRCS_FILTER_OUT_TEST),$(SRCS_TEST)))

COMPILE.c = $(AT)echo "Compiling $(notdir $<)"; $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c


# Full list of files to compile

SRCS=										\
	SandBoxLib.c							\

$(OBJS) : %.obj : %.cpp
	@echo "  CC  $(CROSS_COMPILE) "$<
	$(V)$(CC) $(CPPFLAGS) $(CINCLUDE) $(CDEFINES) -c -MD -o $@  $<

all: $(TARGET_).a
	echo $(TARGET_)

$(TARGET_).a: $(OBJS)
	@echo Updating $(OBJS)
	$(AT)$(AR) -r $(TARGET_).a $(OBJS)

#$(TARGET_).so: $(OBJS)
	#@echo "Creating $@"
	#$(AT)$(CC) $(OBJS) $(PATH_LIBRARIES) $(IDE_LIBRARIES) -Wl,-soname,$(TARGET_).so -shared -o $(TARGET_).so 
#	$(AT)$(CC) $(OBJS) $(PATH_LIBRARIES) $(IDE_LIBRARIES) -Wl,-soname,$(TARGET_).so.1 -shared -o $(TARGET_).so.1.0.0 
#
##$(TARGET_TEST): $(OBJS_TEST)
##	@echo "Creating $@"	
##	$(AT)$(CC) -static $(PATH_LIBRARIES_TEST) $(OBJS_TEST) $(IDE_LIBRARIES_TEST) -o $(TARGET_TEST) 

rebuild: clean all

clean: 
	@echo "Cleaning..."
	@echo $(SRC_DIRS)
	-$(AT)$(RM) $(OBJS) *.d
	-$(AT)$(RM) $(TARGET_).a $(TARGET_).so*
#	-$(AT)$(RM) $(OBJS_TEST) $(TARGET_TEST)	

clean_all: clean
	@echo "Cleanning more..."
	-$(AT)$(RM) $(TARGET_).a $(TARGET_).so.*
#	-$(AT)$(RM) $(OBJS_TEST) $(TARGET_TEST)	
	
-include $(patsubst %.c,%.d,$(filter-out $(SRCS_FILTER_OUT),$(SRCS)))
