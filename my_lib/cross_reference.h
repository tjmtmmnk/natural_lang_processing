#ifndef NATURAL_LANG_PROCESS_CROSS_REFERENCE_H
#define NATURAL_LANG_PROCESS_CROSS_REFERENCE_H

typedef enum TTYPE {
    TPINT = 50,
    TPCHAR,
    TPBOOL,
    TPARRAY,
    TPARRAYINT,
    TPARRAYCHAR,
    TPARRAYBOOL,
    TPPROC
} eType;

typedef enum ESCOPE{
    GLOBAL=100,
    LOCAL
} eScope;

struct TYPE {
    eType var_type; /*the type of variable*/
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
    struct EXID *p_next;
} *global_id_root, *local_id_root;


#endif //NATURAL_LANG_PROCESS_CROSS_REFERENCE_H
