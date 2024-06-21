#ifndef _MIPI_DEMO_H_
#define _MIPI_DEMO_H_

//Initialize MIPI TX - for demo evertying is fixed.
void MIPI_TXG_initialize();
//This will register a callback to transmit frames receieved from dma on the MIPI TX
void MIPI_TXG_start();
//This will unregister the transmit callback
void MIPI_TXG_stop();
//Send a single buffer on the MIPI TX
int MIPI_TXG_transmit(MEM_POOLG_bufDescT *bufDescP);


#endif
