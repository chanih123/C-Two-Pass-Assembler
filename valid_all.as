.entry START
.extern extlbl
mcro MYMACRO
    la extlbl
    move $2, $4
mcroend
START: add $1, $2, $3
       MYMACRO
       addi $5, -10, $6
       bne $1, $2, LOOP
       jmp $7
LOOP:  call extlbl
       hlt
DATA1: .db 10, -20, 30
DATA2: .dh 1000, 2000
DATA3: .dw 100000, -200000
STR:   .asciz "Hello"
