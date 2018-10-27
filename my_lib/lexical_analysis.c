#include <stdio.h>
#include <stdlib.h>
#include "lexical_analysis.h"

//#define DEBUG_MODE

static FILE *fp;
static char *file_name;
static int c_buf;
static int crnt_buf;
static int line_num;
int num_attr = NONE;
char str_attr[MAX_WORD_LENGTH];

static void openFile() {
    if ((fp = fopen(file_name, "r")) == NULL) {
        fprintf(stderr, "Can't open %s", file_name);
        exit(EXIT_FAILURE);
    }
}

static int isDigit(const char c) {
    return c >= '0' && c <= '9';
}

static int isAlphabet(const char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static int isSplit(const char c) {
    rep(i, 0, NUM_OF_SPLIT) {
        if (c == split[i]) {
            return 1;
        }
    }
    return 0;
}

static int isSymbol(const char c) {
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

static int isString(const char c) {
    return c == '\'';
}

static int isCommentSlash(const char c) {
    return ((c == '/') || (c == '*'));
}

static int isCommentBrace(const char c) {
    return ((c == '{') || (c == '}'));
}

// judge continuous symbol
static int isContSymbol(const char c[3]) {
    rep(i, 0, NUM_OF_CONT_SYMBOL) {
        if (strcmp(cont_sym[i], c) == 0) {
            return 1;
        }
    }
    return 0;
}

static int isEOF(const int c) {
    return c == -1;
}

/* @input : number or keyword or symbol or name
 * Don't allow exception in this function
 */
static int getTokenCode() {
#ifdef DEBUG_MODE
    printf("%s\n", str_attr);
#endif
    if (num_attr >= 0) {
        return TNUMBER;
    }

    rep(i, 0, NUM_OF_KEY) {
        if (strcmp(str_attr, key[i].keyword) == 0) {
            return key[i].token_number;
        }
    }

    rep(j, 0, NUM_OF_SYMBOL) {
        if (strcmp(str_attr, symbol[j].symbol) == 0) {
            return symbol[j].token_number;
        }
    }

    countUpID(str_attr);
    return TNAME;
}

static void lineCountUp() {
    if (crnt_buf == '\r') {
        line_num++;
    }
    if (crnt_buf == '\n') {
        line_num++;
    }
    if ((crnt_buf == '\r' && c_buf == '\n') || (crnt_buf == '\n' && c_buf == '\r')) {
        line_num--;
    }
}

static int getLineNum() {
    return line_num;
}

static void clearBuf() {
    rep(i, 0, MAX_WORD_LENGTH) {
        str_attr[i] = '\0';
    }
    num_attr = NONE;
}

static Mode getMode(int _crnt_buf, int _c_buf) {
    if (isAlphabet(_crnt_buf)) {
        return MODE_ALPHA_NUM;
    } else if (isDigit(_crnt_buf)) {
        return MODE_NUM;
    } else if (isSplit(_crnt_buf)) {
        return MODE_SPLIT;
    } else if (isSymbol(_crnt_buf) && isSymbol(_c_buf)) {
        char temp[3] = {'\0'};
        temp[0] = _crnt_buf;
        temp[1] = _c_buf;
        if (isContSymbol(temp)) {
            return MODE_CONT_SYMBOL;
        } else {
            return MODE_SYMBOL;
        }
    } else if (isSymbol(_crnt_buf)) {
        return MODE_SYMBOL;
    } else if (isString(_crnt_buf)) {
        return MODE_STRING;
    } else if (isCommentSlash(_crnt_buf) && isCommentSlash(_c_buf)) {
        return MODE_COMMENT_SLASH;
    } else if (isCommentBrace(_crnt_buf)) {
        return MODE_COMMENT_BRACE;
    } else {
        error(getLineNum(), "Invalid word");
    }
    return MODE_NONE;
}

void setFileName(char *_file_name) {
    file_name = _file_name;
}

static void updateBuf(int times) {
    rep(i, 0, times) {
        crnt_buf = c_buf;
        c_buf = fgetc(fp);
    }
}

void initScan() {
    openFile();
    line_num = 1;
    c_buf = fgetc(fp);
    clearBuf();
    crnt_buf = c_buf;
    c_buf = fgetc(fp);
}

void closeFile() {
    fclose(fp);
}

/* @return : token_code
 * if failed return SCAN_END
 */

int scanTokenOneEach() {
    Mode mode = MODE_NONE;
    int token_code = SCAN_END;
    int buf_i = 0;
    /* branch off first character
     * process in each mode
     */
    while (crnt_buf != EOF || c_buf != EOF) {
        mode = getMode(crnt_buf, c_buf);

        switch (mode) {
            case MODE_ALPHA_NUM:
                while (isAlphabet(crnt_buf) || isDigit(crnt_buf)) {
                    str_attr[buf_i++] = crnt_buf;
                    updateBuf(1);
                }
                break;

            case MODE_NUM:
                while (isDigit(crnt_buf)) {
                    str_attr[buf_i++] = crnt_buf;
                    updateBuf(1);
                }
                sscanf(str_attr, "%d", &num_attr);
                if (num_attr > UNSIGNED_INT_MAX) {
                    error(getLineNum(), "Over num range(0~32767)");
                }
                break;

            case MODE_SYMBOL:
                str_attr[buf_i++] = crnt_buf;
                updateBuf(1);
                break;

            case MODE_CONT_SYMBOL:
                str_attr[buf_i++] = crnt_buf;
                str_attr[buf_i++] = c_buf;
                updateBuf(2);
                break;

            case MODE_STRING:
                updateBuf(1); // get into '''
                while (1) {
                    if (isEOF(crnt_buf)) {
                        error(getLineNum(), "Don't close string");
                    }

                    if (isString(crnt_buf) && !isString(c_buf)) {
                        updateBuf(1);

                        if (buf_i > (MAX_WORD_LENGTH - 1)) {
                            error(getLineNum(), "Too long words in string");
                        }

                        return TSTRING;
                    } else if (isString(crnt_buf) && isString(c_buf)) { // correspond to ''' in string
                        updateBuf(2);
                        buf_i += 2; //count apostrophes one by one
                    } else {
                        updateBuf(1);
                        buf_i++;
                    }
                }

            case MODE_SPLIT:
                lineCountUp();
                updateBuf(1);
                return NONE;

            case MODE_COMMENT_SLASH:
                updateBuf(2); //get into comment
                while (1) {
                    if (isCommentSlash(crnt_buf) && isCommentSlash(c_buf)) {
                        break;
                    }
                    if (isEOF(crnt_buf)) {
                        error(getLineNum(), "Don't close comment");
                    }
                    lineCountUp(); //count up line_num if there is new line character in comment
                    updateBuf(1);
                }
                updateBuf(2); //get out comment
                return NONE;

            case MODE_COMMENT_BRACE:
                updateBuf(1);
                while (1) {
                    if (isCommentBrace(crnt_buf)) {
                        break;
                    }
                    if (isEOF(crnt_buf)) {
                        error(getLineNum(), "Don't close comment");
                    }
                    lineCountUp();
                    updateBuf(1);
                }
                updateBuf(1);
                return NONE;
            default:
                break;

        }
        if (buf_i == 0) {
            updateBuf(1);
        }

        if (buf_i > (MAX_WORD_LENGTH - 1)) {
            error(getLineNum(), "Too long words");
        }

        token_code = getTokenCode();
        clearBuf();

        if (token_code > 0) {
            return token_code;
        }
    }
    return token_code;
}
