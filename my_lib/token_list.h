#ifndef NATURAL_LANG_PROCESS_TOKEN_LIST_H
#define NATURAL_LANG_PROCESS_TOKEN_LIST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_STR_SIZE 1024
#define NUM_OF_TOKEN 49
#define NUM_OF_SYMBOL 18
#define NUM_OF_KEY 28
#define NUM_OF_CONT_SYMBOL 4
#define NUM_OF_SPLIT 4

/* Token */
typedef enum e_name {
    NONE = -2,
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

typedef enum MODE {
    MODE_NONE,
    MODE_ALPHA_NUM,
    MODE_NUM,
    MODE_SYMBOL,
    MODE_CONT_SYMBOL,
    MODE_SPLIT,
    MODE_STRING,
    MODE_COMMENT
} Mode;
extern const struct KEY {
    char *keyword;
    int token_number;
} key[NUM_OF_KEY];

extern const struct SYMBOL {
    char *symbol;
    int token_number;
} symbol[NUM_OF_SYMBOL];

//連続するシンボル
extern const char cont_sym[NUM_OF_CONT_SYMBOL][3];
extern const char split[NUM_OF_SPLIT];
extern int token_counter[NUM_OF_TOKEN + 1];

/* string of each token */
extern char *token_str[NUM_OF_TOKEN + 1];


#endif //NATURAL_LANG_PROCESS_TOKEN_LIST_H
