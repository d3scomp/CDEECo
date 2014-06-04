#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "Console.h"

UART::Properties serialProps {
GPIOA, USART2,
GPIO_Pin_2, GPIO_Pin_3, GPIO_PinSource2, GPIO_PinSource3, RCC_APB1PeriphClockCmd, RCC_AHB1Periph_GPIOA,
RCC_APB1Periph_USART2, GPIO_AF_USART2, USART2_IRQn, 921600 };
UART Console::serial(serialProps);

CDEECO::Receiver* Console::receiver = NULL;
char Console::buffer[MAX_LENGTH];
constexpr const char* ConsoleStream::forms[2];
Level Console::level = All;

void Console::init() {
	serial.setPriority(15, 15);
	serial.init();
}

void Console::toggleLevel() {
	switch(level) {
		case None:
			level = All;
			putString("\n\n\n### Log level: All\n\n\n");
			break;

		case Error:
			level = None;
			putString("\n\n\n### Log level: None\n\n\n");
			break;

		case Debug:
			level = Error;
			putString("\n\n\n### Log level: Error\n\n\n");
			break;

		case Info:
			level = Debug;
			putString("\n\n\n### Log level: Debug\n\n\n");
			break;

		case TaskInfo:
			level = Info;
			putString("\n\n\n### Log level: Info\n\n\n");
			break;

		case All:
			level = TaskInfo;
			putString("\n\n\n### Log level: TaskInfo\n\n\n");
			break;
	}
}

void Console::log(const char* format, ...) {
	if(Info >= Console::level) {
		vTaskSuspendAll();
		va_list args;
		va_start(args, format);
		vsprintf(buffer, format, args);
		putString(buffer);
		va_end(args);
		xTaskResumeAll();
	}
}

void Console::print(const Level level, const char* format, ...) {
	if(level >= Console::level) {
		vTaskSuspendAll();
		va_list args;
		va_start(args, format);
		vsprintf(buffer, format, args);
		putString(buffer);
		va_end(args);
		xTaskResumeAll();
	}
}

void Console::putString(const char *text) {
	for(const char* c = text; *c != 0; c++) {
		while(!serial.canSend())
			;
		serial.send(*c);
	}
}

void Console::setFragmentReceiver(CDEECO::Receiver *receiver) {
	Console::receiver = receiver;
	assert(receiver);

	// Enable event listening
	serial.setRecvListener(receiveListener, NULL);
	log("Disabling receive events, disabling send events");
	// TODO: enable receive events for debug input. Make sure ISR do no collide with MRF ISR
	serial.disableRecvEvents();
	serial.disableSendEvents();
}

void Console::receiveListener(void* data) {
	char c = serial.recv();
	if(c == 'X') {
		KnowledgeFragment fragment;

		// Receive header
		fragment.type = recv<decltype(fragment.type)>();
		fragment.id = recv<decltype(fragment.id)>();
		fragment.size = recv<decltype(fragment.size)>();
		fragment.offset = recv<decltype(fragment.offset)>();

		// Receive data
		for(size_t i = 0; i < fragment.size; ++i)
			fragment.data[i] = recv<char>();

		if(receiver)
			receiver->receiveFragment(fragment, 128);
	}
}

void Console::interrupt() {
	serial.txrxInterruptHandler();
}

template<>
uint8_t Console::recv<uint8_t>() {
	return recvHexVal() << 4 | recvHexVal();
}

uint8_t Console::recvHexVal() {
	while(1) {
		while(!serial.canRecv())
			;
		switch(serial.recv()) {
			case '0':
				return 0b0000;
			case '1':
				return 0b0001;
			case '2':
				return 0b0010;
			case '3':
				return 0b0011;
			case '4':
				return 0b0100;
			case '5':
				return 0b0101;
			case '6':
				return 0b0110;
			case '7':
				return 0b0111;
			case '8':
				return 0b1000;
			case '9':
				return 0b1001;
			case 'a':
			case 'A':
				return 0b1010;
			case 'B':
			case 'b':
				return 0b1011;
			case 'C':
			case 'c':
				return 0b1100;
			case 'D':
			case 'd':
				return 0b1101;
			case 'E':
			case 'e':
				return 0b1110;
			case 'F':
			case 'f':
				return 0b1111;
		}
	}

	return 0;
}

void Console::logFragment(const KnowledgeFragment fragment) {
	// Print knowledge fragment
	const size_t bufLen = 512;
	char buffer[bufLen];

	// Write fragment header
	size_t written = sprintf(buffer, "Fragment:Type:%lx Id:%lx Size:%x Offset:%x", fragment.type, fragment.id,
			fragment.size, fragment.offset);

	// Write fragment data
	for(size_t i = 0; i < fragment.length(); ++i) {
		// Hex output formating
		if(i % 16 == 0) {
			buffer[written++] = '\n';
			buffer[written++] = '\t';
		}
		if(i % 2 == 0)
			buffer[written++] = ' ';

		// Print single byte
		written += sprintf(buffer + written, "%02x", ((char*) &fragment)[i]);

		// Stop printing when running out of buffer
		if(written > bufLen - 64) {
			written += sprintf(buffer + written, "...");
			break;
		}
	}

	buffer[written++] = '\n';
	buffer[written++] = '\n';
	buffer[written++] = 0;

	Console::print(Level::Debug, buffer);
}

Hex hex;
