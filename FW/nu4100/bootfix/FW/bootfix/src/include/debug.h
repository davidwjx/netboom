#ifndef __DEBUG_H__
#define __DEBUG_H__

#define DEBUG_INFO        1
#define DEBUG_LOUD        2
#define DEBUG_VERY_LOUD   4
#define DEBUG_BUFFER   	  8
#define DEBUG_ALL   	  0xffffffff

/*
 * Local Functions
 *
 */
/* Use this function for simulation debug */
void debug_code(unsigned int code);
/* printf implementation for debug output */
int debug_printf(const char *fmt_str, ...);

void debug_buffer_enable(int enable);
/* flushes printf buffer */
void debug_flush_buffer();
void debug_check_for_flush();

/*
 *  Public Functions
 */

/* Use this function to init proper debug method configured via #defines*/
void debug_init(int);

/* Use this function to print debug message  with verbosity*/
#define debug_log(level, fmt,  ...) \
	do { \
		if (level > BOOTROM_DEBUG_LEVEL) \
			break; \
		debug_printf(fmt, ##__VA_ARGS__); \
	} while(0)


/* Use this function to print error message with _LINE_ and _FILE_ */
#define error_log(level, fmt, ...) \
	do { \
		debug_log(level, "%s:%d(): " fmt, \
		__func__, __LINE__, ##__VA_ARGS__); \
	} while (0)

/* Use this functions during debug */
#if BOOTROM_DEBUG_LEVEL > 0
	#define   dbg_log(level, fmt, ...) debug_log(level,fmt, ## __VA_ARGS__)
	#define   err_log(level, fmt, ...) error_log(level,fmt, ##__VA_ARGS__)
	#define   dbg_code(code) debug_code(code)

#else
	#define dbg_log(level, ...)
	#define err_log(level, ...)
	#define dbg_code(code)
#endif

/* Use this function to print release messages */
#define rel_log(fmt, ...)  \
	do { \
		debug_printf(fmt, ##__VA_ARGS__); \
	}while(0)

/* Use this function to print assert message and finish execution */
#define abort_log(fmt, ...) \
   	do { \
   		debug_printf("%s:%d(): "fmt, __func__,  __LINE__, ##__VA_ARGS__); \
   		while(1); \
   	}while(0)
   	
void board_end(unsigned int);

#endif /*#ifndef __DEBUG_H__ */
