        .ORIG x3000

        LEA R6, DATA         ; LEA
        LD R0, NUM1          ; LD
        LDI R1, NUM1_PTR     ; LDI
        LDR R2, R6, #0       ; LDR (R6 Ö¸Ïò DATA)
        ST R0, SAVE1         ; ST
        STI R1, SAVE2_PTR    ; STI
        STR R2, R6, #1       ; STR

        ADD R3, R0, #5       ; ADD
        AND R4, R1, R2       ; AND
        NOT R5, R3           ; NOT

        BRzp SKIP            ; BR
        JSR SUBROUTINE       ; JSR10
	ADD R6, R6, #-2
SKIP    JMP R6               ; JMP 

        TRAP x25             ; HALT

SUBROUTINE
        ADD R0, R0, #1
        RET

DATA    .FILL xABCD	; x3011
NUM1    .FILL x0005
NUM1_PTR .FILL NUM1
SAVE1   .BLKW 1
SAVE2_PTR .FILL SAVE2
SAVE2  .BLKW 1

        .END
