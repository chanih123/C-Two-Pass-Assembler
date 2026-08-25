.entry STARTLABEL
.entry DATAARRAY
.extern EXTFUNC
.extern EXTVAR

mcro MATHOPS
    add $0, $31, $1
    sub $2, $3, $4
    and $5, $6, $7
    or  $8, $9, $10
    nor $11, $12, $13
mcroend

mcro FLOWOPS
    move $14, $15
    mvhi $16, $17
    mvlo $18, $19
mcroend

STARTLABEL: addi $1, -1, $2
             andi $3, 0, $4
             ori  $5, 32767, $6
             nori $7, -32768, $8
             
             ; Macro expansions
             MATHOPS
             FLOWOPS

LOOPPT:     bne  $1, $2, LOOPPT
             beq  $3, $4, ENDPROG
             blt  $5, $6, STARTLABEL
             bgt  $7, $8, DATAARRAY

             ; Memory access and jumps
             lb   $9, -4, $10
             sb   $11, 100, $12
             lw   $13, 0, $14
             sw   $15, 12, $16
             lh   $17, -2, $18
             sh   $19, 4, $20

             jmp  $21
             la   EXTVAR
             call EXTFUNC
             jmp  ENDPROG

ENDPROG:    hlt

; Data directives and edge values
STRING1:     .asciz ""
STRING2:     .asciz "Hello, World! 123 @#$"
DATABYTES:  .db -128, 127, 0, 15
DATAHALF:   .dh -32768, 32767, 0
DATAARRAY:  .dw -2147483648, 2147483647, 0, 100
