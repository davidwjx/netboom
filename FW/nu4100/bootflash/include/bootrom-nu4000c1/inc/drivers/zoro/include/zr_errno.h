/**
	@file   zr_errno.h

	@brief  List of error codes, combination of various
	                Linux error codes. The right way to use the codes below
	                is to return the negative value of them.

	@date 2010

	@author Uri Shkolnik, Zoro Solutions Ltd

	<b> Copyright (c) 2010-2013 Zoro Solutions Ltd. </b>\n
	43 Hamelacha street, P.O. Box 8786, Poleg Industrial Park, Netanaya, ZIP 42505 Israel\n
	All rights reserved\n\n
	Proprietary rights of Zoro Solutions Ltd are involved in the
	subject matter of this material. All manufacturing, reproduction,
	use, and sales rights pertaining to this subject matter are governed
	by the license agreement. The recipient of this software implicitly
	accepts the terms of the license. This source code is the unpublished
	property and trade secret of Zoro Solutions Ltd.
	It is to be utilized solely under license from Zoro Solutions Ltd and it
	is to be maintained on a confidential basis for internal company use
	only. It is to be protected from disclosure to unauthorized parties,
	both within the Licensee company and outside, in a manner not less stringent
	than that utilized for Licensee's own proprietary internal information.
	No copies of the source or object code are to leave the premises of
	Licensee's business except in strict accordance with the license
	agreement signed by Licensee with Zoro Solutions Ltd.\n\n

	For more details - http://zoro-sw.com
	email: info@zoro-sw.com
*/



#ifndef _ZR_ERRNO_H_
#define _ZR_ERRNO_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Note
	The right way to use the codes below is to
	return the negative value of them
*/

#define EPERM            1
//!< Operation not permitted
#define ENOENT           2
//!< No such file or directory
#define ESRCH            3
//!< No such process
#define EINTR            4
//!< Interrupted system call
#define EIO              5
//!< I/O error
#define ENXIO            6
//!< No such device or address
#define E2BIG            7
//!< Argument list too long
#define ENOEXEC          8
//!< Exec format error
#define EBADF            9
//!< Bad file number
#define ECHILD          10
//!< No child processes
#define EAGAIN          11
//!< Try again
#define ENOMEM          12
//!< Out of memory
#define EACCES          13
//!< Permission denied
#define EFAULT          14
//!< Bad address
#define ENOTBLK         15
//!< Block device required
#define EBUSY           16
//!< Device or resource busy
#define EEXIST          17
//!< File exists
#define EXDEV           18
//!< Cross-device link
#define ENODEV          19
//!< No such device
#define ENOTDIR         20
//!< Not a directory
#define EISDIR          21
//!< Is a directory
#define EINVAL          22
//!< Invalid argument
#define ENFILE          23
//!< File table overflow
#define EMFILE          24
//!< Too many open files
#define ENOTTY          25
//!< Not a typewriter
#define ETXTBSY         26
//!< Text file busy
#define EFBIG           27
//!< File too large
#define ENOSPC          28
//!< No space left on device
#define ESPIPE          29
//!< Illegal seek
#define EROFS           30
//!< Read-only file system
#define EMLINK          31
//!< Too many links
#define EPIPE           32
//!< Broken pipe
#define EDOM            33
//!< Math argument out of domain of func
#define ERANGE          34
//!< Math result not representable
#define EDEADLK         35
//!< Resource deadlock would occur
#define ENAMETOOLONG    36
//!< File name too long
#define ENOLCK          37
//!< No record locks available
#define ENOSYS          38
//!< Function not implemented
#define ENOTEMPTY       39
//!< Directory not empty
#define ELOOP           40
//!< Too many symbolic links encountered
#define EWOULDBLOCK     EAGAIN
//!< Operation would block
#define ENOMSG          42
//!< No message of desired type
#define EIDRM           43
//!< Identifier removed
#define ECHRNG          44
//!< Channel number out of range
#define EL2NSYNC        45
//!< Level 2 not synchronized
#define EL3HLT          46
//!< Level 3 halted
#define EL3RST          47
//!< Level 3 reset
#define ELNRNG          48
//!< Link number out of range
#define EUNATCH         49
//!< Protocol driver not attached
#define ENOCSI          50
//!< No CSI structure available
#define EL2HLT          51
//!< Level 2 halted
#define EBADE           52
//!< Invalid exchange
#define EBADR           53
//!< Invalid request descriptor
#define EXFULL          54
//!< Exchange full
#define ENOANO          55
//!< No anode
#define EBADRQC         56
//!< Invalid request code
#define EBADSLT         57
//!< Invalid slot
#define EDEADLOCK       EDEADLK
//!< See @ref EDEADLK
#define EBFONT          59
//!< Bad font file format
#define ENOSTR          60
//!< Device not a stream
#define ENODATA         61
//!< No data available
#define ETIME           62
//!< Timer expired
#define ENOSR           63
//!< Out of streams resources
#define ENONET          64
//!< Machine is not on the network
#define ENOPKG          65
//!< Package not installed
#define EREMOTE         66
//!< Object is remote
#define ENOLINK         67
//!< Link has been severed
#define EADV            68
//!< Advertise error
#define ESRMNT          69
//!< Srmount error
#define ECOMM           70
//!< Communication error on send
#define EPROTO          71
//!< Protocol error
#define EMULTIHOP       72
//!< Multihop attempted
#define EDOTDOT         73
//!< RFS specific error
#define EBADMSG         74
//!< Not a data message
#define EOVERFLOW       75
//!< Value too large for defined data type
#define ENOTUNIQ        76
//!< Name not unique on network
#define EBADFD          77
//!< File descriptor in bad state
#define EREMCHG         78
//!< Remote address changed
#define ELIBACC         79
//!< Can not access a needed shared library
#define ELIBBAD         80
//!< Accessing a corrupted shared library
#define ELIBSCN         81
//!< .lib section in a.out corrupted
#define ELIBMAX         82
//!< Attempting to link in too many shared libraries
#define ELIBEXEC        83
//!< Cannot exec a shared library directly
#define EILSEQ          84
//!< Illegal byte sequence
#define ERESTART        85
//!< Interrupted system call should be restarted
#define ESTRPIPE        86
//!< Streams pipe error
#define EUSERS          87
//!< Too many users
#define ENOTSOCK        88
//!< Socket operation on non-socket
#define EDESTADDRREQ    89
//!< Destination address required
#define EMSGSIZE        90
//!< Message too long
#define EPROTOTYPE      91
//!< Protocol wrong type for socket
#define ENOPROTOOPT     92
//!< Protocol not available
#define EPROTONOSUPPORT 93
//!< Protocol not supported
#define ESOCKTNOSUPPORT 94
//!< Socket type not supported
#define EOPNOTSUPP      95
//!< Operation not supported on transport endpoint
#define EPFNOSUPPORT    96
//!< Protocol family not supported
#define EAFNOSUPPORT    97
//!< Address family not supported by protocol
#define EADDRINUSE      98
//!< Address already in use
#define EADDRNOTAVAIL   99
//!< Cannot assign requested address
#define ENETDOWN        100
//!< Network is down
#define ENETUNREACH     101
//!< Network is unreachable
#define ENETRESET       102
//!< Network dropped connection because of reset
#define ECONNABORTED    103
//!< Software caused connection abort
#define ECONNRESET      104
//!< Connection reset by peer
#define ENOBUFS         105
//!< No buffer space available
#define EISCONN         106
//!< Transport endpoint is already connected
#define ENOTCONN        107
//!< Transport endpoint is not connected
#define ESHUTDOWN       108
//!< Cannot send after transport endpoint shutdown
#define ETOOMANYREFS    109
//!< Too many references: cannot splice
#define ETIMEDOUT       110
//!< Connection timed out
#define ECONNREFUSED    111
//!< Connection refused
#define EHOSTDOWN       112
//!< Host is down
#define EHOSTUNREACH    113
//!< No route to host
#define EALREADY        114
//!< Operation already in progress
#define EINPROGRESS     115
//!< Operation now in progress
#define ESTALE          116
//!< Stale NFS file handle
#define EUCLEAN         117
//!< Structure needs cleaning
#define ENOTNAM         118
//!< Not a XENIX named type file
#define ENAVAIL         119
//!< No XENIX semaphores available
#define EISNAM          120
//!< Is a named type file
#define EREMOTEIO       121
//!< Remote I/O error
#define EDQUOT          122
//!< Quota exceeded
#define ENOMEDIUM       123
//!< No medium found
#define EMEDIUMTYPE     124
//!< Wrong medium type
#define ECANCELED       125
//!< Operation Canceled
#define ENOKEY          126
//!< Required key not available
#define EKEYEXPIRED     127
//!< Key has expired
#define EKEYREVOKED     128
//!< Key has been revoked
#define EKEYREJECTED    129
//!< Key was rejected by service
/* for robust mutexes */
#define EOWNERDEAD      130
//!< Owner died
#define ENOTRECOVERABLE 131
//!< State not recoverable
#define ERFKILL         132
//!< Operation not possible due to RF-kill

/*
 * These should never be seen by user programs.  To return one of ERESTART*
 * codes, signal_pending() MUST be set.  Note that ptrace can observe these
 * at syscall exit tracing, but they will never be left for the debugged user
 * process to see.
 */
#define ERESTARTSYS     512
#define ERESTARTNOINTR  513
#define ERESTARTNOHAND  514     /* restart if no handler.. */
#define ENOIOCTLCMD     515     /* No ioctl command */
#define ERESTART_RESTARTBLOCK 516 /* restart by calling sys_restart_syscall */

/* Defined for the NFSv3 protocol */
#define EBADHANDLE      521     /* Illegal NFS file handle */
#define ENOTSYNC        522     /* Update synchronization mismatch */
#define EBADCOOKIE      523     /* Cookie is stale */
#define ENOTSUPP        524     /* Operation is not supported */
#define ETOOSMALL       525     /* Buffer or request is too small */
#define ESERVERFAULT    526     /* An untranslatable error occurred */
#define EBADTYPE        527     /* Type not supported by server */
#define EJUKEBOX        528     /* Request initiated, but will not complete before timeout */
#define EIOCBQUEUED     529     /* iocb queued, will get completion event */
#define EIOCBRETRY      530     /* iocb queued, will trigger a retry */


#if 0
/** @name Synopsis Error Codes */
#define DWC_E_INVALID         1001
#define DWC_E_NO_MEMORY       1002
#define DWC_E_NO_DEVICE       1003
#define DWC_E_NOT_SUPPORTED   1004
#define DWC_E_TIMEOUT         1005
#define DWC_E_BUSY            1006
#define DWC_E_AGAIN           1007
#define DWC_E_RESTART         1008
#define DWC_E_ABORT           1009
#define DWC_E_SHUTDOWN        1010
#define DWC_E_NO_DATA         1011
#define DWC_E_DISCONNECT      2000
#define DWC_E_UNKNOWN         3000
#define DWC_E_NO_STREAM_RES   4001
#define DWC_E_COMMUNICATION   4002
#define DWC_E_OVERFLOW        4003
#define DWC_E_PROTOCOL        4004
#define DWC_E_IN_PROGRESS     4005
#define DWC_E_PIPE            4006
#define DWC_E_IO              4007
#define DWC_E_NO_SPACE        4008
#endif

#define EOTHERERR             0xFFFF

#ifdef __cplusplus
}
#endif
#endif /* _ZR_ERRNO_H_ */
