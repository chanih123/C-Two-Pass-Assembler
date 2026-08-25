; Name-validation edge cases.
; Intended checks:
; 1) illegal label names
; 2) illegal macro names
; 3) identical macro and label names
; 4) opcode/directive/register-like names reused as symbols

1BAD: add $1,$2,$3
add:  hlt
.entry add

mcro 2MAC
move $1,$2
mcroend

mcro SAME
add $3,$4,$5
mcroend

SAME: ori $1,-1,$2

$7: hlt
