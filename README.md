# Design of embedded systems using SOCs, FPGAs and SBCs
This repo contains code, designs and reports/journals from various projects from the courses:
- E2DEL (Digital Elektronik - digital electronics),
- E3ISD1 and E4ISD2 (Indlejret Systemdesign - design of embedded systems), and
- E4BIS (Brugeroplevelser i Indlejrede Systemer - user experience with embedded systems).

Hardware used is:
- Freedom KL25Z board from NXP with a Kinetis SOC based on the Arm Cortex M0+ core, bare-metal and with FreeRTOS.
- The Arty A7 FPGA board with the Artix-7 FPGA.
- The PocketBeagle SBC using the Sitara AM335x processor, running headless Debian Linux.
- Various additional off-the-shelf components.

Key concepts and technologies are:
- Development based on system requiremenents.
- SOC-related:
 - Programming for the M0+ ISA using the CMSIS Hardware Abstraction Layer (HAL)
 - Using interrupts efficiently
 - Using peripherals efficiently (DMA, Timers, ADC, DAC, SPI, GPIO, etc.)
 - Programming for real-time systems using FreeRTOS
 - Mainly C-programming.
- FPGA-related:
 - Designing synchronous systems with clocked design and finite state-machines (FSM, FSMD)
 - Describing hardware using VHDL
 - Concurrent co-design of hardware and software.
- SBC-related:
 - Systems Programming
 - Network programming
 - Client-server architecture
 - Using Sitara peripherals, e.g. programmable realtime units (PRU), CAN-bus interface (and SocketCAN), etc.
 - Interfacing with the physical world via sensors and regulators
 - Mainly C++ programming (Modern C++, so in the style of C++11, C++14, C++17).

The pdf files in each folder are the main reports/journals, documenting the methods, design, results and learnings/future improvements.
