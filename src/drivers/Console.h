#ifndef CONSOLE_H
#define CONSOLE_H

#include <cstdio>
#include <stdio.h>
#include <stdarg.h>

#include "cdeeco/KnowledgeFragment.h"
#include "cdeeco/Receiver.h"

#include "UART.h"

/**
 * Console log level type
 */
enum Level {
	/// Print nothing
	None = 5,

	/// Print only errors
	Error = 4,

	/// Print errors and task info
	TaskInfo = 3,

	/// Print errors, task info and info
	Info = 2,

	/// Print errors, infos and debug output
	Debug = 1,

	/// Print everything
	All = 0
};

/**
 * Console output driver
 *
 * This driver provides text output using the UART driver instance.
 */
class Console {
public:
	/// Max output length
	static const size_t MAX_LENGTH = 500;

	/**
	 * Construct Console driver
	 *
	 * @param serial UART to use for output
	 */
	Console(UART &serial);

	/**
	 * Initialize console driver
	 */
	void init();

	/**
	 * Print fragment to the output
	 *
	 * @param fragment Knowledge fragment to print
	 */
	void logFragment(const CDEECO::KnowledgeFragment fragment);

	/**
	 * Set receiver for knowledge fragment decoded by console
	 *
	 * @param receiver Reference to receiver interface
	 */
	void setFragmentReceiver(CDEECO::Receiver *receiver);

	/**
	 * Print text to console output
	 *
	 * This behaves similary to printf and uses sprintf internally.
	 *
	 * Do not print floats or doubles unless you know FPU can handle them. STM32F4 FPU won't.
	 *
	 * @param level Message log level
	 * @param format Message formating text. Identical to printf format.
	 */
	void print(const Level level, const char * format, ...);

	/**
	 * Print float value
	 *
	 * @param level Message log level
	 * @param value Float value to print
	 * @param decimals Number of decimal digits to print
	 */
	void printFloat(const Level level, const float value, const int decimals = 2);

	/**
	 * Set console output level
	 */
	void setLevel(const Level level);

	/**
	 * Set next console output level
	 *
	 * This is used to change levels using user button.
	 */
	void toggleLevel();

private:
	/// Reference to UART used for output
	UART &serial;
	/// Working buffer
	char buffer[MAX_LENGTH];
	/// Receiver for decoded knowledge fragments
	CDEECO::Receiver *receiver = NULL;
	/// Current level
	Level level = Info;

	/**
	 * Static listener for serial receive events
	 *
	 * @param data receiver data
	 */
	static void staticReceiveListener(void* data);

	/**
	 * Listener for serial receive events
	 */
	void receiveListener();

	/**
	 * Receive typed data from console
	 *
	 * Used to read binary data from console.
	 *
	 * @tparam T Type to read
	 * @return Received data typed as T
	 */
	template<typename T>
	T recv() {
		T data;
		for(size_t i = 0; i < sizeof(T); ++i)
			((uint8_t*) &data)[i] = recv<uint8_t>();
		return data;
	}

	/**
	 * Write text to console
	 *
	 * @param text Null terminated string to write to the console.
	 */
	void putString(const char *text);

	/**
	 * Receive single hex digit value from console
	 *
	 * @return hex digit value
	 */
	uint8_t recvHexVal();
};

#endif // CONSOLE_H
