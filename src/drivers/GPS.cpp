/**
 * \ingroup drivers
 * @file GPS.cpp
 *
 * \date 28. 9. 2013
 * \author Tomas Bures <bures@d3s.mff.cuni.cz>
 */

#include <cstring>
#include <cstdlib>

#include "GPS.h"
#include "main.h"
#include "Console.h"

GPSL10::GPSL10(UART& uart) : uart(uart) {
}

GPSL10::~GPSL10() {
}

void GPSL10::init() {
	gpsSentence[0][0] = 0;
	gpsSentence[1][0] = 0;
	uart.setRecvListener(uartRecvListenerStatic, this);
	uart.enableRecvEvents();
}

void GPSL10::uartRecvListenerStatic(void* obj) {
	static_cast<GPSL10*>(obj)->uartRecvListener();
}

void GPSL10::uartRecvListener() {
	uint8_t data = uart.recv();
	char *workBuf = gpsSentence[workGPSSentenceIdx];

	if (data == 10) {
	} else if (data == 13) {
		workBuf[workGPSSentenceBufPos] = 0;
		workGPSSentenceBufPos = 0;
		if (!std::strncmp(workBuf, "$GPRMC", 6)) {
			validGPSSentenceIdx = workGPSSentenceIdx;
			workGPSSentenceIdx = (workGPSSentenceIdx == 0 ? 1 : 0);
			if(sentenceListener)
				sentenceListener(sentenceListenerObj);
		}
	} else {
		if (workGPSSentenceBufPos < MAX_GPS_SENTENCE_LENGTH - 1) {
			workBuf[workGPSSentenceBufPos++] = data;
		}
	}
}

void GPSL10::setSentenceListener(SentenceListener sentenceListener, void* obj) {
	this->sentenceListener = sentenceListener;
	sentenceListenerObj = obj;
}

GPSL30::GPSL30(Properties& initProps, UART& uart) : GPSL10(uart), props(initProps) {
}

GPSL30::~GPSL30() {
}

void GPSL30::init() {
	GPIO_InitTypeDef  gpioInitStruct;

	RCC_AHB1PeriphClockCmd(props.clkPWR, ENABLE);
	RCC_AHB1PeriphClockCmd(props.clkRST, ENABLE);
	RCC_AHB1PeriphClockCmd(props.clkWUP, ENABLE);

	gpioInitStruct.GPIO_Mode = GPIO_Mode_OUT;
	gpioInitStruct.GPIO_OType = GPIO_OType_PP;
	gpioInitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	gpioInitStruct.GPIO_Pin = props.pinRST;
	GPIO_Init(props.gpioRST, &gpioInitStruct);
	GPIO_ResetBits(props.gpioRST, props.pinRST);
	delayTimer.mDelay(1);
	GPIO_SetBits(props.gpioRST, props.pinRST);
	delayTimer.mDelay(1);

	gpioInitStruct.GPIO_Pin = props.pinPWR;
	GPIO_Init(props.gpioPWR, &gpioInitStruct);

	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN;
	gpioInitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

	gpioInitStruct.GPIO_Pin = props.pinWUP;
	GPIO_Init(props.gpioWUP, &gpioInitStruct);


	GPSL10::init();

	while (!GPIO_ReadInputDataBit(props.gpioWUP, props.pinWUP)) {
		GPIO_ResetBits(props.gpioPWR, props.pinPWR);
		delayTimer.mDelay(1);
		GPIO_SetBits(props.gpioPWR, props.pinPWR);
		delayTimer.mDelay(1);
	}
}

GPSL10::GPSFix GPSL10::getGPSFix() {
	// $GPRMC,144844.546,A,5007.5313,N,01431.7824,E,1.08,262.31,130614,,,A*64
	// 0         10        20        30        40        50        60        70
	char* sent = getSentence();
	GPSFix res;

	// Decode validity
	res.valid = sent[18] == 'A';

	// Decode time
	uint32_t time = atoi(sent + 7);
	res.hour = time / 10000;
	res.minute = (time / 100) % 100;
	res.second = time % 100;

	// Decode date
	uint32_t date = atoi(sent + 57);
	res.day = date / 10000;
	res.month = (date / 100) % 100;
	res.year = 2000 + date % 100;

	// Decode latitude
	uint16_t lat1 = atoi(sent + 20);
	uint16_t lat2 = atoi(sent + 25);
	uint16_t latDeg = lat1 / 100;
	uint16_t latMin = lat1 % 100;
	uint16_t latMinDec = lat2;
	res.latitude = latDeg + (float)latMin / 60 + (float)latMinDec / 600000;
	if(sent[30] == 'S')
		res.latitude *= -1;

	// Decode longtitude
	uint16_t lon1 = atoi(sent + 32);
	uint16_t lon2 = atoi(sent + 38);
	uint16_t lonDeg = lon1 / 100;
	uint16_t lonMin = lon1 % 100;
	uint16_t lonMinDec = lon2;
	res.longitude = lonDeg + (float)lonMin / 60 + (float)lonMinDec / 600000;
	if(sent[43] == 'W')
			res.longitude *= -1;

	return res;
}
