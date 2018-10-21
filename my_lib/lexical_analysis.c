#include <stdio.h>
#include <stdlib.h>
#include "lexical_analysis.h"
#include "token_list.h"
#include "common.h"

static FILE *fp;
static char *file_name;
static int c_buf;
static int crnt_buf;
int num_attr = -1;
char str_attr[MAX_WORD_LENGTH];

static void openFile() {
    if ((fp = fopen(file_name, "r")) == NULL) {
        fprintf(stderr, "Can't open %s", file_name);
        exit(EXIT_FAILURE);
    }
}

static int isDigit(char c) {
    return c >= '0' && c <= '9';
}

static int isAlphabet(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static int isSplit(char c) {
    for (int i = 0; i < NUM_OF_SPLIT; ++i) {
        if (c == split[i]) {
            return 1;
        }
    }
    return 0;
}

static int isSymbol(char c) {
    switch (c) {
        case '+':
        case '-':
        case '*':
        case '=':
        case '<':
        case '>':
        case '(':
        case ')':
        case '[':
        case ']':
        case '.':
        case ',':
        case ':':
        case ';':
            return 1;
        default:
            return 0;
    }
}

//連続するシンボルの判定
static int isContSymbol(char c[2]) {
    for (int i = 0; i < NUM_OF_CONT_SYMBOL; ++i) {
        if (strcmp(cont_sym[i], c) == 0) {
            return 1;
        }
    }
    return 0;
}

/* @input : 数字 or キーワード or シンボル or 名前
 * ここでは例外的な入力は認めない
 */
static int getTokenCode() {

    printf("%s\n", str_attr);
    if (num_attr > 0) {
        return TNUMBER;
    }

    for (int i = 0; i < NUM_OF_KEY; ++i) {
        if (strcmp(str_attr, key[i].keyword) == 0) {
            return key[i].token_number;
        }
    }

    for (int i = 0; i < NUM_OF_SYMBOL; ++i) {
        if (strcmp(str_attr, symbol[i].symbol) == 0) {
            return symbol[i].token_number;
        }
    }

    return TNAME;
}

static void clearBuf() {
    for (int i = 0; i < MAX_WORD_LENGTH; i++) {
        str_attr[i] = '\0';
    }
    num_attr = 0;
}

void setFileName(char *_file_name) {
    file_name = _file_name;
}

void initScan() {
    openFile();
    c_buf = fgetc(fp);
    clearBuf();
    crnt_buf = c_buf;
    c_buf = fgetc(fp);
}

void closeFile() {
    fclose(fp);
}

// @return : トークンコード, 失敗したら-1
int scanTokenOneEach() {
    int token_code = -1;
    int buf_i = 0;
    int has_spilit = 0;

    while (!has_spilit && crnt_buf != EOF) {
        switch (c_buf) {
            case ' ':
                if (!isSplit(crnt_buf)) {
                    str_attr[buf_i] = crnt_buf;
                    has_spilit = 1;
                }
                c_buf = fgetc(fp);
                crnt_buf = c_buf;
                c_buf = fgetc(fp);
                break;
            case '\t':
                has_spilit = 1;
                break;
            case '\r': {
                if (!isSplit(crnt_buf)) {
                    str_attr[buf_i] = crnt_buf;
                    char next = fgetc(fp);

                    if (next == '\n') {
                        crnt_buf = fgetc(fp);
                        c_buf = fgetc(fp);
                    } else if (next != EOF) {
                        c_buf = fgetc(fp);
                        crnt_buf = next;
                    } else {
                        return SCAN_END;
                    }
                    has_spilit = 1;
                    break;
                } else {
                    crnt_buf = c_buf;
                    c_buf = fgetc(fp);
                    break;
                }
            }
            case '\n': {
                if (!isSplit(crnt_buf)) {
                    str_attr[buf_i] = crnt_buf;
                    printf("%c\n", crnt_buf);
                    char next = fgetc(fp);

                    if (next == '\r') {
                        crnt_buf = fgetc(fp);
                        c_buf = fgetc(fp);
                    } else if (next != EOF) {
                        c_buf = fgetc(fp);
                        crnt_buf = next;
                    } else {
                        return SCAN_END;
                    }
                    has_spilit = 1;
                    break;
                } else {
                    crnt_buf = c_buf;
                    c_buf = fgetc(fp);
                    break;
                }
            }
            case '{': {
                char next;
                while ((next = fgetc(fp)) != '}');
                c_buf = fgetc(fp);
                has_spilit = 1;
                break;
            }
            case '\'': {
                char next;
                while ((next = fgetc(fp)) != '\'');
                crnt_buf = fgetc(fp);
                c_buf = fgetc(fp);
                token_code = TSTRING;
                has_spilit = 1;
                break;
            }
            case '/': {
                char next = fgetc(fp);
                if (next == '*') {
                    char temp;
                    while ((temp = fgetc(fp)) != '/');
                    crnt_buf = fgetc(fp);
                    c_buf = fgetc(fp);
                }
                break;
            }
            default:
                if (c_buf == EOF) {
                    return SCAN_END;
                } else if (isAlphabet(crnt_buf)) {
                    if (isAlphabet(c_buf) || isDigit(c_buf) || isSymbol(c_buf)) {
                        str_attr[buf_i++] = crnt_buf;
                        if (isSymbol(c_buf)) {
                            has_spilit = 1;
                        }
                    } else {
                        if (buf_i > MAX_WORD_LENGTH) {
                            error("Over words length range(1~1000)");
                        }
                        has_spilit = 1;
                    }
                } else if (isDigit(crnt_buf)) {
                    if (isDigit(c_buf)) {
                        str_attr[buf_i++] = crnt_buf;
                    } else {
                        sscanf(str_attr, "%d", &num_attr);
                        if (num_attr > UNSIGNED_INT_MAX) {
                            error("Over num range(0~32767)");
                        }
                        has_spilit = 1;
                    }
                } else if (isSymbol(crnt_buf) && isSymbol(c_buf)) {
                    str_attr[0] = crnt_buf;
                    str_attr[1] = c_buf;
                    if (isContSymbol(str_attr)) {
                        crnt_buf = fgetc(fp);
                        c_buf = fgetc(fp);
                        has_spilit = 1;
                        break;
                    }
                } else if (isSymbol(crnt_buf)) {
                    str_attr[0] = crnt_buf;
                    str_attr[1] = '\0';
                    has_spilit = 1;
                }
                crnt_buf = c_buf;
                c_buf = fgetc(fp);
                break;
        }
    }

    if (token_code < 0) {
        token_code = getTokenCode();
    }

    clearBuf();

    return
            token_code;
}

int getLatestFoundTokenLine() {
    return 0;
}

