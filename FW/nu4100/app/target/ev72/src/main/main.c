/****************************************************************************
 *
 *   FileName: main.c
 *
 *   Author:  Noam P.
 *
 *   Date: 
 *
 *   Description: 
 *   
 ****************************************************************************/

//#include <stdio.h>
#include "evthreads.h"
#include "log.h"
#include "ev_init.h"
#include "inu_cnn_pipe_api.h"

#define  MAIL_BOX_DONE                    0x02001000 
#define  MAIL_BOX_ERROR                   0x02001004

int main(int argc, char **argv)
{
	 //_sr(0x08000002, 0x5E);
	volatile int *mail_box_done  = (volatile int *)MAIL_BOX_DONE;
	volatile int *mail_box_error = (volatile int *)MAIL_BOX_ERROR;

	*mail_box_done = 0x1;
	*mail_box_error = 0x0;
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "Start main! %d CPUs\n", evGetMaxCpu());

    EV_INITG_init();

    return 0;
}

