        .cdecls "perfTestContainer.c"

        .clink
        .global start
start:
        CLR     r30, r30.t5      ; turn off the LED
        SET     r0, r0.t10       ; Initialize value to send to PRU1
CHECK:
        WBS     r31, 3           ; wait for bit set on GPIO in
        XOUT    10, &r0, 4       ; Send value to PRU1
        CLR     r0, r0.t10       ; Prepare the next value to send to PRU1
        WBC     r31, 3           ; wait until the bit is cleared again
        XOUT    10, &r0, 4       ; Send value to PRU1
        SET     r0, r0.t10       ; Prepare value for next round to PRU1
        JMP     CHECK            ; Repeat
        HALT                     ; done

;        SET     r30, r30.t5      ; set the LED, disabled for performance
;        CLR     r30, r30.t5      ; clear the LED, disabled for performance
