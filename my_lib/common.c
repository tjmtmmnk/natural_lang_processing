#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include "syntatic_analysis.h"
#include "lexical_analysis.h"

int error(int line_num, char *mes) {
    fprintf(stderr, "[ERROR] line%d : %s\n", line_num, mes);
    fflush(stderr);
    return NONE;
}

int errorWithReturn(int line_num, char *mes) {
    fprintf(stderr, "[ERROR] line%d : %s\n", line_num, mes);
    fflush(stderr);
    return ERROR;
}