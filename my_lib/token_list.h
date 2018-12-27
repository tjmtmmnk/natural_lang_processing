#ifndef NATURAL_LANG_PROCESS_TOKEN_LIST_H
#define NATURAL_LANG_PROCESS_TOKEN_LIST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM_OF_TOKEN 49
#define NUM_OF_SYMBOL 18
#define NUM_OF_KEY 28
#define NUM_OF_CONT_SYMBOL 4
#define NUM_OF_SPLIT 4

/* Token */
typedef enum e_name {
    NONE = -1,
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
    MODE_COMMENT_SLASH, // /**/
    MODE_COMMENT_BRACE  // {}
} Mode;

typedef enum TTYPE {
    TPINT,
    TPCHAR,
    TOBOOL,
    TPARRAY,
    TPARRAYINT,
    TPARRAYCHAR,
    TPARRAYBOOL,
    TPPROC
} eType;

extern const struct KEY {
    char *keyword;
    int token_number;
} key[NUM_OF_KEY];

extern const struct SYMBOL {
    char *symbol;
    int token_number;
} symbol[NUM_OF_SYMBOL];

struct ID {
    char *name;
    int count;
    struct ID *nextp;
} *idroot;

struct TYPE {
    eType token_type;
    int array_size;
    struct TYPE *p_array; /* pointer to element type if TPARRAY */
    struct TYPE *p_proc; /* pointer to parameter's type list if token_type is TPPROC */
};

struct LINE {
    int ref_line;
    struct LINE *p_next;
};

struct EXID {
    char *name;
    char *proc_name;
    struct TYPE *p_type;
    int is_formal_param; /* 1:formal parameter, 0:else(variable) */
    int def_line;
    struct LINE *p_ref;
    struct ID *p_next;
} *grobal_id, *local_id;

extern const char cont_sym[NUM_OF_CONT_SYMBOL][3]; //continuous symbol
extern const char split[NUM_OF_SPLIT];
extern int token_counter[NUM_OF_TOKEN + 1];

extern char *token_str[NUM_OF_TOKEN + 1]; /* string of each token */


#endif //NATURAL_LANG_PROCESS_TOKEN_LIST_H
