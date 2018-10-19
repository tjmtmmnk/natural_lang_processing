#include "token_list.h"

char *split[NUM_OF_SPLIT] = {
        " ", "\t", "\r", "\n", "\r\n", "\n\r", "{", "}", "/*", "*/"
};

struct KEY key[KEY_WORD_SIZE] = {
        {"and",       TAND},
        {"array",     TARRAY},
        {"begin",     TBEGIN},
        {"boolean",   TBOOLEAN},
        {"break",     TBREAK},
        {"call",      TCALL},
        {"char",      TCHAR},
        {"div",       TDIV},
        {"do",        TDO},
        {"else",      TELSE},
        {"end",       TEND},
        {"false",     TFALSE},
        {"if",        TIF},
        {"integer",   TINTEGER},
        {"not",       TNOT},
        {"of",        TOF},
        {"or",        TOR},
        {"procedure", TPROCEDURE},
        {"program",   TPROGRAM},
        {"read",      TREAD},
        {"readln",    TREADLN},
        {"return",    TRETURN},
        {"then",      TTHEN},
        {"true",      TTRUE},
        {"var",       TVAR},
        {"while",     TWHILE},
        {"write",     TWRITE},
        {"writeln",   TWRITELN}
};

char *token_str[NUM_OF_TOKEN + 1] = {
        "",
        "NAME", "program", "var", "array", "of", "begin", "end", "if", "then",
        "else", "procedure", "return", "call", "while", "do", "not", "or", "div", "and", "char", "integer", "boolean",
        "readln", "writeln", "true",
        "false", "NUMBER", "STRING", "+", "-", "*", "=", "<>", "<", "<=", ">", ">=", "(", ")", "[", "]", ":=", ".", ",",
        ":", ";", "read", "write", "break"
};

int token_counter[NUM_OF_TOKEN + 1] = {0};