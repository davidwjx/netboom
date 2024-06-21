
--------------------------------------
NuSerial UART communication Framework
--------------------------------------

Description:
------------
NuSerial is a lightweight UART communication protocol for exchanging information from a NU4000/NU4100 to another NU4000/NU4100.
It is designed to be configured as a reliable protocol that requires receiver to ACK or not requiring ACKs.
NuSerial in its current implementation when initialised, waits for the other side to initialise the UART system and then starts the communication.
NuSerial framework once initialised, runs in the background as long as the NU4000/NU4100 is up and running.

Testing:
-------
A smoke test has been provided in the file nuserial/test/nuserial_test.c
This is a simple test where master sends out a message with MsgIds acting as a counter. The slave when receives the message, forwards the same message
back to master. The receive callback function in master checks the MsgId counter and tests that the counters are correct. Currently the counter runs from
0 to 255.

Usage:
-----
Below is am example sequence of using the NuSerial framework:
1. Application initialises the NuSerial framework using nuserial_init() and provides the receive callback functions as the argument.
   The receive callback function is called when a valid message is received.
2. Application queues up a message to be sent using nuserial_queue_frame().
3. The application doesn't queue the message but instead sends directly to the UART0 using nuserial_send()
4. When a valid message is received, application is notified with the received message through the receive callback function that was provided in nuserial_init().
