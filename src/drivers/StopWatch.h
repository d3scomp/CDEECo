/*
 * StopWatch.h
 *
 *  Created on: 4.7.2014
 *      Author: Vladimír Matěna
 */

#ifndef STOPWATCH_H
#define STOPWATCH_H

#include "stm32f4xx.h"

#include "main.h"
#include "Console.h"

/**
 * Simple stop-watch
 *
 * Intended for execution time measurement with high precision. measured interval must be shorter than 65536us.
 * This stop-watch supports one measurement at time and detects overflows when used with interrupts enabled.
 */
class StopWatch {
public:
	/**
	 * Start measurement
	 */
	static void start();

	/**
	 * End measurement
	 */
	static void stop();

	/**
	 * Print measured time difference
	 */
	static void print();

	/**
	 * Initialize stop watch
	 *
	 * @param timer Pointer to time structure
	 * @param clkCmdFun timer Enable function
	 * @param clk Timer enable function parameter
	 * @param irqn Timer IRQ number
	 */
	static void init(TIM_TypeDef *timer, void (*clkCmdFun)(uint32_t, FunctionalState), uint32_t clk, IRQn irqn);

	/**
	 * Handle timer interrupt
	 *
	 * This signalizes timer overflow during measurement. Thus current measurement is invalid.
	 */
	static void interrupt();

private:
	/// Last timer difference in us
	static volatile uint16_t diff;
	/// Whenever the last measurement resulted in timer overrun
	static volatile bool overrun;
	/// Timer to use for measurement
	static TIM_TypeDef *tim;
};

#endif // STOPWATCH_H
