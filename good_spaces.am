; Valid program (situation 2): extra whitespace, more I-type ops.
; No .entry and an unused .extern.
; Expect: .am + .ob. No .ent. No .ext.

.extern unusedX

NUMS: .db 1, -2, 3
HALF: .dh -1
WORD: .dw 0
MSG:  .asciz "Ok"

START: addi $1, 8, $2
       subi $2, 1, $3
       nori $4, 0, $5
       sb $6, 2, $7
       lw $8, 0, $9
       add $3, $5, $9
       move $20, $4
       jmp $4
       hlt
