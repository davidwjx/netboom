securebootsim: SBLib/build_linux
	cd SBCommon && $(MAKE)
	cd $< && $(MAKE) $(AM_MAKEFLAGS) securebootsim
	cp $</securebootsim $@


