; Valid program (situation 3): macros, .entry after the label, extern used in J.
; Expect: .am + .ob + .ent (LOOP) + .ext (val1 at the la).

.extern val1

mcro LOAD
la val1
mcroend

MAIN: add $3,$5,$9
LOOP: ori $9,-5,$2
      LOAD
      jmp DONE

.entry LOOP

DONE: hlt
