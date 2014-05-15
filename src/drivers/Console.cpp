#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "Console.h"

#ifdef CONSOLE_LCD
GMD1602 Console::lcd(GPIOE, RCC_AHB1Periph_GPIOE);
char Console::stored[20];
#endif
#ifdef CONSOLE_SERIAL
UART::Properties serialProps {
GPIOA, USART2,
GPIO_Pin_2, GPIO_Pin_3, GPIO_PinSource2, GPIO_PinSource3, RCC_APB1PeriphClockCmd, RCC_AHB1Periph_GPIOA,
		RCC_APB1Periph_USART2, GPIO_AF_USART2, USART2_IRQn, 921600 };
UART Console::serial(serialProps);
#endif

void* Console::listenerData = NULL;
Console::Listener Console::listenerFunc = NULL;

void Console::init() {
#ifdef CONSOLE_LCD
	lcd.init();
#endif

#ifdef CONSOLE_SERIAL
	serial.init();
#endif
}
void Console::log(const char *text) {
	vTaskSuspendAll();

#ifdef CONSOLE_LCD
	lcd.clear();
	lcd.writeXY(stored, 0, 0);
	strcpy(stored, text);
	lcd.writeXY(text, 0, 1);
#endif

#ifdef CONSOLE_SERIAL
	for(const char* c = text; *c != 0; c++) {
		while(!serial.canSend());
		serial.send(*c);
	}
	while(!serial.canSend());
	serial.send('\n');
#endif

	xTaskResumeAll();
}

void Console::setRecvFragmentListener(Listener listener, void* data) {
	// Store listener function and data
	listenerData = data;
	listenerFunc = listener;
	assert(listenerData);
	assert(listenerFunc);

	// Enable event listening
	serial.setRecvListener(receiveListener, NULL);
	log("Enabling receive events");
	serial.setPriority(2, 2);
	serial.enableRecvEvents();
}

void Console::receiveListener(void* data) {
	char recv = serial.recv();
	if(recv == 'X') {
		Console::log(">>>> Received <<<<");
	}
}

void Console::interrupt() {
	serial.txrxInterruptHandler();
}

void Console::logFragment(KnowledgeFragment fragment) {
	// Print knowledge fragment
	const size_t bufLen = 256;
	char buffer[bufLen];

	// Write fragment header
	size_t written = sprintf(buffer, "Broadcast Fragment:\n\tType:%lx\n\tId:%lx\n\tSize:%x\n\tOffset:%x", fragment.type,
			fragment.id, fragment.size, fragment.offset);

	// Write fragment data
	size_t validSize = sizeof(fragment) - sizeof(fragment.data) + fragment.size;
	for(size_t i = 0; i < validSize; ++i) {
		// Hex output formating
		if(i % 16 == 0) {
			buffer[written++] = '\n';
			buffer[written++] = '\t';
		}
		if(i % 2 == 0)
			buffer[written++] = ' ';

		// Print single byte
		written += sprintf(buffer + written, "%02x", ((char*)&fragment)[i]);

		// Stop printing when running out of buffer
		if(written > bufLen - 32) {
			sprintf(buffer + written, "...");
			break;
		}
	}

	Console::log(buffer);
}
