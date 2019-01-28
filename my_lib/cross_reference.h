#ifndef NATURAL_LANG_PROCESS_CROSS_REFERENCE_H
#define NATURAL_LANG_PROCESS_CROSS_REFERENCE_H

#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "common.h"
#include "lexical_analysis.h"

#define PROC_NAME_LENGTH 100
typedef enum e_type {
    TPINT = 50,
    TPCHAR,
    TPBOOL,
    TPARRAY,
    TPARRAYINT,
    TPARRAYCHAR,
    TPARRAYBOOL,
    TPPROC
} eType;

typedef enum e_scope {
    GLOBAL = 100,
    LOCAL
} eScope;

struct TYPE {
    eType var_type;
    int array_size;
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
    int has_set_type;
    int def_line;
    int is_formal_param;
    struct LINE *p_ref; //correspond to muitiple reference
    struct EXID *p_next;
} *global_id_root, *local_id_root;

extern void initGlobalID();
extern void initLocalID();
extern int registerExID(char *name, int def_line, int has_set_type);
extern int updateExIDType(eKeyword type, int is_array, int size);
extern int updateExIDTypeProcedure();
extern int updateExIDRefLine(char *name, int ref_line, int type);
extern void printCrossReference();
extern void setProcName(char *name);
const char *getProcName();
extern void setScope(eScope _scope);
extern int isPrevDefined(char *name);
extern int isStandardType(int type);
extern int getLocalVarType(char *name);
extern int getGlobalVarType(char *name);
int getArraySize(eScope _scope, char *name);
extern int keywordToType(int key, int is_array);
extern int arrayTypeToStandardType(int type);
extern int checkMatchDeclerVarAndCallExpression(char *name, int exp_num, int *types);

#endif //NATURAL_LANG_PROCESS_CROSS_REFERENCE_H
