/**
 * \ingroup drivers
 * @file StopWatch.cpp
 *
 * Time measurement driver implementation
 *
 * \date 4.7.2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 */

#include "StopWatch.h"


TIM_TypeDef* StopWatch::tim;
volatile uint16_t StopWatch::diff;
volatile bool StopWatch::overrun;

/**
 * Start measurement
 */
void StopWatch::start() {
	tim->CNT = 0;
	overrun = false;
}

/**
 * End measurement
 */
void StopWatch::stop() {
	diff = tim->CNT;
}

/**
 * Print measured time difference
 */
void StopWatch::print() {
	if(!overrun)
		console.print(None, "#DIFF:%d\n", diff);
	else
		console.print(None, "#DIFF:-1\n");
}

/**
 * Initialize stop watch
 *
 * @param timer Pointer to time structure
 * @param clkCmdFun timer Enable function
 * @param clk Timer enable function parameter
 * @param irqn Timer IRQ number
 */
void StopWatch::init(TIM_TypeDef *timer, void (*clkCmdFun)(uint32_t, FunctionalState), uint32_t clk, IRQn irqn) {
	tim = timer;

	clkCmdFun(clk, ENABLE);

	TIM_TimeBaseInitTypeDef timInitStruct;
	timInitStruct.TIM_Prescaler = 168;
	timInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	timInitStruct.TIM_Period = 0xFFFF;
	timInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	timInitStruct.TIM_RepetitionCounter = 0x0000;
	TIM_TimeBaseInit(tim, &timInitStruct);

	TIM_ClearFlag(tim, TIM_FLAG_Update);
	TIM_ITConfig(tim, TIM_IT_Update, ENABLE);
	TIM_Cmd(tim, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = irqn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}

/**
 * Handle timer interrupt
 *
 * This signalizes timer overflow during measurement. Thus current measurement is invalid.
 */
void StopWatch::interrupt() {
	overrun = true;
}
