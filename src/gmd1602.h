/**
 * GMD1602A Control code
 *
 * Inspired by LiquidCrystal library: http://arduino.cc/en/Tutorial/LiquidCrystal
 */

#ifndef GMD1602_H_
#define GMD1602_H_

#include "stm32f4xx.h"

#include "FreeRTOS.h"
#include "semphr.h"

class GMD1602 {
public:
	GMD1602(GPIO_TypeDef* gpio, uint32_t clk);

	void init();
	void writeText(char *text);
	void writeXY(char *text, int x, int y);
	void clear();
	void setXY(uint8_t x, uint8_t y);
private:
	GPIO_TypeDef* gpio;
	uint32_t clk;

	const uint32_t RS = GPIO_Pin_6;
	const uint32_t RW = GPIO_Pin_5;
	const uint32_t E = GPIO_Pin_4;

	const uint32_t D4 = GPIO_Pin_3;
	const uint32_t D5 = GPIO_Pin_2;
	const uint32_t D6 = GPIO_Pin_1;
	const uint32_t D7 = GPIO_Pin_0;

	const uint8_t LCD_SET4BIT = 0b0010;

	// flags for function set
	const uint8_t LCD_8BITMODE = 0b00010000;
	const uint8_t LCD_4BITMODE = 0b00000000;
	const uint8_t LCD_2LINE = 0b00001000;
	const uint8_t LCD_1LINE = 0b00000000;
	const uint8_t LCD_5x10DOTS = 0b00000100;
	const uint8_t LCD_5x8DOTS = 0b00000000;

	// commands
	const uint8_t LCD_CLEARDISPLAY = 0b0001;
	const uint8_t LCD_RETURNHOME = 0x0010;
	const uint8_t LCD_ENTRYMODESET = 0x04;
	const uint8_t LCD_DISPLAYCONTROL = 0x08;
	const uint8_t LCD_CURSORSHIFT = 0x10;
	const uint8_t LCD_FUNCTIONSET = 0b00100000;
	const uint8_t LCD_SETCGRAMADDR = 0x40;
	const uint8_t LCD_SETDDRAMADDR = 0x80;

	// flags for display on/off control
	const uint8_t LCD_DISPLAYON = 0x04;
	const uint8_t LCD_DISPLAYOFF = 0x00;
	const uint8_t LCD_CURSORON = 0x02;
	const uint8_t LCD_CURSOROFF = 0x00;
	const uint8_t LCD_BLINKON = 0x01;
	const uint8_t LCD_BLINKOFF = 0x00;

	void command(uint8_t command);
	void data(uint8_t data);
	void write4(uint8_t data);
	void lo(uint32_t pin);
	void hi(uint32_t pin);

	SemaphoreHandle_t writeSem;
};











#endif
