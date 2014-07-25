/**
 * \ingroup drivers
 * @file GMD1602.h
 *
 * GMD1602A Control code a simple 16x2 LCD driver
 *
 * Code here is inspired by LiquidCrystal library: http://arduino.cc/en/Tutorial/LiquidCrystal
 *
 * \date 3.3.2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 */

#ifndef GMD1602_H
#define GMD1602_H

#include "stm32f4xx.h"

#include "FreeRTOS.h"
#include "semphr.h"

/**
 * Testing driver for 16x2 LCD alphanumeric display
 *
 * This code was written as practice of STM32F4 hardware control. It should not be used as is.
 *
 * \ingroup drivers
 */
class GMD1602 {
public:
	/**
	 * Construct GMD1602 driver
	 *
	 * @param gpio GPIO to use for hardware control
	 * @param clk GPIO clock to use for hardware control
	 */
	GMD1602(GPIO_TypeDef* gpio, uint32_t clk);

	/**
	 * Initialize the driver
	 */
	void init();

	/**
	 * Write text
	 *
	 * @param text text to write
	 */
	void writeText(const char *text);

	/**
	 * Write text at coordinates
	 *
	 * @param text Text to write
	 * @param x Column to start at. Allowed range is from 0 to 15.
	 * @param y Row to start at. Allowed range is from 0 to 1.
	 */
	void writeXY(const char *text, int x, int y);

	/**
	 * Clear screen
	 */
	void clear();

	/**
	 * Set cursor to position
	 *
	 * @param x Column to start at. Allowed range is from 0 to 15.
	 * @param y Row to start at. Allowed range is from 0 to 1.
	 */
	void setXY(uint8_t x, uint8_t y);
private:
	/// Hardware GPIO used to access LCD
	GPIO_TypeDef* gpio;
	/// GPIO clock used to access LCD
	uint32_t clk;

	/// GPIO pin connected to the RS pin on the LCD
	const uint32_t RS = GPIO_Pin_6;
	/// GPIO pin connected to the RW pin on the LCD
	const uint32_t RW = GPIO_Pin_5;
	/// GPIO pin connected to the E pin on the LCD
	const uint32_t E = GPIO_Pin_4;

	/// GPIO pin connected to the data 3 pin on the LCD
	const uint32_t D4 = GPIO_Pin_3;
	/// GPIO pin connected to the data 2 pin on the LCD
	const uint32_t D5 = GPIO_Pin_2;
	/// GPIO pin connected to the data 1 pin on the LCD
	const uint32_t D6 = GPIO_Pin_1;
	/// GPIO pin connected to the data 0 pin on the LCD
	const uint32_t D7 = GPIO_Pin_0;

	/// Command constant to set LCD into 4 bit mode
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

	/**
	 * Send command to LCD
	 *
	 * @param command Command value
	 */
	void command(uint8_t command);

	/**
	 * Send data to LCD
	 *
	 * @param data Data to send
	 */
	void data(uint8_t data);

	/**
	 * Write 4 bits to LCD
	 *
	 * @param data Data to write. First 4 low-order bits are used.
	 */
	void write4(uint8_t data);
	/**
	 * Set pin low
	 *
	 * @param pin Pin to set low
	 */
	void lo(uint32_t pin);
	/**
	 * Set pin hi
	 *
	 * @param pin Pin to set hi
	 */
	void hi(uint32_t pin);

	/**
	 * LCD access semaphore
	 */
	SemaphoreHandle_t writeSem;
};

#endif
