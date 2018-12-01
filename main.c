#include <stdio.h>
#include <stdlib.h>
#include "my_lib/lexical_analysis.h"
#include "my_lib/syntatic_analysis.h"

#define EXEC_MODE

int main(int nc, char *np[]) {
    if (nc < 2) {
        fprintf(stderr, "File name id not given.");
        return EXIT_FAILURE;
    }

    setFileName(np[1]);
    initScan();

    if(parseProgram() == OK){
        printf("\n!!!No Parse Error!!!\n");
    }

    closeFile();

    return EXIT_SUCCESS;
}
