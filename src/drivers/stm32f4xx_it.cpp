/**
 * \ingroup config
 * @file stm32f4xx_it.cpp
 *
 * STM32F4 interrupt service routine implementations
 *
 */

#include "stm32f4xx_it.h"

#include "main.h"
#include "LED.h"

#include "Console.h"
#include "StopWatch.h"
#include "cdeeco/Radio.h"

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief   This function handles NMI exception.
 */
void NMI_Handler(void) {
}

/**
 * @brief  This function handles Hard Fault exception.
 */
void HardFault_Handler(void) {
	/* Go to infinite loop when Hard Fault exception occurs */
	console.print(Error, "Hard fault !!!\n");
	while (1) { }
}

/**
 * @brief  This function handles Memory Manage exception.
 */
void MemManage_Handler(void) {
	/* Go to infinite loop when Memory Manage exception occurs */
	console.print(Error, "Memory manager exception !!!\n");
	while (1) { }
}

/**
 * @brief  This function handles Bus Fault exception.
 */
void BusFault_Handler(void) {
	/* Go to infinite loop when Bus Fault exception occurs */
	console.print(Error, "Bus fault !!!\n");
	while (1) { }
}

/**
 * @brief  This function handles Usage Fault exception.
 */
void UsageFault_Handler(void) {
	/* Go to infinite loop when Usage Fault exception occurs */
	console.print(Error, "Usage fault exception !!!\n");
	while (1) { }
}

/**
 * @brief  This function handles SVCall exception.
 */
/*void SVC_Handler(void) {
}*/

/**
 * @brief  This function handles Debug Monitor exception.
 */
void DebugMon_Handler(void) {
}

/**
 * @brief  This function handles PendSVC exception.
 */
/*void PendSV_Handler(void) {
}*/

/**
 * @brief  This function handles SysTick Handler.
 */
/*void SysTick_Handler(void) {
	PulseLED::tickInterruptHandler();
}*/

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f40xx.s/startup_stm32f427x.s).                         */
/******************************************************************************/

/**
 * @brief  This function handles USART2 global interrupt request.
 */
void USART2_IRQHandler(void) {
	uartSerial.txrxInterruptHandler();
}

/**
 * @brief  This function handles USART2 global interrupt request.
 */
void USART6_IRQHandler(void) {
	uartGPS.txrxInterruptHandler();
}

/**
 * @brief  This function handles EXTI0_IRQ Handler.
 */
void EXTI0_IRQHandler(void) {
	toggleButton.pressedInterruptHandler();
}

/**
 * @brief  This function handles EXTI1_IRQ Handler.
 */
void EXTI1_IRQHandler(void) {
//	msgHandler.runInterruptHandler();
}

/**
 * @brief  This function handles EXTI2_IRQ Handler.
 */
void EXTI2_IRQHandler(void) {
	mrf.rfInterruptHandler();
}

/**
 * @brief  This function handles SPI1 global interrupt request.
 */
void SPI3_IRQHandler(void) {
//	GPIOA->BSRRL = GPIO_Pin_5;    // Requires test2Led to be initialized in main.cpp
	mrf.spiInterruptHandler();
//	GPIOA->BSRRH = GPIO_Pin_5;
}

void TIM7_IRQHandler() {
	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
	PulseLED::tickInterruptHandler();
}

void TIM1_UP_TIM10_IRQHandler() {
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	StopWatch::interrupt();
}

/**
 * @}
 */
