/**
 * @file main.h
 *
 * Base application include file
 *
 * \date 15. 9. 2013
 * \author Tomas Bures <bures@d3s.mff.cuni.cz>
 * \author Vladimír Matěna <vlada@mattty.cz>
 */

#ifndef MAIN_H
#define MAIN_H

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


#endif // MAIN_H
