#include <stdio.h>
#include <stdlib.h>
#include "lexical_analysis.h"

static FILE *fp;
static char *file_name;
static int c_buf;

void setFileName(char *_file_name) {
    file_name = _file_name;
}

void initScan() {
    openFile(file_name);
    c_buf = fgetc(fp);
}

void openFile() {
    if ((fp = fopen(file_name, "r")) == NULL) {
        fprintf(stderr, "Can't open %s", file_name);
        exit(EXIT_FAILURE);
    }
}

void closeFile() {
    fclose(fp);
}

// @return : 次のトークンコード
int scanTokenOneEach() {

}

int getLatestFoundTokenLine() {

}

