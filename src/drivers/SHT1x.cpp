/*
 * SHT1x.cpp
 *
 *  Created on: 28. 5. 2014
 *      Author: Vladimír Matěna
 */

#include "SHT1x.h"

#include "main.h"
#include "cstdio"
#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>

#include "Console.h"

void SHT1x::init() {
	// Initialize GPIO
	RCC_AHB1PeriphClockCmd(properties.gpioClk, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = properties.sclPin | properties.sdaPin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(properties.gpio, &GPIO_InitStruct);
	sclLo();
	sdaHi();
}

float SHT1x::readTemperature() {
	uint16_t raw = readCommand(CMD_MEASURE_TEMP);
	return TD1 + TD2 * raw;
}

void SHT1x::reset() {
	// Wait for sensor sleep state
	delayTimer.mDelay(11);

	startCommand();
	sendByte(CMD_SOFT_RESET);

	// Wait for sensor sleep state
	delayTimer.mDelay(11);
}

void SHT1x::sdaMode(uint32_t mode) {
	// Find pin position
	uint32_t pinPos = 0;
	for(int i = 0; i < 8; ++i)
		if(1 << i & properties.sdaPin) {
			pinPos = i;
			break;
		}

	// Set pin mode
	properties.gpio->MODER &= ~(GPIO_MODER_MODER0 << (pinPos * 2));
	properties.gpio->MODER |= (mode << (pinPos * 2));
}

void SHT1x::sdaHi(void) {
	sdaMode(GPIO_Mode_OUT);
	GPIO_SetBits(properties.gpio, properties.sdaPin);
}

void SHT1x::sdaLo(void) {
	sdaMode(GPIO_Mode_OUT);
	GPIO_ResetBits(properties.gpio, properties.sdaPin);
}

bool SHT1x::sdaRead() {
	sdaMode(GPIO_Mode_IN);
	return GPIO_ReadInputDataBit(properties.gpio, properties.sdaPin) != Bit_RESET;
}

void SHT1x::sclPulse(void) {
	sclHi();
	delayTimer.uDelay(COM_DELAY);
	sclLo();
}

void SHT1x::sclHi() {
	GPIO_SetBits(properties.gpio, properties.sclPin);
}

void SHT1x::sclLo() {
	GPIO_ResetBits(properties.gpio, properties.sclPin);
}

/**
 * Send byte to sensor
 *
 * @param data Byte to send
 * @return True when successful, false otherwise
 */
bool SHT1x::sendByte(uint8_t data) {
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

uint8_t SHT1x::receiveByte() {
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

void SHT1x::startCommand(void) {
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

uint16_t SHT1x::readCommand(uint8_t command) {
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