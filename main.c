#include <stdio.h>
#include <stdlib.h>
#include "my_lib/lexical_analysis.h"
#include "my_lib/syntatic_analysis.h"
#include "my_lib/output_objectfile.h"

#define EXEC_MODE

int main(int nc, char *np[]) {
    if (nc < 2) {
        fprintf(stderr, "File name id not given.");
        return EXIT_FAILURE;
    }

    setFileName(np[1]);
    initScan();

    setOutputFileName(np[1]);

    initializeCompiler();

    if (parseProgram() == OK) {
        printf("\n!!!No Parse Error!!!\n");
    }

    closeFile();

    finalizeCompiler();
    return EXIT_SUCCESS;
}
