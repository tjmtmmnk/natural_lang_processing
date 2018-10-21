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
    return c_buf >= '0' && c_buf <= '9';
}

static int isAlphabet(char c) {
    return (c_buf >= 'a' && c_buf <= 'z') || (c_buf >= 'A' && c_buf <= 'Z');
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
}

void closeFile() {
    fclose(fp);
}

// @return : トークンコード, 失敗したら-1
int scanTokenOneEach() {
    int token_code = -1;
    int buf_i = 0;
    int has_spilit = 0;
    crnt_buf = c_buf;
    c_buf = fgetc(fp);

    while (!has_spilit) {
        switch (c_buf) {
            case ' ':
            case '\t':
                crnt_buf = c_buf;
                c_buf = fgetc(fp);
                break;
            case '\r': {
                char next = fgetc(fp);
                crnt_buf = c_buf;
                c_buf = next;
                if (next == '\n') {
                    c_buf = fgetc(fp);
                }
                break;
            }
            case '\n': {
                char next = fgetc(fp);
                crnt_buf = c_buf;
                c_buf = next;
                if (next == '\r') {
                    c_buf = fgetc(fp);
                }
                break;
            }
            case '{': {
                char next;
                while ((next = fgetc(fp)) != '}');
                c_buf = fgetc(fp);
                break;
            }
            case '\'': {
                char next;
                while ((next = fgetc(fp)) != '\'');
                c_buf = fgetc(fp);
                break;
            }
            case '/': {
                char next = fgetc(fp);
                if (next == '*') {
                    while ((next = fgetc(fp)) != '/');
                    c_buf = fgetc(fp);
                }
                break;
            }
            default:
                printf("crnt : %c\tc : %c\n", crnt_buf, c_buf);
                if (c_buf == EOF) {
                    return SCAN_END;
                } else if (isAlphabet(crnt_buf)) {
                    if (isDigit(c_buf)) {
                        if (buf_i > MAX_WORD_LENGTH) {
                            error("Over words length range(1~1000)");
                        }
                        has_spilit = 1;
                    } else {
                        str_attr[buf_i++] = crnt_buf;
                    }
                } else if (isDigit(crnt_buf)) {
                    if (isAlphabet(c_buf)) {
                        sscanf(str_attr, "%d", &num_attr);
                        if (num_attr > UNSIGNED_INT_MAX) {
                            error("Over num range(0~32767)");
                        }
                        has_spilit = 1;
                    } else {
                        str_attr[buf_i++] = crnt_buf;
                    }
                } else if (isSymbol(crnt_buf) && isSymbol(c_buf)) {
                    str_attr[0] = crnt_buf;
                    str_attr[1] = c_buf;
                    has_spilit = 1;
                } else if (isSymbol(crnt_buf)) {
                    str_attr[0] = crnt_buf;
                    has_spilit = 1;
                }
                crnt_buf = c_buf;
                c_buf = fgetc(fp);
                break;
        }
    }
    token_code = getTokenCode();
    clearBuf();

    return token_code;
}

int getLatestFoundTokenLine() {
    return 0;
}

