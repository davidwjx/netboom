
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "inu_common.h"
#include "nuserial.h"



#define MAX_MSG_ID  255
#define IS_MASTER

static uint8_t writeNotificationReceived = 0;
static bool testResult = true;
static bool testthreadActive = true;

void test_read_func(uint16_t msgID, void *buff, uint16_t len)
{
#if defined(IS_MASTER)
    static uint16_t msgIDIndex= 0;
    if(msgIDIndex != msgID){
        // Expected Msg was not written.
        testResult = false;
        testthreadActive = false;
        LOGG_PRINT(LOG_INFO_E, NULL, "MSG Write failed. Expected MSG ID - %d, received MSGID - %d\n\n", msgIDIndex, msgID);
    }
    msgIDIndex++;
    LOGG_PRINT(LOG_INFO_E, NULL, "MSG ID - %d, MSG - %s, Len - %d\n\n", msgID, buff, len);
#else
    // Echo back the received message to master
    nuserial_send(msgID, buff, len);
#endif
}

void write_complete_notification(uint16_t msgID, int32_t retVal)
{
    static uint16_t msgIDIndex= 0;
    if(msgIDIndex != msgID){
        // Expected Msg was not written.
        testResult = false;
        testthreadActive = false;
        LOGG_PRINT(LOG_INFO_E, NULL, "MSG Write failed. Expected MSG ID - %d, received MSGID - %d\n\n", msgIDIndex, msgID);
    }

    if(retVal != 0){
        // MSg write failed
        testResult = false;
        testthreadActive = false;
        LOGG_PRINT(LOG_INFO_E, NULL, "MSG Write failed. Retval - %d.\n\n", retVal);
    }
    msgIDIndex++;
    writeNotificationReceived = 1;
}

static void* test_thread(void *args) 
{
    char *buff = "Hello from uart0";
    uint16_t MsgID = 0;

    while(!nuserial_is_init_complete())
        sleep(1);

    LOGG_PRINT(LOG_INFO_E, NULL, "NuSerial Init succeeded.\n\n");

    #if defined(IS_MASTER)

        while(testthreadActive){
            writeNotificationReceived = 0;
            nuserial_queue_frame(MsgID++, buff, strlen(buff), write_complete_notification);

            while(!writeNotificationReceived)
                usleep(50); // enable context switch

            if(MsgID >= MAX_MSG_ID)
                testthreadActive = 0;
        }
    #endif
}

void test_nuserial(void)
{
    nuserial_init(test_read_func);
    
    pthread_t pidBG;
#if defined(IS_MASTER)
    LOGG_PRINT(LOG_INFO_E, NULL, "NuSerial Master Init succeeded.\n\n");
#else
    LOGG_PRINT(LOG_INFO_E, NULL, "NuSerial Slave Init succeeded.\n\n");
#endif
    pthread_create(&pidBG, NULL, test_thread, NULL);
}