# =============================================================================
# Copyright 2020  Synopsys, Inc.
# This file and the associated documentation are proprietary to Synopsys,
# Inc., and may only be used in accordance with the terms and conditions of
# a written license agreement with Synopsys, Inc.
# Notwithstanding contrary terms in the DFPUC, Licensee may provide the
# binaries of the EV Runtime and Utilities Option to its end-customer that
# purchase Licensee ICs that incorporate the Synopsys EV processor core,
# subject to confidentiality terms no less restrictive than those contained in
# the DFPUC.  All other use, reproduction, or distribution of this file
# is strictly prohibited.
# =============================================================================

cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# 1. SET_ARG
init_variable(NUMTESTS1 "${NUM_DATA_SETS1}")
init_variable(SET_ARG1 " -image_test ${NUM_DATA_SETS1} ${NUMTESTS1}")

init_variable(NUMTESTS2 "${NUM_DATA_SETS2}")
init_variable(SET_ARG2 " -image_test ${NUM_DATA_SETS2} ${NUMTESTS2}")

# 2. NN Binaries
set(CNN_BIN_ADDR1       "${CNN_BIN_ADDR}")
math(EXPR CNN_BIN_ADDR2 "${CNN_BIN_ADDR1} + 0x8000000" OUTPUT_FORMAT HEXADECIMAL)

init_variable(CNN_BIN_FILE_ARG1 "-binaddr ${CNN_BIN_ADDR1} ${CNN_BIN1_FILE_ARG}")
init_variable(CNN_BIN_FILE_ARG2 "-binaddr ${CNN_BIN_ADDR2} ${CNN_BIN2_FILE_ARG}")

# 3. XARGS
set(XARGS "$ENV{XARGS}")
set(XARGS "${XARGS} ${EXTRA_XARGS}")

set(RUN_TEST_ARGS "    ${SET_ARG1} ${CNN_BIN_FILE_ARG1} ${ITEST1} ${XARGS} \
                    -g2 ${SET_ARG2} ${CNN_BIN_FILE_ARG2} ${ITEST2} ${XARGS}")

#[[
message(STATUS "SET_ARG1         = ${SET_ARG1} == SET_ARG2 = ${SET_ARG2}")
message(STATUS "ITEST1           = ${ITEST1} == ITEST2  = ${ITEST2}")

message(STATUS "BIN1ADDR1         = ${BIN1ADDR1}")
message(STATUS "CNN_BIN_FILE1     = ${CNN_BIN_FILE1}")
message(STATUS "CNN_BIN_FILE_ARG1 = ${CNN_BIN_FILE_ARG1}")

message(STATUS "BIN1ADDR2         = ${BIN1ADDR2}")
message(STATUS "CNN_BIN_FILE2     = ${CNN_BIN_FILE2}")
message(STATUS "CNN_BIN_FILE_ARG2 = ${CNN_BIN_FILE_ARG2}")
message(STATUS "XARGS             = ${XARGS}")
message(STATUS "RUN_TEST_ARGS     = ${RUN_TEST_ARGS}")
#]]

set(CNN_OBJ_MULTI_PATH "${CNN_OBJ_PATH}/${GRAPH1_NAME} ${CNN_OBJ_PATH}/${GRAPH2_NAME}")

run(RUN_DIR ${CMAKE_CURRENT_BINARY_DIR} "${RUN_TEST_ARGS}")
