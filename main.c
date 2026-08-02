#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "first_pass.h"

int main(int argc, char *argv[]) {
    int i;
    
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename.am>\n", argv[0]);
        return 1;
    }

    printf("=== Starting First Pass for file: %s ===\n\n", argv[1]);

    /* הרצת המעבר הראשון */
    if (run_first_pass(argv[1])) {
        printf("\n>>> First Pass Completed Successfully! <<<\n");
        printf("Final IC (Instruction Counter) = %d\n", IC);
        printf("Final DC (Data Counter)        = %d\n\n", DC);

        /* 1. הדפסת תמונת הקוד (code_image) */
        printf("=========================================\n");
        printf("             CODE IMAGE (%d bytes)       \n", IC - 100);
        printf("=========================================\n");
        for (i = 100; i < IC; i += 4) {
            printf("Address %04d:  %02X %02X %02X %02X\n", 
                   i, 
                   code_image[i], 
                   code_image[i + 1], 
                   code_image[i + 2], 
                   code_image[i + 3]);
        }
        printf("\n");

        /* 2. הדפסת תמונת הנתונים (data_image) */
        printf("=========================================\n");
        printf("             DATA IMAGE (%d bytes)       \n", DC);
        printf("=========================================\n");
        for (i = 0; i < DC; i++) {
            printf("DC[%04d] (Absolute %04d):  0x%02X (%d)\n", 
                   i, 
                   IC + i, 
                   data_image[i], 
                   (signed char)data_image[i]);
        }
        printf("=========================================\n");

    } else {
        printf("\n>>> First Pass Failed due to errors in file. <<<\n");
    }

    return 0;
}
