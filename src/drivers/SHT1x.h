/*
 * SHT1x.h
 *
 *  Created on: 27. 5. 2014
 *      Author: Vladimír Matěna
 */

#ifndef SHT1X_H_
#define SHT1X_H_

#include "cstdio"
#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>

#include "Console.h"

class SHT1x {
public:
	struct Properties {
		GPIO_TypeDef* gpio;
		uint32_t gpioClk;
		uint32_t sclPin;
		uint32_t sdaPin;
	};

	SHT1x(Properties &properties) :
			properties(properties) {
		Console::log("\n\n\n\n\nSHT1x constructor");
	}

	void init();
	float readTemperature();

private:
	static const uint8_t CMD_MEASURE_TEMP = 0b00000011;
	static const uint8_t CMD_MEASURE_HUMID = 0b00000101;
	static const uint8_t CMD_READ_STATUS = 0b00000111;
	static const uint8_t CMD_WRITE_STATUS = 0b00000110;
	static const uint8_t CMD_SOFT_RESET = 0b00011110;

	static const uint16_t COM_DELAY = 3;

	static constexpr float TD1 = -39.7f;
	static constexpr float TD2 = 0.01f;

	Properties &properties;

	void reset();
	void sdaMode(uint32_t mode);
	void sdaHi(void);
	void sdaLo(void);
	bool sdaRead();
	void sclPulse(void);
	void sclHi();
	void sclLo();
	bool sendByte(uint8_t data);
	uint8_t receiveByte();
	void startCommand(void);
	uint16_t readCommand(uint8_t command);
};

#endif // SHT1X_H_
