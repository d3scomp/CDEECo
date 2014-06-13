#include "stm32f4xx_it.h"

#include "main.h"
#include "LED.h"

#include "Console.h"
#include "cdeeco/Radio.h"


/** @addtogroup Template_Project
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief   This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void) {
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void) {
	/* Go to infinite loop when Hard Fault exception occurs */
	Console::log("Hard fault !!!\n");
	while (1) { }
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void) {
	/* Go to infinite loop when Memory Manage exception occurs */
	Console::log("Memory manager exception !!!\n");
	while (1) { }
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void) {
	/* Go to infinite loop when Bus Fault exception occurs */
	Console::log("Bus fault !!!\n");
	while (1) { }
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void) {
	/* Go to infinite loop when Usage Fault exception occurs */
	Console::log("Usage fault exception !!!\n");
	while (1) { }
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
/*void SVC_Handler(void) {
}*/

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void) {
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
/*void PendSV_Handler(void) {
}*/

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
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
 * @param  None
 * @retval None
 */
void USART2_IRQHandler(void) {
	Console::interrupt();
}

/**
 * @brief  This function handles USART2 global interrupt request.
 * @param  None
 * @retval None
 */
void USART6_IRQHandler(void) {
	uartGPS.txrxInterruptHandler();
}

/**
 * @brief  This function handles EXTI0_IRQ Handler.
 * @param  None
 * @retval None
 */
void EXTI0_IRQHandler(void) {
	toggleButton.pressedInterruptHandler();
}

/**
 * @brief  This function handles EXTI1_IRQ Handler.
 * @param  None
 * @retval None
 */
void EXTI1_IRQHandler(void) {
//	msgHandler.runInterruptHandler();
}

/**
 * @brief  This function handles EXTI2_IRQ Handler.
 * @param  None
 * @retval None
 */
void EXTI2_IRQHandler(void) {
	CDEECO::Radio::mrf.rfInterruptHandler();
}

/**
 * @brief  This function handles SPI1 global interrupt request.
 * @param  None
 * @retval None
 */
void SPI3_IRQHandler(void) {
//	GPIOA->BSRRL = GPIO_Pin_5;    // Requires test2Led to be initialized in main.cpp
	CDEECO::Radio::mrf.spiInterruptHandler();
//	GPIOA->BSRRH = GPIO_Pin_5;
}

void TIM7_IRQHandler() {
	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
	PulseLED::tickInterruptHandler();
}

/**
 * @}
 */
