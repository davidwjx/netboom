#include "metadata_updater.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define SENSOR_UPDATE_MESSAGE 0x01
#define MESSAGE_COUNTER_MASK 0xFF00
typedef struct
{
    int64_t messagesSent;
    int64_t messagesFailed;
    uint8_t currentMessageCounter;
    uint8_t previousMessageCounter;
    int64_t messagesReceived;
} METADATA_UART_sensorUpdateStats;
/**
 * @brief Repeats a sensor update from the master to the slave
 * 
 * @param buffer Buffer to repeat from the master to the slave
 * @param size Size of the buffer
 * @return ERRG_codeE Returns an error code
 */
ERRG_codeE METADATA_UART_repeatSensorUpdateFromMasterToSlave(char * buffer, size_t size);
/**
 * @brief Initialises the Metadata UART module
 * 
 * @return ERRG_codeE ERRG_codeE Returns an error code
 */
ERRG_codeE METADATA_UART_init();

#ifdef __cplusplus
}
#endif