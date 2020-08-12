        .cdecls "perfTestContainer.c"

        .clink
        .global start
start:
        CLR     r30, r30.t10     ; start with GPIO low
CHECK:
        XIN     10, &r0, 4       ; read from scratchpad bank 0
        MOV     r30, r0          ; Move value from scratchpad into GPIO out
        JMP     CHECK            ; loop
        HALT                     ; done

