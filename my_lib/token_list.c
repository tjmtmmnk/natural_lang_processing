#include "token_list.h"

const struct KEY key[NUM_OF_KEY] = {
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

const struct SYMBOL symbol[NUM_OF_SYMBOL] = {
        {"+",  TPLUS},
        {"-",  TMINUS},
        {"*",  TSTAR},
        {"=",  TEQUAL},
        {"<>", TNOTEQ},
        {"<",  TLE},
        {"<=", TLEEQ},
        {">",  TGR},
        {">=", TGREQ},
        {"(",  TLPAREN},
        {")",  TRPAREN},
        {"[",  TLSQPAREN},
        {"]",  TRSQPAREN},
        {":=", TASSIGN},
        {".",  TDOT},
        {",",  TCOMMA},
        {":",  TCOLON},
        {";",  TSEMI}
};

const char cont_sym[NUM_OF_CONT_SYMBOL][3] = {
        "<>", "<=", ">=", ":="
};

const char split[NUM_OF_SPLIT] = {' ', '\t', '\n', '\r'};

char *token_str[NUM_OF_TOKEN + 1] = {
        "",
        "NAME", "program", "var", "array", "of", "begin", "end", "if", "then",
        "else", "procedure", "return", "call", "while", "do", "not", "or", "div", "and", "char", "integer", "boolean",
        "readln", "writeln", "true",
        "false", "NUMBER", "STRING", "+", "-", "*", "=", "<>", "<", "<=", ">", ">=", "(", ")", "[", "]", ":=", ".", ",",
        ":", ";", "read", "write", "break"
};

int token_counter[NUM_OF_TOKEN + 1] = {0};
