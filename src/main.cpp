/*
 * main.cpp
 *
 *  Created on: 15. 9. 2013
 *      Author: Tomas Bures <bures@d3s.mff.cuni.cz>
 */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "main.h"

#include "test/TestComponent.h"

#include <cstdio>
#include <sstream>
#include <string>

#include "UART.h"

using namespace std;

// Initialize system console
#include "console.h"
#ifdef CONSOLE_LCD
GMD1602 Console::lcd(GPIOE, RCC_AHB1Periph_GPIOE);
char Console::stored[20];
#endif
#ifdef CONSOLE_SERIAL
UART::Properties serialProps {
GPIOA, USART2,
GPIO_Pin_2, GPIO_Pin_3, GPIO_PinSource2, GPIO_PinSource3, RCC_APB1PeriphClockCmd, RCC_AHB1Periph_GPIOA,
		RCC_APB1Periph_USART2, GPIO_AF_USART2, USART2_IRQn, 921600 };
UART Console::serial(serialProps);
#endif

// Initialize delay timer
Timer::Properties tim6Props {
TIM6, RCC_APB1PeriphClockCmd, RCC_APB1Periph_TIM6, TIM6_DAC_IRQn };
Timer delayTimer(tim6Props);

/** System startup function */
int main(void) {
	delayTimer.setPriority(1, 1);
	delayTimer.init();
	Console::init();

	Console::log("\n\n\n\n\n\n\n\n\n\n# # # # # # # # # # # # # # # # # # # #\n # # # # # # # # # # # # # # # # # # # \n# # # # # # # # # # # # # # # # # # # #\n>>> SYSTEM INIT <<<");

	Console::log(">>> About to construct test component");

	new TestComponent();

	Console::log(">>> Running scheduler");

	/* Start the scheduler. */
	vTaskStartScheduler();
	Console::log(">>> End reached - THIS SHOULD NOT HAPPEN !!!!");
	while (1) {}
}

// FreeRTOS System error handlers
extern "C" {
	void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName) {
		Console::log("STACK OVERFLOW!!");
		while (1) {}
	}

	void vApplicationMallocFailedHook(void) {
		Console::log("MALLOC FAILED!!!");
		while (1) {}
	}
}
