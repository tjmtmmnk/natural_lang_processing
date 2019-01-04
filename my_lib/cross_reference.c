#include "cross_reference.h"

char *proc_name;

void initGlobalID() {
    global_id_root = NULL;
}

void initLocalID() {
    proc_name = (char *) malloc(PROC_NAME_LENGTH);
    local_id_root = NULL;
}

void setProcName(char *name) {
    strcpy(proc_name, name);
}

static struct EXID *existExIDinTable(eScope scope, char *name) {
    struct EXID *p;

    if (scope == GLOBAL) {
        for (p = global_id_root; p != NULL; p = p->p_next) {
            if (strcmp(name, p->name) == 0) { return p; }
        }
    } else if (scope == LOCAL) {
        for (p = local_id_root; p != NULL; p = p->p_next) {
            if ((strcmp(name, p->name) == 0) && (strcmp(proc_name, p->proc_name) == 0)) { return p; }
        }
    }
    return NULL;
}

int registerExID(eScope scope, char *name, int def_line, int has_set_type) {
    struct EXID *p, **q;
    char *np;

    if (scope == SCOPE_NONE) { return 0; } //do nothing

    if (strcmp(proc_name, "call") == 0) { return 0; } //do nothing

    if ((p = existExIDinTable(scope, name)) != NULL) {
        return errorWithReturn(getLineNum(), "twice define");
    }

    if ((p = (struct EXID *) malloc(sizeof(struct EXID))) == NULL) {
        fprintf(stderr, "[ERROR] can't malloc in 'registerExID'\n");
        return 0;
    }

    if ((np = (char *) malloc(strlen(name) + 1)) == NULL) {
        fprintf(stderr, "[ERROR] can't malloc in 'registerExID'\n");
        return 0;
    }

    if ((p->p_type = (struct TYPE *) malloc(sizeof(struct TYPE))) == NULL) {
        fprintf(stderr, "[ERROR] can't malloc in 'registerExID'\n");
        return 0;
    }

    strcpy(np, name);
    p->name = np;
    p->def_line = def_line;
    p->has_set_type = has_set_type;
    p->proc_name = (char *) malloc(PROC_NAME_LENGTH);

    if (scope == GLOBAL) {
        strcpy(p->proc_name, "global");
        q = &global_id_root;
    } else if (scope == LOCAL) {
        strcpy(p->proc_name, proc_name);
        q = &local_id_root;
    }

    for (; *q != NULL; q = &((*q)->p_next));
    *q = p;

    return 1;
}

int updateExIDType(eScope scope, eKeyword type, int is_array, int size) {
    struct EXID *p;
    if (scope == LOCAL) {
        p = local_id_root;
    } else if (scope == GLOBAL) {
        p = global_id_root;
    } else {
        return 0;
    }

    for (; p != NULL; p = p->p_next) {
        if (!p->has_set_type) {
            if (is_array) {
                p->p_type->array_size = size;
                switch (type) {
                    case TINTEGER:
                        p->p_type->var_type = TPARRAYINT;
                        break;
                    case TCHAR:
                        p->p_type->var_type = TPARRAYCHAR;
                        break;
                    case TBOOLEAN:
                        p->p_type->var_type = TPARRAYBOOL;
                        break;
                    default:
                        return 0;
                }
            } else {
                p->p_type->array_size = 0;
                switch (type) {
                    case TINTEGER:
                        p->p_type->var_type = TPINT;
                        break;
                    case TCHAR:
                        p->p_type->var_type = TPCHAR;
                        break;
                    case TBOOLEAN:
                        p->p_type->var_type = TPBOOL;
                        break;
                    default:
                        return 0;
                }
            }
        }
        p->has_set_type = 1; //use for flag to indicate whether or not it has been updated
    }

    return 1;
}

int updateExIDTypeProcedure() {
    struct EXID *p, *q;
    for (p = local_id_root; p != NULL; p = p->p_next) {
        if ((strcmp(proc_name, p->proc_name) == 0) && (strcmp(p->name, p->proc_name) == 0)) { //procedure
            p->p_type->var_type = TPPROC;
            break;
        }
    }

    for (q = local_id_root; q != NULL; q = q->p_next) {
        if ((strcmp(proc_name, q->proc_name) == 0) && (strcmp(q->name, q->proc_name) > 0)) {
            if ((p->p_type->p_proc = (struct TYPE *) malloc(sizeof(struct TYPE))) == NULL) {
                fprintf(stderr, "[ERROR] can't malloc in 'updateExIDTypeProcedure'\n");
                return 0;
            }
            p->p_type->p_proc->var_type = q->p_type->var_type;
            p->p_type->p_proc->array_size = q->p_type->array_size;
            p->p_type->p_proc = q->p_type;
            p = q;
        }
    }

    return 1;
}

void debugExIDTable() {
    struct EXID *p;
    printf("global : \n");
    for (p = global_id_root; p != NULL; p = p->p_next) {
        printf("%d\t%s\t%d\n", p->def_line, p->name, p->p_type->var_type);
    }
    printf("\nlocal : \n");
    for (p = local_id_root; p != NULL; p = p->p_next) {
        printf("%d\t%s\t%s\t%d\n", p->def_line, p->name, p->proc_name, p->p_type->var_type);
    }
}

void debug() {
    struct EXID *p;
    struct TYPE *t;
    for (p = local_id_root; p != NULL; p = p->p_next) {
        if (p->p_type->var_type == TPPROC) {
            printf("%s\t%d\t", p->name, p->p_type->var_type);
            for (t = p->p_type->p_proc; t != NULL; t = t->p_proc) {
                printf("%d\t", t->var_type);
            }
            printf("\n");
        }
    }
}