/*
 * ping.h
 *
 *  Created on: Oct 17, 2020
 *      Author: mattg1
 */
#ifndef PING_H_
#define PING_H_

void ping_init(void);
void ping_trigger(void);
float ping_getDistance(void);
void ping_handler(void);
volatile int pingInterruptStatus();
void resetPingIS();
float ping_getDeltaRaw();
void callPing(void);

#endif /* PING_H_ */
