/*
 * Copyright 2022 IMDT
 * All rights reserved.
 *
 */
// NuSerial Protocol v1.0.
//
// NuSerial is a lightweight UART communication protocol for exchanging information from a NU4000/NU4100 to another NU4000/NU4100 or
// between other MCUs.
// It is designed to be configured as reliable (requires receiver to ACK) or not requiring ACKs. NuSerial in its current implementation
// When initialised waits for the other side to initialise the UART system and start the communication.
// NuSerial framework once initialised runs in the background as long as the NU4000/NU4100 is up and running.
#ifndef NUSERIAL_H
#define NUSERIAL_H
#define SYNC_TIME_OUT 2E6
#define SYNC_PERIOD SYNC_TIME_OUT/4 /*Try to send the heartbeat 4 times faster than the Sync timeout*/
#include <stdint.h>
/*Give C linkage for all the C++ functions*/
#ifdef __cplusplus
extern "C" {
#endif

typedef void (*nuserial_receive_handler_t)(uint16_t msgID, void *buf, uint16_t readLen);
typedef void (*nuserial_write_notify_t)(uint16_t msgID, int32_t retVal);

/**
 * @brief NuSerial communication protocol init
 * @param recvHandler when a valid Frame arrives, the recvHandler callback function will be
 *                    executed from the NuSerial UART communication Protocol Framework.
 * @return 0 means success, -1 means failed
 */ 
int nuserial_init(nuserial_receive_handler_t recvHandler);

/**
 * @brief NuSerial communication protocol send function. Sends the given payload directly on the wire
 *        without queueing.
 * @param msgID frame identifier, to distinctly identify the message type.
 * @param payload frame data
 * @param payloadLen data length
 * @return 0 means success, -1 means failed
 */ 
int nuserial_send(uint16_t msgID, void *payload, uint16_t payloadLen);

/**
 * @brief NuSerial communication protocol queue before sending. This API queues the data to be transmitted
 *  when the transmitter is available, helpful when dealing with several frames to be sent.
 * @param msgID frame identifier, to distinctly identify the message type.
 * @param framePayload frame data
 * @param payloadLen data length
 * @param notifyFunc Call back function that will be called when the queued up message has been transmitted successfully/failed.
 * @return 0 means success, -1 general error, --2 queue full, invalid arguments
 */ 
int nuserial_queue_frame(uint16_t msgID, void *framePayload, uint16_t payloadLen, nuserial_write_notify_t notifyFunc);

/**
 * @brief NuSerial communication protocol initialisation status.
 * @return false means initialising, true initialised
 */ 
bool nuserial_is_init_complete(void);
/**
 * @brief Returns if the connection is healthy
 * 
 * 
 * 
 * @return Returns true if the connection is healthy (sync period has been received within timeout period)
 */
bool nuserial_connection_healthy(void);
/**
 * @brief Gets UART Debug information for the NU4K UART Interconnect
 * @param transmitCounter Transmit counter
 * @param receiveCounter Receive counter
 * @param crcErrors     Number of CRC errors
 * @param transmitTimeNs  How long it takes to transmit in nanoseconds
 * @param lastRxSyncTimeUsec  Last time a sync packget was received
 * @return 
 */
bool nuserial_getUartDebugInformation(UINT64 *transmitCounter, 
    UINT64 *receiveCounter, 
    UINT64 *crcErrors, 
    UINT64 *transmitTimeNs,
    UINT64 *lastRxSyncTimeUsec);
#endif // NUSERIAL_H
#ifdef __cplusplus
}
#endif
