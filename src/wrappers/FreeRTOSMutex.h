/**
 * \ingroup wrappers
 * @file FreeRTOSMutex.h
 *
 * C++ wrapper for FreeRTOS mutex
 *
 * \date 17. 5. 2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 *
 */

#ifndef FREERTOSMUTEX_H
#define FREERTOSMUTEX_H

// Forward declaration in order to avoid unecessary dependencies
typedef void * SemaphoreHandle_t;

/**
 * C++ wrapper for FreeRTOS mutex
 *
 * \ingroup wrappers
 */
class FreeRTOSMutex {
public:
	/**
	 * FreeRTOS mutex constructor
	 *
	 * Takes no parameters. Creates new FreeRTOS mutex. FreeRTOS system will allocate memory for the mutex structures.
	 */
	FreeRTOSMutex();

	/**
	 * Lock the mutex
	 *
	 * This will block if the mutex is already locked.
	 */
	void lock();

	/**
	 * Unlock the mutex
	 */
	void unlock();

private:
	/**
	 * FreeRTOS semaphore handle
	 *
	 * Used for mutex implementation.
	 */
	SemaphoreHandle_t sem;
};

#endif // FREERTOSMUTEX_H
