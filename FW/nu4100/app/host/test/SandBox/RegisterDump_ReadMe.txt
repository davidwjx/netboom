1. The config file should be put in the same directory where SandBoxFW.exe(inu_sandbox) locates
   The config file format decription is like below:
	BLOCK:block_name
	[Register Descriptor]

	block_name: AXI_READER,AXI_WRITER,GME,CDE,SLU,IAE,PPE,CVA,SENSOR,DPHY
	Register Descriptor:
		if BLOCK is SENSOR, then the register string format is:
			[RegisterName,I2CBusNum,I2CDevAddr,AddrSize,RegStartAddr,RegEndAddr,Description]
		if BLOCK is DPHY, then the register string format is:
			[RegisterName,RegAddr,RegCount,Description]
		if BLOCK is not SENSOR or DPHY, then the register string format is:
			[RegisterName,RegAddr,RegCount,Description]
2. The example config file is hw_path_reg_dump.cfg
3. SandBox runtime command: D(or debug_registers) config_file output_file
	config_file: register dump config file
	output_file: register dump output file
	e.g: D hw_path_reg_dump.cfg dump1
	     After the command, the result is saved in the file "dump1"
        