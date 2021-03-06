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
#include "Button.h"
#include "Timer.h"
#include "Console.h"

#include "cdeeco/CDEECo.h"

// Hardware
extern Timer delayTimer;
extern Button toggleButton;
extern UART uartGPS;
extern UART uartSerial;

#endif // MAIN_H
