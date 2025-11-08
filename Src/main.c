#include "stm32f103xb.h"
#include "adc_dma.h"
#include "uart.h"
#include <stdio.h>



extern uint16_t adc_raw_data[number_of_channels];



int main (void)
{

	/*Initialize UART*/
	uart2_tx_init();

	adc_dma_init();

	while(1)
	{
		printf("ADC0: %d \n\r", adc_raw_data[0]);
		printf("ADC1: %d \n\r", adc_raw_data[1]);
		        for (volatile int i = 0; i < 90000; i++){}; // Delay
	}
}









