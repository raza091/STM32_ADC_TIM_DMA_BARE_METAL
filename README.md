# STM32 Multi-Channel ADC with DMA (Bare-Metal)

Two-channel analog acquisition on an STM32F103RB using only CMSIS register access. No HAL, no LL drivers. The ADC runs continuously in scan mode and the DMA controller writes every conversion into RAM in circular mode, so the CPU never touches the ADC data register.

![MCU](https://img.shields.io/badge/MCU-STM32F103RB-03234B)
![Language](https://img.shields.io/badge/Language-C-blue)
![Style](https://img.shields.io/badge/Style-bare--metal%20CMSIS-lightgrey)

## What it does

| Block | Configuration |
|---|---|
| ADC1 | Channels 0 and 1 (PA0, PA1), scan mode, continuous conversion, software start, self-calibration before start |
| DMA1 Channel 1 | Peripheral-to-memory, 16-bit transfers, circular mode, memory increment, transfer-complete and transfer-error interrupts |
| USART2 | TX only on PA2, 115200 baud, `printf` retargeted through `syscalls.c` |
| Main loop | Prints both channel values from `adc_raw_data[]` |

```
PA0 ──┐                         ┌──────────────┐
      ├─► ADC1 (scan, cont.) ─► │ ADC1->DR     │
PA1 ──┘                         └──────┬───────┘
                                       │ DMA1 Ch1 (circular)
                                       ▼
                              adc_raw_data[2]  ──► printf ──► USART2 TX (PA2)
```

## Source layout

```
Src/main.c        application loop, prints ADC values
Src/adc_dma.c     ADC1 + DMA1 Channel 1 setup and IRQ handler
Src/uart.c        USART2 TX driver and baud-rate calculation
Src/syscalls.c    newlib stubs so printf() writes to the UART
Inc/              headers for the modules above
Startup/          startup_stm32f103rbtx.s
STM32F103RBTX_FLASH.ld   linker script
chip_headers/     CMSIS core and STM32F1xx device headers (ARM / STMicroelectronics)
```

## Build and run

1. Open the folder as an existing project in STM32CubeIDE (the `.cproject`/`.project` files are included), or build with `arm-none-eabi-gcc` using the linker script and startup file above.
2. Flash an STM32F103RB board over ST-LINK.
3. Connect PA0 and PA1 to analog sources (a potentiometer works) and open a serial terminal on the USART2 TX pin.

Expected output:

```
ADC0: 2048
ADC1: 1023
```

## Notes

- `CLK` in `uart.c` is set to 36 MHz (APB1). If your clock tree differs, change it or the baud rate will be off.
- The 12-bit result is right-aligned, so values range from 0 to 4095.

## Related

- [STM32_DMA_MEM2MEM_BARE_METAL](https://github.com/raza091/STM32_DMA_MEM2MEM_BARE_METAL): DMA memory-to-memory transfer with an interrupt flag, same toolchain and layout.

## Author

Ali Raza Tariq. Embedded systems, PCB design and digital IC design. [LinkedIn](https://www.linkedin.com/in/ali-raza-embedded-fpga) · [GitHub](https://github.com/raza091)
