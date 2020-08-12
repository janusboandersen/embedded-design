/*
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    MKL25Z128xxx4_wk37_1.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */
#include "keys.h"
#include "rgbLed.h"
#include "LCD_4bit.h"


/*
 * @brief   Application entry point.
 */
int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    PRINTF("E3ISD1 wk37 lab 1: GPIO and LCD.\n");

    init_rgb();
    setup_keys();
    //test_keys(); //infinite test loop

    //Use the library function to initialize and clear the LCD (includes MCU port init)
    Init_LCD(); //not lcd_init() as in lab note
    Clear_LCD(); //not lcd_clear() as in lab note
    Set_Cursor(0, 0);
    Print_LCD("E3ISD1 Singapore");

    unsigned key[NUM_KEYS] = {0}; //array to captures key presses

    /* Enter an infinite loop, which reads the buttons */
    while(1) {

    	read_keys(key);
    	control_rgb(key[SWUp], key[SWRt], key[SWLt]);

    	//Display the key directions
    	Set_Cursor(0, 1);
    	if (key[SWUp]) {
    		Print_LCD("->  Up    ");
    	}
    	if (key[SWDn]) {
    		Print_LCD("->  Down  ");
    	}
    	if (key[SWLt]) {
    	 	Print_LCD("->  Left  ");
    	}
    	if (key[SWRt]) {
    		Print_LCD("->  Rght  ");
    	}
    	if (key[SWCr]) {
    	    Print_LCD("->  Cntr  ");
    	}

        /* 'Dummy' NOP to allow source level single stepping of
            tight while() loop */
    //    __asm volatile ("nop");
    }
    return 0 ;
}
