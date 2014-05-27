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

	void init() {
		// Initialize GPIO
		RCC_AHB1PeriphClockCmd(properties.gpioClk, ENABLE);
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.GPIO_Pin = properties.sclPin;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(properties.gpio, &GPIO_InitStruct);
		sclLo();
		sdaHi();
	}

	float readTemperature() {
		uint16_t raw = readCommand(CMD_MEASURE_TEMP);

		int w = 0;
		char buff[20];
		buff[w++] = 'b';
		buff[w++] = '0';
		buff[w++] = ':';
		for(int i = 0; i < 16; ++i) {
			if(raw & (1 << i))
				buff[w++] = '1';
			else
				buff[w++] = '0';
		}
		Console::log(buff);

		return TD1 + TD2 * raw;
	}

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

	void reset() {
		// Wait for sensor sleep state
		delayTimer.mDelay(11);

		startCommand();
		sendByte(CMD_SOFT_RESET);

		// Wait for sensor sleep state
		delayTimer.mDelay(11);
	}

	void sdaHi(void) {
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Pin = properties.sdaPin;
		GPIO_Init(properties.gpio, &GPIO_InitStructure);
	}
	void sdaLo(void) {
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Pin = properties.sdaPin;
		GPIO_Init(properties.gpio, &GPIO_InitStructure);
	}

	void sclPulse(void) {
		sclHi();
		delayTimer.uDelay(COM_DELAY);
		sclLo();
	}

	void sclHi() {
		GPIO_SetBits(properties.gpio, properties.sclPin);
	}

	void sclLo() {
		GPIO_ResetBits(properties.gpio, properties.sclPin);
	}

	bool sdaRead() {
		return GPIO_ReadInputDataBit(properties.gpio, properties.sdaPin) != Bit_RESET;
	}

	/**
	 * Send byte to sensor
	 *
	 * @param data Byte to send
	 * @return True when successful, false otherwise
	 */
	bool sendByte(uint8_t data) {
		// Send command data
		for(int i = 0; i < 8; ++i) {
			if(data & 0b10000000)
				sdaHi();
			else
				sdaLo();
			data <<= 1;
			delayTimer.uDelay(COM_DELAY);
			sclPulse();
		}

		// Read acknowledge
		sdaHi();
		delayTimer.mDelay(5);
		uint8_t ack = sdaRead();
		sclPulse();

		return !ack;
	}

	uint8_t receiveByte() {
		// Receive data
		uint8_t data = 0;
		for(int i = 0; i < 8; ++i) {
			data <<= 1;
			if(sdaRead())
				data |= 1;
			sclPulse();
			delayTimer.uDelay(COM_DELAY);
		}

		// Acknowledge received data
		sdaLo();
		delayTimer.uDelay(COM_DELAY);
		sclPulse();
		sdaHi();
		delayTimer.uDelay(COM_DELAY);

		return data;
	}

	void startCommand(void) {
		// Command start sequence
		sdaHi();
		sclLo();

		delayTimer.uDelay(COM_DELAY);
		sclHi();
		delayTimer.uDelay(COM_DELAY);

		sdaLo();
		delayTimer.uDelay(COM_DELAY);
		sclLo();
		delayTimer.uDelay(COM_DELAY);
		sclHi();
		delayTimer.uDelay(COM_DELAY);

		sdaHi();
		delayTimer.uDelay(COM_DELAY);
		sclLo();
	}

	uint16_t readCommand(uint8_t command) {
		reset();

		startCommand();
		bool ok = sendByte(command);
		if(!ok)
			Console::log("Error writing byte to sensor !!!");

		delayTimer.mDelay(320);

		union {
			uint16_t value;
			uint8_t bytes[2];
		} result;

		result.bytes[1] = receiveByte();
		result.bytes[0] = receiveByte();

		return result.value;
	}
};

#endif // SHT1X_H_
