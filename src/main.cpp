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
#include "drivers/UART.h"
#include "drivers/SHT1x.h"
#include "cdeeco/KnowledgeCache.h"
#include "drivers/LED.h"

#include "test/TestComponent.h"
#include "test/Thermometer.h"
#include "test/Alarm.h"
#include "test/TempExchange.h"

#include <cstdio>
#include <sstream>
#include <string>
#include <signal.h>

using namespace std;

// Initialize system console
#include "Console.h"

// Initialize delay timer
Timer::Properties tim6Props {
TIM6, RCC_APB1PeriphClockCmd, RCC_APB1Periph_TIM6, TIM6_DAC_IRQn };
Timer delayTimer(tim6Props);

PulseLED::Properties pulseProps {
	RCC_APB1Periph_TIM7, TIM7, TIM7_IRQn, 6, 3
};

/**
 * Interrupt priority map
 *
 * HIGHEST
 * 0 - MRF SPI
 * 1 ------------------- FreeRTOS critical section
 * 1 - MRF RF
 * 2 - System scheduler
 * 3 - UART - Console
 * 7 - TIM7 - Pulse LED tick
 *
 *
 */


/** System startup function */
int main(void) {
	// Initialize basic system hardware
	delayTimer.init();
	Console::init();
	PulseLED::initTimer(pulseProps);

	Console::log("\n\n\n\n\n\n\n\n\n\n");
	Console::log("# # # # # # # # # # # # # # # # # # # #");
	Console::log(" # # # # # # # # # # # # # # # # # # #");
	Console::log("# # # # # # # # # # # # # # # # # # # #");
	Console::log("\n>>> SYSTEM INIT <<<");

	Console::log(">>> Waiting 3s for debugger to stop me...");
	delayTimer.mDelay(3000);
	Console::log(">>> Starting system");

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	System *system = new System();

	// Test component
	new TestComponent(*system);

	// Temperature monitoring system
	new Thermometer::Component(*system, 1);
	Alarm::Component* alarm = new Alarm::Component(*system, 1);
	KnowledgeCache<Thermometer::Component::Type, Thermometer::Knowledge, 10>* cache = new KnowledgeCache<
			Thermometer::Component::Type, Thermometer::Knowledge, 10>();
	system->registerCache(cache);
	new TempExchange::Ensamble(*alarm, *cache);

	// Start the scheduler.
	Console::log(">>> Running scheduler");
	vTaskStartScheduler();

	// This should not be reached
	Console::log(">>> End reached - THIS SHOULD NOT HAPPEN !!!!");
	while(1) {
	}
}

// FreeRTOS System error handlers
extern "C" {
	void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName) {
		Console::log("STACK OVERFLOW!!");
		while(1) {
		}
	}

	void vApplicationMallocFailedHook(void) {
		Console::log("MALLOC FAILED!!!");
		while(1) {
		}
	}
}

// GCC 4.9.0 fix ssp by dummy ssp
extern "C" {
	void __stack_chk_fail() {
	}
	bool __stack_chk_guard() {
		return true;
	}
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line) {
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	Console::log("Assert failed");
	//Console::log("Assert: %s:%d", file, line);

	/* Infinite loop */
	while(1) {
	}
}
#endif

