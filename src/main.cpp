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

#include "TestComponent.h"

#include <cstdio>

uint32_t mainCycles;

Timer::Properties tim6Props {
	TIM6, RCC_APB1PeriphClockCmd, RCC_APB1Periph_TIM6, TIM6_DAC_IRQn
};
Timer delayTimer(tim6Props);

LED::Properties greenLedProps {
	GPIOD, GPIO_Pin_12, RCC_AHB1Periph_GPIOD
};
LED::Properties redLedProps {
	GPIOD, GPIO_Pin_14, RCC_AHB1Periph_GPIOD
};
LED::Properties orangeLedProps {
	GPIOD, GPIO_Pin_13, RCC_AHB1Periph_GPIOD
};
LED::Properties blueLedProps {
	GPIOD, GPIO_Pin_15, RCC_AHB1Periph_GPIOD
};
//LED green(greenLedProps);
LED red(redLedProps);
LED blue(blueLedProps);
LED orange(orangeLedProps);

GMD1602 lcd(GPIOE, RCC_AHB1Periph_GPIOE);

#define LEDFLASH(LED,DELAY) \
	void led_##LED(void *pvParameters) { \
	LED.init(); \
	while(1) { \
		vTaskDelay(DELAY / portTICK_PERIOD_MS); \
		LED.on(); \
		vTaskDelay(DELAY / portTICK_PERIOD_MS); \
		LED.off(); \
	} \
} \

LEDFLASH(red, 100);
LEDFLASH(blue, 127);
LEDFLASH(orange, 200);


void LCDWrite(void *pvParameters) {
	int cnt = 0;
	char buff[10];
	lcd.clear();
	while(1) {
		sprintf(buff, "%d", cnt++);
		lcd.writeXY(buff, 0, 0);
		vTaskDelay(100 / portTICK_PERIOD_MS);
		//	for(volatile int i = 0; i < 100000; ++i);
	}
}

void LCDProgress(void *pvParameters) {
	int pos = 0;
	int dir = 1;
	while(1) {
		lcd.writeXY(" ", pos, 1);
		pos += dir;
		if(pos > 14 || pos < 1)
			dir *= -1;
		lcd.writeXY("#", pos, 1);
		vTaskDelay(42 / portTICK_PERIOD_MS);
	}
}

int main(void) {
	delayTimer.setPriority(1,1);
	delayTimer.init();

	lcd.init();
	lcd.writeXY("Task create..", 0, 0);

	/* Spawn the LED tasks. */
//	xTaskCreate(led_red, "redLEDFlash", 2*configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 1UL ), ( TaskHandle_t * ) NULL );
	xTaskCreate(led_blue, "blueLEDFlash", 2*configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 1UL ), ( TaskHandle_t * ) NULL );
//	xTaskCreate(led_orange, "orangeLEDFlash", 2*configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 1UL ), ( TaskHandle_t * ) NULL );

	/* Spawn the LCD counter task. */
	xTaskCreate(LCDWrite, "LCDWrite", 2*configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 1UL ), ( TaskHandle_t * ) NULL );

//	xTaskCreate(LCDProgress, "LCDProgress", 2*configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 1UL ), ( TaskHandle_t * ) NULL );

	lcd.writeXY("TestComponent..", 0, 0);

	TestComponent testComponent;

	lcd.writeXY("Running sched..", 0, 0);

	/* Start the scheduler. */
	vTaskStartScheduler();


	lcd.clear();
	lcd.writeText("End reached!!!");
	while(1);
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

	/* Infinite loop */
	while (1) { }
}
#endif

