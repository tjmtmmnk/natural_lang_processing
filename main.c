#include <stdio.h>
#include <stdlib.h>
#include "my_lib/lexical_analysis.h"

#define EXEC_MODE

int main(int nc, char *np[]) {
    if (nc < 2) {
        fprintf(stderr, "File name id not given.");
        return EXIT_FAILURE;
    }

    setFileName(np[1]);
    initScan();

    int token_code;
    while ((token_code = scanTokenOneEach()) != SCAN_END) {
        if (token_code >= 0) {
            token_counter[token_code]++;
        }
    }

#ifdef EXEC_MODE
    rep(i, 1, NUM_OF_TOKEN + 1) {
        if (token_counter[i] > 0) {
            printf("%-10s\t%28d\n", token_str[i], token_counter[i]);
        }
    }
    printf("\n");
    debugIDTable();
#endif

    closeFile();

    return EXIT_SUCCESS;
}
