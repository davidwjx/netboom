#if 0 && defined(_WIN32)
#include <tchar.h>
#include <Windows.h>
#else
#define _T(_x) _x
#define _ftprintf fprintf
#define _tcschr strchr
#define _tcsrchr strrchr
#define LPCTSTR const char *
#define LPTSTR char *
#define TCHAR char
#endif

/*
 * egetopt: get option letter from argument vector (an extended
 *      version of getopt).
 *
 * Non standard additions to the ostr specs are:
 * 1) '?': immediate value following arg is optional (no white space
 *    between the arg and the value)
 * 2) '#': +/- followed by a number (with an optional sign but
 *    no white space between the arg and the number). The - may be
 *    combined with other options, but the + cannot.
 */

extern "C" int eoptopt;
extern "C" int egetopt(int nargc, LPTSTR const *nargv, const TCHAR *ostr);
extern "C" LPTSTR eoptarg;
extern "C" int eoptind;
