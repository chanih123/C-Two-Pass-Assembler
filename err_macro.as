; Pre-assembler error: extra text on mcro.
; Expect: error, no first/second pass, no .ob/.ent/.ext.

mcro GEN extra
add $1,$2,$3
mcroend

MAIN: hlt
