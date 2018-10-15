#ifndef NATURAL_LANG_PROCESS_TOKEN_LIST_H
#define NATURAL_LANG_PROCESS_TOKEN_LIST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_STR_SIZE 1024
#define NUM_OF_TOKEN 49
#define KEY_WORD_SIZE 28

/* Token */
typedef enum e_name {
    TNAME = 1
} eName;

typedef enum e_keyword {
    TPROGRAM = 2,
    TVAR,
    TARRAY,
    TOF,
    TBEGIN,
    TEND,
    TIF,
    TTHEN,
    TELSE,
    TPROCEDURE,
    TRETURN,
    TCALL,
    TWHILE,
    TDO,
    TNOT,
    TOR,
    TDIV,
    TAND,
    TCHAR,
    TINTEGER,
    TBOOLEAN,
    TREADLN,
    TWRITELN,
    TTRUE,
    TFALSE,
    TREAD = 47,
    TWRITE,
    TBREAK
} eKeyword;

typedef enum e_unsigned_integer {
    TNUMBER = 27
} eUnsignedInteger;

typedef enum e_string {
    TSTRING = 28
} eString;

typedef enum e_symbol {
    TPLUS = 29,
    TMINUS,
    TSTAR,
    TEQUAL,
    TNOTEQ,     // <>
    TLE,        // <
    TLEEQ,      // <=
    TGR,        // >
    TGREQ,      // >=
    TLPAREN,    // (
    TRPAREN,    // )
    TLSQPAREN,  // [
    TRSQPAREN,  // ]
    TASSIGN,    // :=
    TDOT,
    TCOMMA,
    TCOLON,
    TSEMI,
} eSymbol;

extern struct KEY {
    char *keyword;
    int token_number;
} key[KEY_WORD_SIZE];

int token_counter[NUM_OF_TOKEN + 1];

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

/* string of each token */
char *token_str[NUM_OF_TOKEN + 1] = {
        "",
        "NAME", "program", "var", "array", "of", "begin", "end", "if", "then",
        "else", "procedure", "return", "call", "while", "do", "not", "or", "div", "and", "char", "integer", "boolean",
        "readln", "writeln", "true",
        "false", "NUMBER", "STRING", "+", "-", "*", "=", "<>", "<", "<=", ">", ">=", "(", ")", "[", "]", ":=", ".", ",",
        ":", ";", "read", "write", "break"
};


#endif //NATURAL_LANG_PROCESS_TOKEN_LIST_H
