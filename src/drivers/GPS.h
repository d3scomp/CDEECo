/**
 * \ingroup drivers
 * @file GPS.h
 *
 * \date 28. 9. 2013
 * \author Tomas Bures <bures@d3s.mff.cuni.cz>
 */

#ifndef GPS_H_
#define GPS_H_

#include "stm32f4xx.h"
#include "UART.h"

class GPSL10 {
public:
	struct GPSFix {
		bool valid;
		uint8_t hour;
		uint8_t minute;
		uint8_t second;
		uint8_t month;
		uint8_t day;
		uint16_t year;
		float latitude;
		float longitude;
		float speed;
		float course;
		float variation;
	};

	static constexpr auto MAX_GPS_SENTENCE_LENGTH = 255;

	GPSL10(UART& uart);
	~GPSL10();

	void init();

	inline char *getSentence() {
		return gpsSentence[validGPSSentenceIdx];
	}

	GPSFix getGPSFix();

	typedef void (*SentenceListener)(void *);

	void setSentenceListener(SentenceListener sentenceListener, void* obj);

private:
	UART& uart;

	static void uartRecvListenerStatic(void *obj);
	void uartRecvListener();

	SentenceListener sentenceListener = NULL;
	void* sentenceListenerObj = NULL;

	char gpsSentence[2][256];
	int validGPSSentenceIdx = 1;
	int workGPSSentenceIdx = 0;
	int workGPSSentenceBufPos = 0;
};

class GPSL30 : public GPSL10 {
public:
	struct Properties {
		GPIO_TypeDef *gpioPWR, *gpioRST, *gpioWUP;
		uint32_t pinPWR, pinRST, pinWUP;
		uint32_t clkPWR, clkRST, clkWUP;
	};

	GPSL30(Properties& initProps, UART& uart);
	~GPSL30();

	void init();

private:
	Properties props;
};

#endif /* GPS_H_ */
