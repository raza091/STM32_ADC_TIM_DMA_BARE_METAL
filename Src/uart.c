#include "uart.h"

#define UART2EN     (1U << 17)
#define GPIOAEN     (1U << 2)
#define CR1_TE      (1U << 3)
#define CR1_UE      (1U << 13)
#define SR_TXE      (1U << 7)
#define UART_BAUDRATE 115200
#define CLK         36000000 // Adjust if your system clock differs

static uint16_t compute_uart_bd(uint32_t periph_clk, uint32_t baudrate);
static void uart_set_baudrate(uint32_t periph_clk, uint32_t baudrate);
static void uart2_write(int ch);

int __io_putchar(int ch) {
    uart2_write(ch);
    return ch;
}

void uart2_tx_init(void) {
    // Enable clock access to GPIOA
    RCC->APB2ENR |= GPIOAEN;

    // Configure PA2 as alternate function push-pull for UART2_TX
    GPIOA->CRL &= ~(0xF << 8); // Clear bits 8-11
    GPIOA->CRL |= (0xB << 8);  // Set to 1011 (output 50 MHz, AF push-pull)

    // Enable clock access to UART2
    RCC->APB1ENR |= UART2EN;

    // Set baud rate
    uart_set_baudrate(CLK, UART_BAUDRATE);

    // Configure UART2: enable transmitter and UART
    USART2->CR1 = 0; // Clear CR1
    USART2->CR1 |= CR1_TE | CR1_UE;
}

static void uart2_write(int ch) {
    // Wait until TXE (transmit data register empty)
    while (!(USART2->SR & SR_TXE)) {}

    // Write to data register
    USART2->DR = (ch & 0xFF);
}

static uint16_t compute_uart_bd(uint32_t periph_clk, uint32_t baudrate) {
    float usartdiv = (float)periph_clk / (16 * baudrate);
    uint16_t mantissa = (uint16_t)usartdiv;
    uint16_t fraction = (uint16_t)((usartdiv - mantissa) * 16 + 0.5);
    return (mantissa << 4) | (fraction & 0xF);
}

static void uart_set_baudrate(uint32_t periph_clk, uint32_t baudrate) {
    USART2->BRR = compute_uart_bd(periph_clk, baudrate);
}
