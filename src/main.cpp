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
LED rxtxLed(greenLedProps);
LED outOfSyncLed(redLedProps);
LED mrfRecvLed(blueLedProps);
LED mrfSendLed(orangeLedProps);


GMD1602 lcd(GPIOE, RCC_AHB1Periph_GPIOE);



void LEDFlash(void *pvParameters) {

	lcd.clear();
	lcd.setText("LEDFlash task");

	rxtxLed.init();
	outOfSyncLed.init();
	mrfRecvLed.init();
	mrfSendLed.init();

	while(1) {
		for(volatile int i = 0; i < 1000000; ++i);
		rxtxLed.on();
		outOfSyncLed.on();
		mrfRecvLed.on();
		mrfSendLed.on();


		for(volatile int i = 0; i < 1000000; ++i);
		rxtxLed.off();
		outOfSyncLed.off();
		mrfRecvLed.off();
		mrfSendLed.off();
	}
}

void LCDWrite(void *pvParameters) {
	lcd.clear();
	lcd.setText("LCDWrite task");
	vTaskDelay(1000 / portTICK_PERIOD_MS);

	int cnt = 0;
	char buff[20];
	lcd.clear();
	while(1) {
		sprintf(buff, "cnt: %p", cnt++);
		lcd.setCursor(0,0);
		lcd.setText(buff);
		vTaskDelay(10 / portTICK_PERIOD_MS);
		//for(volatile int i = 0; i < 100000; ++i);
	}
}

int main(void) {
	delayTimer.setPriority(1,1);
	delayTimer.init();

	rxtxLed.init();
	outOfSyncLed.init();
	mrfRecvLed.init();
	mrfSendLed.init();

	rxtxLed.on();
	outOfSyncLed.on();
	mrfRecvLed.on();
	mrfSendLed.on();

	lcd.init();

	lcd.clear();
	lcd.setText("Task create..");

	/* Spawn the LED task. */
	xTaskCreate( LEDFlash, "LEDFlash", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 1UL ), ( TaskHandle_t * ) NULL );

	/* Spawn the LCD task. */
	xTaskCreate( LCDWrite, "LCDWrite", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 1UL ), ( TaskHandle_t * ) NULL );

	lcd.setText("OK");
	lcd.setCursor(1, 0);
	lcd.setText("Run sched now");
	delayTimer.mDelay(750);

	/* Start the scheduler. */
	vTaskStartScheduler();

	lcd.setText("OK");


	lcd.clear();
	lcd.setText("End reached!!!");
	while(1);


/*
	lcd.setText("Hello world!");

	delayTimer.mDelay(1000);
	lcd.clear();
	lcd.setText("abcdefghijklmnopqrstuvwxyz");

	delayTimer.mDelay(1000);
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.setText("line0,0");
	lcd.setCursor(1,1);
	lcd.setText("line1,1");

	delayTimer.mDelay(1000);
	lcd.clear();
	int cnt = 0;
	char buff[20];
	while(1) {
		sprintf(buff, "cnt: %p", cnt++);
		lcd.setCursor(0,0);
		lcd.setText(buff);
		delayTimer.mDelay(100);
	}

	while(1) {
		for(int i = 0; i < 131072; ++i)
			rxtxLed.on();

		for(int i = 0; i < 131072; ++i)
			rxtxLed.off();
	}*/
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

