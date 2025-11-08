#include "adc_dma.h"
#include "stm32f103xb.h"

#define GPIOAEN			(1U<<2)
#define ADC1EN			(1U<<9)
#define CR1_SCAN		(1U<<8)
#define CR2_DMA			(1U<<8)
#define CR2_CONT		(1U<<1)
#define CR2_ADON		(1U<<0)
#define CR2_SWSTART		(1U<<22)


#define 	DMA1EN				(1U<<0)
#define		DMA1_CCR_CIRC		(1U<<5)
#define 	DMA1_CCR_EN			(1U<<0)
#define 	DMA1_CCR_MEM2MEM	(1U<<14)
#define 	DMA1_CCR_MINC		(1U<<7)
#define 	DMA1_CCR_DIR		(1U<<4)
#define 	DMA1_CCR_TCIE		(1U<<1)
#define 	DMA1_CCR_TEIE		(1U<<3)

uint16_t adc_raw_data[number_of_channels];

void adc_dma_init(void)
{
	/**********GPIO Configuration**********/
	/*Enable clock access to ADC GPIO Pins*/
	RCC->APB2ENR |= GPIOAEN;
	/*Set PA0 & PA1 as GPIO Analog Mode*/
	GPIOA->CRL &= ~(0xFF); // Clear PA0, PA1 for analog mode (CNF = 00)

	/**********ADC Configuration**********/
	/*Enable clock access to ADC*/
	RCC->APB2ENR |= ADC1EN;
	/*Set sequence length*/
	ADC1->SQR1 |= (1U<<20); // Sequence length = 2
	/*Set sequence*/
	ADC1->SQR3 = (0U<<0) | (1U<<5); // Channel 0, 1

	/*Enable scan mode*/
	ADC1->CR1 = CR1_SCAN;

	/*Enable DMA continuous mode*/
	ADC1->CR2 |= CR2_DMA | CR2_CONT;

	/**********DMA Configuration**********/
	/*Enable clock access to DMA*/
	RCC->AHBENR |= DMA1EN;
	/*Disable DMA*/
	DMA1_Channel1->CCR &= ~DMA1_CCR_EN;
	/*Wait for DMA to disable*/
	while(DMA1_Channel1->CCR & DMA1_CCR_EN){}

	/*Clear Configuration register*/
	DMA1_Channel1->CCR = 0;

	/*Enable Circular mode*/
	DMA1_Channel1->CCR |= DMA1_CCR_CIRC;

	/*Enable mem increment*/
	DMA1_Channel1->CCR |= DMA1_CCR_MINC | DMA1_CCR_TCIE | DMA1_CCR_TEIE;

	/*Set mem transfer size*/
	DMA1_Channel1->CCR |= (1U<<10);


	/*Set periph transfer size*/
	DMA1_Channel1->CCR |= (1U<<8);





	/*Set periph address*/
	DMA1_Channel1->CPAR = (uint32_t) &(ADC1->DR);

	/*Set mem address*/
	DMA1_Channel1->CMAR = (uint32_t) adc_raw_data;

	/*Set number of transfer*/
	DMA1_Channel1->CNDTR = (uint16_t)number_of_channels;

	NVIC_EnableIRQ(DMA1_Channel1_IRQn);

	/*Enable DMA*/
	DMA1_Channel1->CCR |= DMA1_CCR_EN;

	/**********ADC Start**********/
	/*Enable ADC*/
	ADC1->CR2 |= CR2_ADON;

	/*Start Calibration*/
	ADC1->CR2 |= (1U<<3);
	while(ADC1->CR2 & (1U<<3)){}
	ADC1->CR2 |= CR2_ADON; // Ensure ADC is ON

	/*Start ADC*/
	ADC1->CR2 |= CR2_SWSTART;
}
void dma_Channel1_IRQHandler(void)
{
	if(DMA1->ISR & (1U<<1)){
		DMA1->IFCR |= (1U<<1); // Clear TC Flag
		// Optionally toggle a GPIO for debugging
	}
	if(DMA1->ISR & (1U<<3)){
		DMA1->IFCR |= (1U<<3); // Clear TE Falg
		// Handle Error
	}
}







