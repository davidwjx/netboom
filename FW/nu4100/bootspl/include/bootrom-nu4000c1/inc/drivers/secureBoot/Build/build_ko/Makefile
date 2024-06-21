#############################################################################
#
# Copyright (c) 2011-2018 INSIDE Secure B.V. All Rights Reserved.
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

default: subbuild

subbuild:
	./_subbuild.sh

clean:
	rm -f umdevxs_k-*.ko
	make -f pci/Makefile clean
	make -f mem/Makefile clean
	make -f sim/Makefile clean

# note: not using install in sub makefile
install:
	@if [ "x$(SAFEZONE_CM_PATH)" == "x" ]; then \
        echo "FATAL: Please provide SAFEZONE_CM_PATH"; \
    else \
        echo "Installing to $(SAFEZONE_CM_PATH)/sbin"; \
        mkdir -p            $(SAFEZONE_CM_PATH)/sbin; \
        cp umdevxs_k-*.ko   $(SAFEZONE_CM_PATH)/sbin; \
        cp load.sh          $(SAFEZONE_CM_PATH)/sbin; \
        cp unload.sh        $(SAFEZONE_CM_PATH)/sbin; \
        chmod +x            $(SAFEZONE_CM_PATH)/sbin/load.sh; \
        chmod +x            $(SAFEZONE_CM_PATH)/sbin/unload.sh; \
    fi

# end of file
