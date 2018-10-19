#include <stdio.h>
#include <stdlib.h>
#include "lexical_analysis.h"
#include "token_list.h"

static FILE *fp;
static char *file_name;
static int c_buf;
static int crnt_buf;
static char buf[MAX_WORD_LENGTH];

static void openFile() {
    if ((fp = fopen(file_name, "r")) == NULL) {
        fprintf(stderr, "Can't open %s", file_name);
        exit(EXIT_FAILURE);
    }
}

void setFileName(char *_file_name) {
    file_name = _file_name;
}

void initScan() {
    openFile();
    c_buf = fgetc(fp);
}

void closeFile() {
    fclose(fp);
}

// @return : 次のトークンコード
int scanTokenOneEach() {
    int token_code = -1;
    crnt_buf = c_buf;
    while ((c_buf = fgetc(fp)) != EOF) {
        for (int si = 0; si < NUM_OF_SPLIT; si++) {
            if (c_buf == split[si]) {
                return buf;
            }
        }
        crnt_buf = c_buf;
    }
    return 0;
}

static void clearBuf() {
    for (int i = 0; i < MAX_WORD_LENGTH; i++) {
        buf[i] = '\0';
    }
}

int getLatestFoundTokenLine() {
    return 0;
}

