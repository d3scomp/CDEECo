/*
 * main.h
 *
 *  Created on: 15. 9. 2013
 *      Author: Tomas Bures <bures@d3s.mff.cuni.cz>
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "stm32f4xx.h"

#include "UART.h"
#include "LED.h"
#include "Button.h"
#include "Timer.h"
#include "GPS.h"
#include "MRF24J40.h"
#include "Console.h"

// Hardware
extern Timer delayTimer;
extern Button toggleButton;
extern UART uartGPS;
extern GPSL30 gps;
extern LED blueLED;
extern LED orangeLED;
extern MRF24J40 mrf;
extern UART uartSerial;
extern Console console;


#endif /* MAIN_H_ */
