#############################################################################
#
# Copyright (c) 2009-2018 INSIDE Secure B.V. All Rights Reserved.
#
# This confidential and proprietary software may be used only as authorized
# by a licensing agreement from INSIDE Secure.
#
# The entire notice above must be reproduced on all authorized copies that
# may only be made to the extent permitted by a licensing agreement from
# INSIDE Secure.
#
# For more information or support, please go to our online support system at
# https://customersupport.insidesecure.com.
# In case you do not have an account for this system, please send an e-mail
# to ESSEmbeddedHW-Support@insidesecure.com.
#
##############################################################################

#
# Makefile for UMDevXS 'PeekPoke' test application
#

BIN_DIR:=$(SAFEZONE_CM_PATH)/bin

PEEKPOKE_SOURCES = \
    ../../Integration/UMDevXS/PeekPoke/src/umdevxs_peekpoke.c \
    ../../Integration/UMDevXS/UserPart/src/umdevxsproxy.c

default: $(PEEKPOKE_SOURCES)
	$(CC) \
    -o umdevxs_peekpoke \
    -I. \
    -I../../Integration/UMDevXS/UserPart/incl \
    -I../../Integration/UMDevXS/KernelPart/incl \
    $(PEEKPOKE_SOURCES) \
    -pthread

clean:
	rm -f umdevxs_peekpoke

install:
	@if [ "x$(SAFEZONE_CM_PATH)" == "x" ]; then \
        echo "FATAL: Please provide SAFEZONE_CM_PATH"; \
    else \
        echo "Installing to $(BIN_DIR)"; \
        mkdir -p $(BIN_DIR); \
        cp umdevxs_peekpoke $(BIN_DIR); \
        chmod +x $(BIN_DIR)/umdevxs_peekpoke; \
        ln -s -f $(BIN_DIR)/umdevxs_peekpoke $(BIN_DIR)/pp; \
        ls -l $(BIN_DIR); \
    fi

# end of file
