#include "common.h"
#include <stdio.h>
#include <stdlib.h>

void error(char *mes) {
    fprintf(stderr, "ERROR: %s", mes);
    fflush(stderr);
    exit(1);
}