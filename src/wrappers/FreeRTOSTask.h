/**
 * \defgroup wrappers FreeRTOS wrappers
 * C++ classes wrapping FreeRTOS constructs
 */

/**
 * \ingroup wrappers
 * @file FreeRTOSTask.h
 *
 * C++ FreeRTOS task wrapper
 *
 * \date 17. 5. 2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 *
 * @see FreeRTOSTask
 *
 */

#ifndef FREERTOSTASK_H
#define FREERTOSTASK_H

#include <cstdlib>
#include <cstdint>

typedef void* TaskHandle_t;

/**
 * C++ wrapper for FreeRTOS task
 *
 * \ingroup wrappers
 */
class FreeRTOSTask {
public:
	/// Default task stack size in bytes
	static const size_t DEFAULT_STACK_SIZE = 1024;
	/// Default task priority
	static const unsigned long DEFAULT_PRIORITY = 1UL;

	/**
	 * Create FreeRTOStask
	 *
	 * @param stackSize Task stack size
	 * @param priority Task priority
	 */
	FreeRTOSTask(size_t stackSize = DEFAULT_STACK_SIZE, unsigned long priority = DEFAULT_PRIORITY);

	/**
	 * Destroy task
	 */
	virtual ~FreeRTOSTask();

	/**
	 * Code to be executed by task
	 *
	 * This is to be implemented by user.
	 */
	virtual void run() = 0;

	/**
	 * Delay task execution
	 *
	 * This suspends task execution for specified time. Timing may be not accurate as this is using
	 * scheduler to implement the delay. On the other hand the task is not consuming no CPU time when
	 * delayed using this method.
	 *
	 * @param ms Delay in milliseconds
	 */
	static void mDelay(uint16_t ms);
	
	/**
	 * Gets tick count
	 */
	static long getTickCount();

	/**
	 * Suspend this task using FreeRTOS API
	 */
	void suspend();

private:
	/// Task handle used by FreeRTOS
	TaskHandle_t handle;

	/// FreeRTOS task entry point
	static void taskBody(void *data);
};

#endif // FREERTOSTASK_H
