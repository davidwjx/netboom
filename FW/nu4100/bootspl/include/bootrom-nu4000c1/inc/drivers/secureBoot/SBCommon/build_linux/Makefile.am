##
## File: Makefile.am
##
## Template for building Makefile with automake+configure
##

##############################################################################
## Copyright (c) 2009-2018 INSIDE Secure B.V. All Rights Reserved.
##
## This confidential and proprietary software may be used only as authorized
## by a licensing agreement from INSIDE Secure.
##
## The entire notice above must be reproduced on all authorized copies that
## may only be made to the extent permitted by a licensing agreement from
## INSIDE Secure.
##
## For more information or support, please go to our online support system at
## https://customersupport.insidesecure.com.
## In case you do not have an account for this system, please send an e-mail
## to ESSEmbeddedHW-Support@insidesecure.com.
##############################################################################
#
# Configuration Options:
#
# ENABLE_DEBUG
#    Enable debugging
# ENABLE_STACK
#    Enable stack tracing
# ENABLE_HEAP
#    Enable heap tracing
# ENABLE_PERF
#    Enable performance info
# ENABLE_COVERAGE
#    Enable coverage tracing
#

AM_CFLAGS = -std=gnu99 -Wall -pedantic
top_src = $(top_srcdir)/../..

if ENABLE_STACK
CFLAGS += -finstrument-functions
CPPFLAGS += -DSTACK_MEASUREMENT -DCFG_ENABLE_STACK
endif

if ENABLE_HEAP
CPPFLAGS += -DHEAP_MEASUREMENT -DCFG_ENABLE_HEAP
endif

if ENABLE_GCC_STRICT_WARNINGS
AM_CFLAGS += -W -Wshadow -Wvla
endif

if ENABLE_DEBUG
# Debugging enabled.
CPPFLAGS += -DCFG_ENABLE_DEBUG
else
# No debugging (performance build)
CPPFLAGS += -DCFG_IMPLDEFS_NO_DEBUG
endif

if ENABLE_COVERAGE
CPPFLAGS += -DGCOV_PROFILE -DCFG_ENABLE_COVERAGE
LIBS += -lgcov
if ENABLE_DEBUG
# With debugging, test coverage analysis requires enabling additional flag to
# dump coverage reports prior abnormal exit
CPPFLAGS += -DDEBUG_CF_ABORT_WRITE_PROFILE
endif
endif

CONFIGURATION_INCLUDES = \
    -I$(top_src)/SBCommon/Config \
    -I$(top_src)/Config

include_HEADERS =

lib_LIBRARIES =

SB_TEST_PROGRAMS_LIST = \
    sbswsha224 \
    sbswsha256 \
    sbswsha384 \
    sbswsha512 \
    sbswecdsa224 \
    sbswecdsa256 \
    sbswecdsa384 \
    sbswecdsa521 \
    sbswendian_opt \
    sbswendian_force_align_opt \
    sbswendian_noopt \
    sbswendian_force_align_noopt

noinst_PROGRAMS = \
    rng-health \
    $(SB_TEST_PROGRAMS_LIST)

if ENABLE_PERF
PERF_PROGRAM_LIST = \
    perf_sbswsha224 \
    perf_sbswsha256 \
    perf_sbswsha384 \
    perf_sbswsha512 \
    perf_sbswecdsa224 \
    perf_sbswecdsa256 \
    perf_sbswecdsa384 \
    perf_sbswecdsa521

noinst_PROGRAMS += $(PERF_PROGRAM_LIST)
endif

noinst_LIBRARIES = \
    libsbswsha224.a \
    libsbswsha256.a \
    libsbswsha384.a \
    libsbswsha512.a \
    libsbswecdsa224.a \
    libsbswecdsa256.a \
    libsbswecdsa384.a \
    libsbswecdsa521.a \
    libsfzutf.a \
    libfmwk_sb.a

check_PROGRAMS = \
    $(SB_TEST_PROGRAMS_LIST)

check_LIBRARIES = \
    libsfzutf.a \
    libfmwk_sb.a

SBCOMMON_CPPFLAGS_LIST = \
    $(CONFIGURATION_INCLUDES) \
    -I$(top_src)/Framework/PUBDEFS/incl \
    -I$(top_src)/Framework/IMPLDEFS/incl \
    -I$(top_src)/Framework/CLIB/incl \
    -I$(top_src)/SFZUTF/incl \
    -I$(top_src)/micro-ecc \
    -I$(top_src)/SBCommon/SBIF_API/incl \
    -I$(top_src)/SBCommon/SBIF_TOOLS/incl \
    -I$(top_src)/SBCommon/SB_SW_CRYPTO/incl

# Software Only Secure Boot tests
SBSW_CPPFLAGS = \
    $(CONFIGURATION_INCLUDES) \
    -I$(top_src)/Framework/PUBDEFS/incl \
    -I$(top_src)/Framework/IMPLDEFS/incl \
    -I$(top_src)/Framework/CLIB/incl \
    -I$(top_src)/SFZUTF/incl \
    -I$(top_src)/SBCommon/SB_SW_CRYPTO/incl \
    -I$(top_src)/SBCommon/SBIF_API/incl \
    -I$(top_src)/SBCommon/SBIF_TOOLS/incl \
    -I$(top_src)/micro-ecc \
    -DuECC_VLI_NATIVE_LITTLE_ENDIAN=1 -DuECC_WORD_SIZE=4

sbswsha_CPPFLAGS = \
    $(SBSW_CPPFLAGS) \
    -DSBSWCRYPTO_CF_FULL_FUNCTIONALITY

sbswsha_SOURCES = \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/minsha_init.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/minsha_update.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/minsha_core.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sb_sw_hash_sha512.c

libsbswsha224_a_SOURCES = \
    $(sbswsha_SOURCES)

libsbswsha224_a_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC224 \
    $(SBSW_CPPFLAGS)

libsbswsha256_a_SOURCES = \
    $(sbswsha_SOURCES)

libsbswsha256_a_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC256 \
    $(SBSW_CPPFLAGS)

libsbswsha384_a_SOURCES = \
    $(sbswsha_SOURCES)

libsbswsha384_a_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC384 \
    $(SBSW_CPPFLAGS)

libsbswsha512_a_SOURCES = \
    $(sbswsha_SOURCES)

libsbswsha512_a_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC521 \
    $(SBSW_CPPFLAGS)

libsbswecdsa224_a_SOURCES = \
    $(top_src)/micro-ecc/uECC.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sb_sw_ecdsa224.c

libsbswecdsa224_a_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC224 \
    $(SBSW_CPPFLAGS) \
    -DuECC_SUPPORTS_secp224r1=1 -DSUPPORT_POINT_VALIDATION

libsbswecdsa256_a_SOURCES = \
    $(top_src)/micro-ecc/uECC.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sb_sw_ecdsa256.c

libsbswecdsa256_a_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC256 \
    $(SBSW_CPPFLAGS) \
    -DuECC_SUPPORTS_secp256r1=1 -DSUPPORT_POINT_VALIDATION

libsbswecdsa384_a_SOURCES = \
    $(top_src)/micro-ecc/uECC.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sb_sw_hash_sha512.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sb_sw_ecdsa384.c

libsbswecdsa384_a_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC384 \
    $(SBSW_CPPFLAGS) \
    -DuECC_SUPPORTS_secp384r1=1 -DSUPPORT_POINT_VALIDATION

libsbswecdsa521_a_SOURCES = \
    $(top_src)/micro-ecc/uECC.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sb_sw_hash_sha512.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sb_sw_ecdsa521.c

libsbswecdsa521_a_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC521 \
    $(SBSW_CPPFLAGS) \
    -DuECC_SUPPORTS_secp521r1=1 -DSUPPORT_POINT_VALIDATION

sbswecdsa224_CPPFLAGS = $(libsbswecdsa224_a_CPPFLAGS)

sbswecdsa224_SOURCES = \
    $(top_src)/SBCommon/SB_TEST/src/lib-sbswecdsa224.c \
    $(top_src)/SBCommon/SB_TEST/src/sbswecdsa224.c

sbswecdsa224_LDADD = \
    libsbswecdsa224.a \
    libsbswsha224.a \
    $(check_LIBRARIES)

sbswecdsa256_CPPFLAGS = $(libsbswecdsa256_a_CPPFLAGS)

sbswecdsa256_SOURCES = \
    $(top_src)/SBCommon/SB_TEST/src/lib-sbswecdsa256.c \
    $(top_src)/SBCommon/SB_TEST/src/sbswecdsa256.c

sbswecdsa256_LDADD = \
    libsbswecdsa256.a \
    libsbswsha256.a \
    $(check_LIBRARIES)

sbswecdsa384_CPPFLAGS = $(libsbswecdsa384_a_CPPFLAGS)

sbswecdsa384_SOURCES = \
    $(top_src)/SBCommon/SB_TEST/src/lib-sbswecdsa384.c \
    $(top_src)/SBCommon/SB_TEST/src/sbswecdsa384.c

sbswecdsa384_LDADD = \
    libsbswecdsa384.a \
    libsbswsha384.a \
    $(check_LIBRARIES)

sbswecdsa521_CPPFLAGS = $(libsbswecdsa521_a_CPPFLAGS)

sbswecdsa521_SOURCES = \
    $(top_src)/SBCommon/SB_TEST/src/lib-sbswecdsa521.c \
    $(top_src)/SBCommon/SB_TEST/src/sbswecdsa521.c

sbswecdsa521_LDADD = \
    libsbswecdsa521.a \
    libsbswsha512.a \
    $(check_LIBRARIES)

perf_sbswecdsa224_CPPFLAGS = $(sbswecdsa224_CPPFLAGS) -DPERF

perf_sbswecdsa224_SOURCES = $(sbswecdsa224_SOURCES)

perf_sbswecdsa224_LDADD = $(sbswecdsa224_LDADD)

perf_sbswecdsa256_SOURCES = $(sbswecdsa256_SOURCES)

perf_sbswecdsa256_CPPFLAGS = $(sbswecdsa256_CPPFLAGS) -DPERF

perf_sbswecdsa256_LDADD = $(sbswecdsa256_LDADD)

perf_sbswecdsa384_SOURCES = $(sbswecdsa384_SOURCES)

perf_sbswecdsa384_CPPFLAGS = $(sbswecdsa384_CPPFLAGS) -DPERF

perf_sbswecdsa384_LDADD = $(sbswecdsa384_LDADD)

perf_sbswecdsa521_SOURCES = $(sbswecdsa521_SOURCES)

perf_sbswecdsa521_CPPFLAGS = $(sbswecdsa521_CPPFLAGS) -DPERF

perf_sbswecdsa521_LDADD = $(sbswecdsa521_LDADD)

sbswsha224_LDADD = \
    libsbswsha224.a \
    $(check_LIBRARIES)

sbswsha224_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC224 \
    $(SBSW_CPPFLAGS)

sbswsha224_SOURCES = \
    $(top_src)/SBCommon/SB_TEST/src/lib-sbswhashtest.c \
    $(top_src)/SBCommon/SB_TEST/src/sbswsha224.c

perf_sbswsha224_LDADD = $(sbswsha224_LDADD)

perf_sbswsha224_CPPFLAGS = $(sbswsha224_CPPFLAGS) -DPERF

perf_sbswsha224_SOURCES = $(sbswsha224_SOURCES)

sbswsha256_LDADD = \
    libsbswsha256.a \
    $(check_LIBRARIES)

sbswsha256_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC256 \
    $(SBSW_CPPFLAGS)

sbswsha256_SOURCES = \
    $(top_src)/SBCommon/SB_TEST/src/lib-sbswhashtest.c \
    $(top_src)/SBCommon/SB_TEST/src/sbswsha256.c

perf_sbswsha256_LDADD = $(sbswsha256_LDADD)

perf_sbswsha256_CPPFLAGS = $(sbswsha256_CPPFLAGS) -DPERF

perf_sbswsha256_SOURCES = $(sbswsha256_SOURCES)

sbswsha384_LDADD = \
    libsbswsha384.a \
    $(check_LIBRARIES)

sbswsha384_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC384 \
    $(SBSW_CPPFLAGS)

sbswsha384_SOURCES = \
    $(top_src)/SBCommon/SB_TEST/src/lib-sbswhashtest.c \
    $(top_src)/SBCommon/SB_TEST/src/sbswsha384.c

perf_sbswsha384_LDADD = $(sbswsha384_LDADD)

perf_sbswsha384_CPPFLAGS = $(sbswsha384_CPPFLAGS) -DPERF

perf_sbswsha384_SOURCES = $(sbswsha384_SOURCES)

sbswsha512_LDADD = \
    libsbswsha512.a \
    $(check_LIBRARIES)

sbswsha512_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC521 \
    $(SBSW_CPPFLAGS)

sbswsha512_SOURCES = \
    $(top_src)/SBCommon/SB_TEST/src/lib-sbswhashtest.c \
    $(top_src)/SBCommon/SB_TEST/src/sbswsha512.c

perf_sbswsha512_LDADD = $(sbswsha512_LDADD)

perf_sbswsha512_CPPFLAGS = $(sbswsha512_CPPFLAGS) -DPERF

perf_sbswsha512_SOURCES = $(sbswsha512_SOURCES)

# Endianness testing macros:
sbswendian_opt_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC224 \
    $(SBCOMMON_CPPFLAGS_LIST)

sbswendian_opt_LDADD = \
    $(check_LIBRARIES)

sbswendian_opt_SOURCES = \
    $(top_src)/SBCommon/SB_TEST/src/sbswendiantest.c

sbswendian_force_align_opt_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC224 \
    $(SBCOMMON_CPPFLAGS_LIST) \
    -DSB_SW_FORCE_ALIGNED_ACCESS

sbswendian_force_align_opt_LDADD = \
    $(check_LIBRARIES)

sbswendian_force_align_opt_SOURCES = \
    $(top_src)/SBCommon/SB_TEST/src/sbswendiantest.c

sbswendian_noopt_CFLAGS = -O0
sbswendian_noopt_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC224 \
    $(SBCOMMON_CPPFLAGS_LIST)

sbswendian_noopt_LDADD = \
    $(check_LIBRARIES)

sbswendian_noopt_SOURCES = \
    $(top_src)/SBCommon/SB_TEST/src/sbswendiantest.c

sbswendian_force_align_noopt_CFLAGS = -O0
sbswendian_force_align_noopt_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC224 \
    $(SBCOMMON_CPPFLAGS_LIST) \
    -DSB_SW_FORCE_ALIGNED_ACCESS

sbswendian_force_align_noopt_LDADD = \
    $(check_LIBRARIES)

sbswendian_force_align_noopt_SOURCES = \
    $(top_src)/SBCommon/SB_TEST/src/sbswendiantest.c

rng_health_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config \
    -I$(top_src)/Framework/PUBDEFS/incl \
    -I$(top_src)/Framework/IMPLDEFS/incl \
    -I$(top_src)/Framework/CLIB/incl \
    -I$(top_src)/SBCommon/SB_SW_CRYPTO/incl \
    -I$(top_src)/Framework/SPAL_API/incl \
    $(CONFIGURATION_INCLUDES) \
    -DAES_SUPPORT_ENCRYPT

rng_health_SOURCES = \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/nist-sp-chain.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/nist-sp-800-90.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/nist-health-test.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(top_src)/SBCommon/SB_TEST/src/rng-health.c

rng_health_LDADD = \
    $(check_LIBRARIES)

# Notice: nobuffer.so is compiled with non-automake rule
nobuffer.so: $(top_src)/SFZUTF/util/nobuffer.c
	$(CC) $(CFLAGS) $(CFLAGS) -shared -o $@ $<

# Flags & rules for running test runner
TESTRUNNER_FLAGS = "--timeout=9999"

list_check:
	@for test in $(check_PROGRAMS); do echo '***' $$test;./$$test --list 2>&1; done

run_check: $(check_PROGRAMS)
	@ok=0;count=0;for test in $(check_PROGRAMS); do count=`expr $$count + 1`; echo $(TESTRUNNER_PRE) $(top_src)/SFZUTF/scripts/testrunner.pl $(TESTRUNNER_FLAGS) --xmllog=$$test.xml $$test; $(TESTRUNNER_PRE) $(top_src)/SFZUTF/scripts/testrunner.pl $(TESTRUNNER_FLAGS) --xmllog=$$test.xml $$test; if [ "$$?" -eq "0" ];then ok=`expr $$ok + 1`;fi; done; echo "*** $$count TEST SUITES: $$ok/$$count SUCCESSFUL"

run_check_ssh: $(check_PROGRAMS)
	@ok=0;count=0;for test in $(check_PROGRAMS); do count=`expr $$count + 1`; scp $$test $(SSH_TARGET):/tmp/test;echo $(TESTRUNNER_PRE) $(top_src)/SFZUTF/scripts/testrunner.pl $(TESTRUNNER_FLAGS) --interpreter=ssh --interpreter-opts="-t $(SSH_TARGET)" --xmllog=$$test.xml /tmp/test '#'$$test; $(TESTRUNNER_PRE) $(top_src)/SFZUTF/scripts/testrunner.pl $(TESTRUNNER_FLAGS) --interpreter=ssh --interpreter-opts="-t $(SSH_TARGET)" --xmllog=$$test.xml /tmp/test; if [ "$$?" -eq "0" ];then ok=`expr $$ok + 1`;fi; done; echo "*** $$count TEST SUITES: $$ok/$$count SUCCESSFUL"


# sfzutf is required by sbswsha*
libsfzutf_a_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config \
    -I$(top_src)/Framework/PUBDEFS/incl \
    -I$(top_src)/Framework/IMPLDEFS/incl \
    -I$(top_src)/Framework/CLIB/incl \
    -I$(top_src)/SFZUTF/incl \
    -I$(top_src)/Framework/SPAL_API/incl \
    $(CONFIGURATION_INCLUDES)

libsfzutf_a_SOURCES = \
    $(top_src)/SFZUTF/src/sfzutf.c \
    $(top_src)/SFZUTF/src/sfzutf_main_stdio.c \
    $(top_src)/SFZUTF/src/sfzutf_interactive.c \
    $(top_src)/SFZUTF/src/sfzutf-utils.c

if ENABLE_STACK
libsfzutf_a_SOURCES += $(top_src)/SFZUTF/src/sfzutf-stack.c
endif
if ENABLE_HEAP
libsfzutf_a_SOURCES += $(top_src)/SFZUTF/src/sfzutf-heap.c
endif
if ENABLE_PERF
libsfzutf_a_SOURCES += $(top_src)/SFZUTF/src/sfzutf-perf.c
endif


# Framework is required by rng-health
libfmwk_sb_a_CPPFLAGS = \
    -I$(top_src)/Framework/PUBDEFS/incl \
    -I$(top_src)/Framework/IMPLDEFS/incl \
    -I$(top_src)/Framework/SPAL_API/incl \
    -I$(top_src)/Framework/CLIB/incl \
    -I$(top_src)/Kit/DriverFramework/v4/CLib_Abstraction/incl \
    -D_POSIX_SOURCE \
    -D_POSIX_C_SOURCE=199309L \
    -D_XOPEN_SOURCE=600 \
    $(CONFIGURATION_INCLUDES)

if ENABLE_HEAP
libfmwk_sb_a_CPPFLAGS += \
    -I$(top_src)/SFZUTF/incl
endif

libfmwk_sb_a_SOURCES = \
    $(top_src)/Framework/SPAL_MEMORY_ANSI/src/spal_memory_ansi.c \
    $(top_src)/Framework/SPAL_POSIX/src/spal_posix_mutex.c \
    $(top_src)/Framework/SPAL_POSIX/src/spal_posix_semaphore.c \
    $(top_src)/Framework/SPAL_POSIX/src/spal_posix_thread.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/Framework/DEBUG/src/debug_abort.c \
    $(top_src)/Framework/DEBUG/src/debug_printf.c

#----------------------------------------------------------------------------
# Clean up
#----------------------------------------------------------------------------

distclean-local:
	rm -f aclocal.m4 compile configure install-sh Makefile.in missing depcomp config.guess config.sub
	rm -rf autom4te.cache
	rm -f *.gcno *.gcda *.xml

# end of file Makefile.am
