#include "dma.h"
#include "stm32f103xb.h"

#define 	DMA1EN				(1U<<0)
#define 	DMA1_CCR_EN			(1U<<0)
#define 	DMA1_CCR_MEM2MEM	(1U<<14)
#define 	DMA1_CCR_MINC		(1U<<7)
#define 	DMA1_CCR_PINC		(1U<<6)
#define 	DMA1_CCR_DIR		(1U<<4)
#define 	DMA1_CCR_TCIE		(1U<<1)
#define 	DMA1_CCR_TEIE		(1U<<3)


void dma1_mem2mem_config(void)
{
	/*Enable Clock access to DMA1*/
	RCC->AHBENR |= DMA1EN;

	/*Disable DMA1 Channel1 to configure it*/
	DMA1_Channel1->CCR &= ~DMA1_CCR_EN;

	/*Wait until Channel1 is disabled*/
	while(DMA1_Channel1->CCR & DMA1_CCR_EN){}

	/*Clear Configuration register*/
	DMA1_Channel1->CCR = 0;

	/*Enable memory to memory mode*/
	DMA1_Channel1->CCR |= DMA1_CCR_MEM2MEM;


	/*Set data size to 16 bits for both memory and peripheral*/
	/*Set MSIZE*/
	DMA1_Channel1->CCR |= (1U<<10);
	DMA1_Channel1->CCR &= ~(1U<<11);

	/*Set PSIZE*/
	DMA1_Channel1->CCR |= (1U<<8);
	DMA1_Channel1->CCR &= ~(1U<<9);

	/*Enable Memory and Peripheral Increment*/
	DMA1_Channel1->CCR |= DMA1_CCR_MINC | DMA1_CCR_PINC;

	/*Note: DMA1_CCR_DIR should not be set to Mem2Mem mode*/
	/*Clear DIR to ensure mem2mem transfer*/
	DMA1_Channel1->CCR |= DMA1_CCR_DIR;

	/*Enable Transfer Complete & Error Interrupts*/
	DMA1_Channel1->CCR |= DMA1_CCR_TCIE | DMA1_CCR_TEIE;

	/*Set priority to medium (optional, PL[1:0] = 01)*/
	DMA1_Channel1->CCR |= (1U<<12);
	DMA1_Channel1->CCR &= ~(1U<<13);

	/*Enable DMA1 Channel1 interrupt in NVIC*/
	NVIC_EnableIRQ(DMA1_Channel1_IRQn);

	/*Set Priority*/
	NVIC_SetPriority(DMA1_Channel1_IRQn, 0);
}


void dma_transfer_start(uint32_t src_buff, uint32_t dest_buff, uint32_t len)
{
	/*Set Source (memory) address*/
	DMA1_Channel1->CMAR = src_buff;

	/*Set Dest (memory) address*/
	DMA1_Channel1->CPAR = dest_buff;

	/*Set transfer length*/
	DMA1_Channel1->CNDTR = len;

	/*Enable DMA Stream*/
	DMA1_Channel1->CCR |= DMA1_CCR_EN;

}








