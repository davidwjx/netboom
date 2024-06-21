#ifdef __ANDROID__
#include <android/log.h>

#define  LOG_TAG    "someTag"

#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

#else
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <sstream>

class LinuxLogger
{
	public:

	static void write(std::string text)
	{
		std::cout << text << std::endl;
	}



	static void formatAndWrite(char *fmt, ...)
	{
		va_list ap;
		int d;
		char *s;
		std::stringstream buffer;

		va_start(ap, fmt);
		while (*fmt)
			switch (*fmt++) {
				case 's':              /* string */
				s = va_arg(ap, char *);	buffer << s; break;

				case 'd':              /* int */
				d = va_arg(ap, int); buffer << d; break;
			}

		va_end(ap);
		std::cout << buffer.str() << std::endl;
	}
};

#define  LOGE(a) LinuxLogger::write(a)
#define  LOGW(...)
#define  LOGD(...)
#define  LOGI(...)

#endif
