# Co-design
Co-develop system consisting of hardware (FPGA) and software (running on FreeRTOS on MCU).
System communicates via SPI.

Purpose:
1. Use software platform as interface to take commands from a user
2. Transmit commands via SPI to the FPGA board
3. Decode commands in FPGA hardware and perform needed actions (PWM).


The modular VHDL includes among other:
- System FSM to coordinate hardware synchronously
- SPI slave (FSMD) to receive commands
- Decoder
- 4 x 3 x 12-bit registerfile for the PWM duty values
- Testbenches for the hardware.

The folder *kl25z-source* contains code for the KL25Z-board. Compiles with NXP MCUXpressoIDE.

The folder *a7-source* contains modular VHDL and constraints for Artix-A7 (Arty). Can be synthesized using Vivado 2019.

