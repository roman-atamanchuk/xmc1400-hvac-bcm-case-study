# XMC1400 Blower Speed Control Module (Bare-Metal)

HVAC body-control case study on the Infineon XMC1400 Boot Kit, written in C with DAVE IDE.

## What it does
- Polls CAN message 0x23A for the requested blower speed and shows it on the boot-kit LEDs (P4.0-P4.2)
- Runs as a bare-metal polling loop (no RTOS, no interrupts) to keep timing simple and predictable
- Measures worst-case loop time with a hardware timer, including timer wrap-around handling
- Optional test mode, built in only when enabled at compile time

## Hardware and tools
- Infineon XMC1400 Boot Kit
- DAVE IDE, C

## Related labs
- [Lab 1: interrupt-driven CAN](https://github.com/roman-atamanchuk/xmc1400-lab1-can-interrupts)
- [Lab 2: FreeRTOS with PWM](https://github.com/roman-atamanchuk/xmc1400-lab2-freertos-pwm)
- [Lab 3: FreeRTOS message queues](https://github.com/roman-atamanchuk/xmc1400-lab3-rtos-queues)
