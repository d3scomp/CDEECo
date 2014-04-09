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

using namespace std;

#include "console.h"
GMD1602 Console::lcd(GPIOE, RCC_AHB1Periph_GPIOE);
char Console::stored[20];


Timer::Properties tim6Props {
	TIM6, RCC_APB1PeriphClockCmd, RCC_APB1Periph_TIM6, TIM6_DAC_IRQn
};
Timer delayTimer(tim6Props);

/** System startup function */
int main(void) {
	delayTimer.setPriority(1,1);
	delayTimer.init();
	Console::init();

	Console::log("TestComponent..");

	new TestComponent();

	Console::log("Running schedule");

	/* Start the scheduler. */
	vTaskStartScheduler();
	Console::log("End reached!!!!!");
	while(1);
}

// FreeRTOS System error handlers
extern "C" {
	void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName ) {
		Console::log("STACK OVERFLOW!!");
		while(1);
	}

	void vApplicationMallocFailedHook(void) {
		Console::log("MALLOC FAILED!!!");
		while(1);
	}
}
