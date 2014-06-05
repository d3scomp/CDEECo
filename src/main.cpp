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
#include "test/PortableSensor.h"
#include "test/Alarm.h"
#include "test/TempExchange.h"

#include <cstdio>
#include <sstream>
#include <string>
#include <signal.h>

// Initialize system console
#include "Console.h"

// Initialize delay timer
Timer::Properties tim6Props {
TIM6, RCC_APB1PeriphClockCmd, RCC_APB1Periph_TIM6, TIM6_DAC_IRQn };
Timer delayTimer(tim6Props);

PulseLED::Properties pulseProps {
RCC_APB1Periph_TIM7, TIM7, TIM7_IRQn, 6, 0 };

Button::Properties userButtonProps {
GPIOA, GPIO_Pin_0, RCC_AHB1Periph_GPIOA, EXTI_Line0, EXTI_PortSourceGPIOA, EXTI_PinSource0, EXTI0_IRQn };
Button toggleButton(userButtonProps);

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
 * 8 - user button
 *
 */

/**
 * Enable VFP unit, taken from FreeRTOS port
 */
static void enableVFP(void) {
	__asm volatile ("ldr.w r0, =0xE000ED88");
	// The FPU enable bits are in the CPACR.
	__asm volatile ("ldr r1, [r0]");
	__asm volatile ("orr r1, r1, #( 0xf << 20 )");
	// Enable CP10 and CP11 co-processors, then save back.
	__asm volatile ("str r1, [r0]");
	__asm volatile ("bx r14");
}

TickType_t lastUserPress;
void userPressed(void* data) {
	TickType_t now = xTaskGetTickCount();
	if((now - lastUserPress) > portTICK_PERIOD_MS * 10)
		Console::toggleLevel();
	lastUserPress = now;
}

/* void pulseLedTimerCallbackFunction( TimerHandle_t xTimer ) {
 PulseLED::tickInterruptHandler();
 }*/

void cdeecoSetup(const uint32_t uniqId) {
	//// System setup
	auto system = new CDEECO::System(uniqId);

	// Test component
	new TestComponent(*system);

	///// Temperature monitoring system
	// Components
	auto sensor = new Sensor::Component(*system);
	auto alarm = new Alarm::Component(*system);

	// Caches
	auto sensorCache = new KnowledgeCache<Sensor::Component::Type, Sensor::Knowledge, 10>();
	auto alarmCache = new KnowledgeCache<Alarm::Component::Type, Alarm::Knowledge, 10>();
	system->registerCache(sensorCache);
	system->registerCache(alarmCache);

	// Ensembles
	new TempExchange::Ensamble(*alarm, *sensorCache);
	new TempExchange::Ensamble(*sensor, *alarmCache);
}

/** System startup function */
int main(void) {
	// Initialize basic system hardware
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	enableVFP();
	delayTimer.init();
	Console::init();
	PulseLED::initTimer(pulseProps);
	toggleButton.setPriority(8, 0);
	toggleButton.setPressedListener(userPressed, NULL);
	toggleButton.init();

	Console::log("\n\n\n\n\n\n\n\n\n\n\n");
	Console::log("# # # # # # # # # # # # # # # # # # # #\n");
	Console::log(" # # # # # # # # # # # # # # # # # # #\n");
	Console::log("# # # # # # # # # # # # # # # # # # # #\n");
	Console::log("\n>>> SYSTEM INIT <<<\n");

	Console::log(">>> Waiting 3s for debugger to stop me...\n");
	delayTimer.mDelay(3000);
	Console::log(">>> Starting system\n");

	// Get unique device id
	const uint32_t uniqId = *((uint32_t*) 0x1FFF7A10);
	Console::log("\n\n>>>>> Unique system Id: %x <<<<<<\n\n\n", uniqId);

	cdeecoSetup(uniqId);

	/*	// TODO: This is not nice
	 Console::log(">>> Setting timer to make pulse leds work");
	 TimerHandle_t pulseLedTimerhandle = xTimerCreate("PulseLedTimer", 100 / portTICK_PERIOD_MS, pdTRUE, 0, pulseLedTimerCallbackFunction);
	 xTimerStart(pulseLedTimerhandle, 10);*/

	// Start the scheduler.
	Console::log(">>> Running scheduler\n");
	vTaskStartScheduler();

	// This should not be reached
	Console::log(">>> End reached - THIS SHOULD NOT HAPPEN !!!!\n");
	assert_param(false);
}

// FreeRTOS System error handlers
extern "C" {
	void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName) {
		Console::log("STACK OVERFLOW!!\n");
		assert_param(false);
	}

	void vApplicationMallocFailedHook(void) {
		Console::log("MALLOC FAILED!!!\n");
		assert_param(false);
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
	// User can add his own implementation to report the file name and line number,
	Console::print(Error, "\n\n\n#### Assert failed ####\nFile: %s:%d\n\n\n", file, line);

	/* Infinite loop */
	while(1) {
	}
}
#endif

