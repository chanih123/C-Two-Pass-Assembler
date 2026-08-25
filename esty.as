; ===================================================
; comprehensive_fails_final.as
; comprehensive_fails_final.as
; Comprehensive first pass test file covering ALL edge cases.
; All lines are strictly under 70 characters (safe).
; ===================================================
   
; --- Group 1: Label Syntax Errors (Error 44) ---
1LABEL:     add $1, $2, $3
MY_LABEL:   sub $4, $5, $6
THISLABELISWAYTOOLONGFORTHEASSEMBLERTOHANDLE: jmp MAIN
add:        nor $1, $2, $3
$5:         or $1, $2, $3
.db:        and $1, $2, $3
 BADLABEL :  add $1, $2, $3
  
; --- Group 2: Opcode & Comma Errors ---
invalidop   $1, $2, $3
,           add $1, $2, $3
add ,       $1, $2, $3
 add         $1,, $2, $3
 add         $1, $2, $3,
 .db ,       1, 2, 3
.db         1, 2, 3,
move        $1 $2
 .db         1 2 3
    
; --- Group 3: Register Errors (Error 46) ---
add         $32, $1, $2
  add         r5, $1, $2
= 
; --- Group 4: Operand & Argument Errors ---
 add         $1, $2
 move        $1
  addi        $1, 5
 hlt         extra_junk
add         $1, $2, $3 extra_junk
  .entry      MAIN extra_junk

; --- Group 5: Operand Type Mismatch ---
 add         $1, 10, $2
  addi        $1, $2, $3
   la          $1

; --- Group 6: Numeric Range Violations (Error 57) ---
    addi        $1, 40000, $2
        subi        $1, -35000, $2
    .db         128
        .db         -129
    .dh         32768
        .dh         -32769
        .dw         2147483648
        .dw         -2147483649
        
        ; --- Group 7: Directive & String Errors ---
        db          5, 6, 7
    .invalid    1, 2, 3
        .db         5.5
    .db         5a
        .asciz      hello"
        .asciz      "hello
        .asciz      "hello" extra_text_here
        .entry      1LABEL
        .extern     EXT_ERR
       
        ; --- Group 8: Semantic & Table Conflicts (Errors 55, 56) ---
        ; Using perfectly legal labels to bypass lexical checks
        DUPLABEL:   add $1, $2, $3
        DUPLABEL:   sub $1, $2, $3
       .extern     EXTLOCALTROUBLE
        EXTLOCALTROUBLE: add $1, $2, $3
        .extern     EXTENTTROUBLE
        .entry      EXTENTTROUBLE
        LOCALEXTTROUBLE: add $1, $2, $3
       .extern     LOCALEXTTROUBLE
