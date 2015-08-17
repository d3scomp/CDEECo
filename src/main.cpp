/** @file main.cpp
 *
 * Application entry point and CDEECO++ system initialization
 *
 * \date 15. 9. 2013
 * \author Tomas Bures <bures@d3s.mff.cuni.cz>
 * \author Vladimír Matěna <vlada@mattty.cz>
 *
 */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include "main.h"
#include "drivers/StopWatch.h"
#include "drivers/UART.h"
#include "drivers/SHT1x.h"
#include "drivers/LED.h"
#include "drivers/Console.h"

#include "cdeeco/System.h"
#include "cdeeco/KnowledgeCache.h"

#include <cstdio>
#include <sstream>
#include <string>
#include <signal.h>

#include "ics/MrfRadio.h"
#include "test/Alarm.h"
#include "test/PortableSensor.h"
#include "test/TempExchange.h"
#include "test/TestComponent.h"

// Delay timer
Timer::Properties tim6Props {
TIM6, RCC_APB1PeriphClockCmd, RCC_APB1Periph_TIM6, TIM6_DAC_IRQn };
Timer delayTimer(tim6Props);

// Pulse led
PulseLED::Properties pulseProps {
RCC_APB1Periph_TIM7, TIM7, TIM7_IRQn, 6, 0 };

// User button
Button::Properties userButtonProps {
GPIOA, GPIO_Pin_0, RCC_AHB1Periph_GPIOA, EXTI_Line0, EXTI_PortSourceGPIOA, EXTI_PinSource0, EXTI0_IRQn };
Button toggleButton(userButtonProps);

// GPS
UART::Properties uart6Props {
GPIOC, USART6,
GPIO_Pin_6, GPIO_Pin_7, GPIO_PinSource6, GPIO_PinSource7, RCC_APB2PeriphClockCmd, RCC_AHB1Periph_GPIOC,
RCC_APB2Periph_USART6, GPIO_AF_USART6, USART6_IRQn, 4800 // 9600 for L10, 4800 for L30
};
UART uartGPS(uart6Props);
GPSL30::Properties gpsProps {
GPIOB, GPIOD, GPIOC,
GPIO_Pin_0, GPIO_Pin_6, GPIO_Pin_8,
RCC_AHB1Periph_GPIOB, RCC_AHB1Periph_GPIOD, RCC_AHB1Periph_GPIOC };
GPSL30 gps(gpsProps, uartGPS); // This can be used for L10 as well. It has the three pins PWR, RST, WUP unconnected

// LEDs
LED::Properties greenLedProperties { GPIOD, GPIO_Pin_12, RCC_AHB1Periph_GPIOD };
LED greenLED(greenLedProperties);
LED::Properties orangeLEDProperties { GPIOD, GPIO_Pin_13, RCC_AHB1Periph_GPIOD };
LED orangeLED(orangeLEDProperties);
LED::Properties redLEDProperties { GPIOD, GPIO_Pin_14, RCC_AHB1Periph_GPIOD };
LED redLED(redLEDProperties);
LED::Properties blueLEDProperties { GPIOD, GPIO_Pin_15, RCC_AHB1Periph_GPIOD };
LED blueLED(blueLEDProperties);

// Pulse LEDs
PulseLED greenPulseLED = PulseLED(greenLED, 1);
PulseLED redPulseLED = PulseLED(redLED, 1);

// ZigBee
MRF24J40::Properties mrfProps {
GPIOE, GPIOE, GPIOB, GPIOD,
SPI3,
GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_2,
GPIO_PinSource4, GPIO_PinSource5, GPIO_PinSource3, GPIO_PinSource4, GPIO_PinSource5,
RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOD, RCC_APB1PeriphClockCmd, RCC_APB1Periph_SPI3,
GPIO_AF_SPI3,
EXTI_Line2, EXTI_PortSourceGPIOD, EXTI_PinSource2, EXTI2_IRQn, SPI3_IRQn };
MRF24J40 mrf = MRF24J40(mrfProps, greenPulseLED, redPulseLED);

// Serial console
UART::Properties uart2Props {
GPIOA, USART2,
GPIO_Pin_2, GPIO_Pin_3, GPIO_PinSource2, GPIO_PinSource3, RCC_APB1PeriphClockCmd, RCC_AHB1Periph_GPIOA,
RCC_APB1Periph_USART2, GPIO_AF_USART2, USART2_IRQn, 921600 };
UART uartSerial(uart2Props);
Console console(uartSerial);

/**
 * Interrupt priority map
 *
 * HIGHEST
 * 0 - MRF SPI
 * 1 ------------------- FreeRTOS critical section
 * 1 - MRF RF
 * 2 - System scheduler
 * 3 - UART - Serial console
 * 5 - UART - GPS
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
		console.toggleLevel();
	lastUserPress = now;
}

void cdeecoSetup(const uint32_t uniqId) {
	//// System setup
	auto radio = new MrfRadio(0, uniqId, uniqId);
	auto system = new CDEECO::System<3, 64>(*radio);

	// Test component
	new TestComponent::Component(*system, uniqId);

	///// Temperature monitoring system
	// Components
	auto sensor = new PortableSensor::Component(*system, uniqId);
	auto alarm = new Alarm::Component(*system, uniqId);

	// Caches
	auto sensorCache = new CDEECO::KnowledgeCache<PortableSensor::Component::Type, PortableSensor::Knowledge, 10>();
	auto alarmCache = new CDEECO::KnowledgeCache<Alarm::Component::Type, Alarm::Knowledge, 10>();
	system->registerCache(sensorCache);
	system->registerCache(alarmCache);

	// Ensembles
	new TempExchange::Ensemble(*alarm, *sensorCache);
	new TempExchange::Ensemble(*sensor, *alarmCache);
}

/** System startup function */
int main(void) {
	// Initialize basic system hardware
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	enableVFP();
	delayTimer.init();

	// Initialize pulse led timer
	PulseLED::initTimer(pulseProps);

	// Initialize user button
	toggleButton.setPriority(8, 0);
	toggleButton.setPressedListener(userPressed, NULL);
	toggleButton.init();

	// Initialize GPS
	uartGPS.setPriority(5, 0);
	uartGPS.init();
	gps.init();

	// Initialize LEDs
	redLED.init();
	blueLED.init();
	greenLED.init();
	orangeLED.init();

	// Initialize pulse LEDs
	redPulseLED.init();
	greenPulseLED.init();

	// Initialize ZigBee
	mrf.setSPIPriority(0, 0);
	mrf.setRFPriority(1, 0);
	mrf.init();

	// Console initialization
	uartSerial.setPriority(15, 15);
	uartSerial.init();
	console.init();

	// Initialize stop-watch
	StopWatch::init(TIM1, RCC_APB2PeriphClockCmd, RCC_APB2Periph_TIM1, TIM1_UP_TIM10_IRQn);

	console.print(Info, "\n\n\n\n\n\n\n\n\n\n\n");
	console.print(Info, "# # # # # # # # # # # # # # # # # # # #\n");
	console.print(Info, " # # # # # # # # # # # # # # # # # # #\n");
	console.print(Info, "# # # # # # # # # # # # # # # # # # # #\n");
	console.print(Info, "\n>>> SYSTEM INIT <<<\n");

	console.print(Info, ">>> Waiting 3s for debugger to stop me...\n");
	delayTimer.mDelay(3000);
	console.print(Info, ">>> Starting system\n");

	// Get unique device id
	const uint32_t uniqId = *((uint32_t*) 0x1FFF7A10);
	console.print(Info, "\n\n>>>>> Unique system Id: %x <<<<<<\n\n\n", uniqId);

	// Initialize user defined CDEECO++ system
	cdeecoSetup(uniqId);

	// Start the scheduler.
	console.print(Info, ">>> Running scheduler\n");
	vTaskStartScheduler();

	// This should not be reached
	console.print(Error, ">>> End reached - THIS SHOULD NOT HAPPEN !!!!\n");
	assert_param(false);
}

// FreeRTOS System error handlers
extern "C" {
	void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName) {
		console.print(Error, "STACK OVERFLOW!!\n");
		assert_param(false);
	}

	void vApplicationMallocFailedHook(void) {
		console.print(Error, "MALLOC FAILED!!!\n");
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
	console.print(Error, "\n\n\n#### Assert failed ####\nFile: %s:%d\n\n\n", file, line);

	/* Infinite loop */
	while(1) {
	}
}
#endif

