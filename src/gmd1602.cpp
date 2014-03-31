#include "gmd1602.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include "main.h"

GMD1602::GMD1602(GPIO_TypeDef *gpio, uint32_t clk): gpio(gpio), clk(clk) {
	writeSem = xSemaphoreCreateMutex();
}

void GMD1602::init() {
//	xSemaphoreTake(writeSem, portMAX_DELAY);
//	vTaskSuspendAll();

	GPIO_InitTypeDef  gpioInitStruct;

	// Enable the GPIO_LED Clock
	RCC_AHB1PeriphClockCmd(clk, ENABLE);

	// Configure the GPIO pin
	gpioInitStruct.GPIO_Pin = E | RS | RW | D4 | D5 | D6 | D7;
	gpioInitStruct.GPIO_Mode = GPIO_Mode_OUT;
	gpioInitStruct.GPIO_OType = GPIO_OType_PP;
	gpioInitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(gpio, &gpioInitStruct);


	// Wait until LCD gets ready
	delayTimer.mDelay(100);

	lo(RW);
	lo(RS);

	// Extra init for clod start ??????
	command(LCD_FUNCTIONSET | LCD_2LINE | LCD_5x8DOTS | LCD_4BITMODE);


	// Wait until LCD gets ready
	delayTimer.mDelay(40);


	write4(LCD_4BITMODE);
	delayTimer.mDelay(40);


	command(LCD_FUNCTIONSET | LCD_2LINE | LCD_5x8DOTS | LCD_4BITMODE);
	delayTimer.mDelay(100);

	command(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);
	delayTimer.mDelay(100);


//	xTaskResumeAll();
//	xSemaphoreGive(writeSem);
}

void GMD1602::writeText(char *text) {
///	xSemaphoreTake(writeSem, portMAX_DELAY);
//	vTaskSuspendAll();
	while(*text)
		data(*text++);
//	xTaskResumeAll();
//	xSemaphoreGive(writeSem);
}

void GMD1602::writeXY(char *text, int x, int y) {
//	xSemaphoreTake(writeSem, portMAX_DELAY);
//	vTaskSuspendAll();
	setXY(x, y);
	while(*text)
		data(*text++);
//	xTaskResumeAll();
//	xSemaphoreGive(writeSem);
}

void GMD1602::clear() {
//	xSemaphoreTake(writeSem, portMAX_DELAY);
//	vTaskSuspendAll();
	command(LCD_CLEARDISPLAY);
	delayTimer.mDelay(100);
//	xTaskResumeAll();
//	xSemaphoreGive(writeSem);
}

void GMD1602::setXY(uint8_t x, uint8_t y) {
	// pos will be used as a temporary variable
	x |= 0b10000000;
	if (y == 1)
		x += 0x40;

	command(x);
}

void GMD1602::command(uint8_t command) {
	lo(RS);
	lo(RW);

	write4(command);
	delayTimer.uDelay(50);
	write4(command << 4);
	delayTimer.uDelay(50);
}

void GMD1602::data(uint8_t data) {
	lo(RW);
	hi(RS);

	write4(data);
	delayTimer.uDelay(50);
	write4(data << 4);

	delayTimer.uDelay(50);
}

void GMD1602::write4(uint8_t data) {
	hi(E);

	if(data & 0b10000000) hi(D7); else lo(D7);
	if(data & 0b01000000) hi(D6); else lo(D6);
	if(data & 0b00100000) hi(D5); else lo(D5);
	if(data & 0b00010000) hi(D4); else lo(D4);

	delayTimer.uDelay(1);

	lo(E);
}

void GMD1602::lo(uint32_t pin) {
	gpio->BSRRH = pin;
}

void GMD1602::hi(uint32_t pin) {
	gpio->BSRRL = pin;
}
