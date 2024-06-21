package CommonUtilsJava;

import InuCommon.*;


class Test {
	public static void main(String[] args) 
	{
	System.load("D:/Projects/Inuitive/Development/trunk/bin/android_ndk10d_android-21-arm64-v8a/libCommonUtilsJNI.so");
	InuModelDB modelDB = new InuModelDB();
	modelDB.init();
	}

}
