##
## File: Makefile.am
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

SUBDIRS=@SAFEZONE_SUBDIRS@

# Following line shall be replaced with rules from meta/*.mk
@SAFEZONE_RULES@

# Support run_check
run_check:
	@RET=0; \
	for DIR in $(SUBDIRS);do \
		(cd $$DIR;make -n run_check >/dev/null 2>/dev/null); \
		if [ "X$$?" == "X0" ]; then \
			(cd $$DIR;make run_check);\
			NEWRET=$$?; \
			if [ "X$$RET" == "X0" ]; then \
				RET=$$NEWRET; \
			fi; \
		fi; \
	done

distclean-local:
	rm -f aclocal.m4 compile configure install-sh Makefile.in missing depcomp config.guess config.sub
	rm -rf autom4te.cache
	rm -f meta/combined.rules

# end of file Makefile.am
