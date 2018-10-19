#ifndef NATURAL_LANG_PROCESS_TOKEN_LIST_H
#define NATURAL_LANG_PROCESS_TOKEN_LIST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_STR_SIZE 1024
#define NUM_OF_TOKEN 49
#define KEY_WORD_SIZE 28
#define NUM_OF_SPLIT 10
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

extern char *split[NUM_OF_SPLIT];

extern int token_counter[NUM_OF_TOKEN + 1];

/* string of each token */
extern char* token_str[NUM_OF_TOKEN + 1];


#endif //NATURAL_LANG_PROCESS_TOKEN_LIST_H
