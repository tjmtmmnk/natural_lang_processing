#include "common.h"
#include <stdio.h>
#include <stdlib.h>

void error(int line_num, char *mes) {
    fprintf(stderr, "[ERROR] line%d : %s\n", line_num, mes);
    fflush(stderr);
    exit(1);
}