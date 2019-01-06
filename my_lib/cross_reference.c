#include "cross_reference.h"

static char *proc_name;
static eScope scope;

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

void setScope(eScope _scope) {
    scope = _scope;
}

static struct EXID *existExIDinTable(eScope _scope, char *name) {
    struct EXID *p;

    if (_scope == GLOBAL) {
        for (p = global_id_root; p != NULL; p = p->p_next) {
            if (strcmp(name, p->name) == 0) { return p; }
        }
    } else if (_scope == LOCAL) {
        for (p = local_id_root; p != NULL; p = p->p_next) {
            if ((strcmp(name, p->name) == 0) && (strcmp(proc_name, p->proc_name) == 0)) { return p; }
        }
    }
    return NULL;
}

// @param _scope : must be declered place's scope
int isPrevDefined(char *name) {
    struct EXID *p;
    p = existExIDinTable(LOCAL, name);
    p = (p == NULL) ? existExIDinTable(GLOBAL, name) : p;
    return p != NULL;
}

int registerExID(char *name, int def_line, int has_set_type) {
    struct EXID *p, **q;
    char *np;

    if ((p = existExIDinTable(scope, name)) != NULL) {
        return errorWithReturn(getLineNum(), "twice define");
    }

    if ((p = (struct EXID *) malloc(sizeof(struct EXID))) == NULL) {
        fprintf(stderr, "[ERROR] can't malloc in 'registerExID'\n");
        return ERROR;
    }

    if ((np = (char *) malloc(strlen(name) + 1)) == NULL) {
        fprintf(stderr, "[ERROR] can't malloc in 'registerExID'\n");
        return ERROR;
    }

    if ((p->p_type = (struct TYPE *) malloc(sizeof(struct TYPE))) == NULL) {
        fprintf(stderr, "[ERROR] can't malloc in 'registerExID'\n");
        return ERROR;
    }

    if ((p->p_ref = (struct LINE *) malloc(sizeof(struct LINE))) == NULL) {
        fprintf(stderr, "[ERROR] can't malloc in 'registerExID'\n");
        return ERROR;
    }

    strcpy(np, name);
    p->name = np;
    p->def_line = def_line;
    p->has_set_type = has_set_type;
    p->proc_name = (char *) malloc(PROC_NAME_LENGTH);
    p->p_next = NULL;
    p->p_ref = NULL;
    p->p_type->array_size = 0;

    strcpy(p->proc_name, proc_name);
    if (scope == GLOBAL) {
        q = &global_id_root;
    } else if (scope == LOCAL) {
        q = &local_id_root;
    }

    for (; *q != NULL; q = &((*q)->p_next));
    *q = p;

    return OK;
}

int updateExIDType(eKeyword type, int is_array, int size) {
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
                if (size < 1 || size > MAX_WORD_LENGTH) {
                    return errorWithReturn(getLineNum(), "array size is wrong(1~1024)");
                }
                p->p_type->array_size = size;
                p->p_type->var_type = keywordToType(type, TRUE);
            } else {
                p->p_type->array_size = 0;
                p->p_type->var_type = keywordToType(type, FALSE);
            }
            p->has_set_type = 1;
        }
    }
    return 1;
}

int updateExIDTypeProcedure() {
    struct EXID *p, *q;
    struct TYPE *t;
    for (p = global_id_root; p != NULL; p = p->p_next) {
        if ((strcmp(proc_name, p->proc_name) == 0) && (strcmp(p->name, p->proc_name) == 0)) { //procedure
            p->p_type->var_type = TPPROC;
            p->has_set_type = TRUE;
            break;
        }
    }

    if (p == NULL) { return 0; }

    t = p->p_type;

    // make the list of formal params
    for (q = local_id_root; q != NULL; q = q->p_next) {
        if ((strcmp(proc_name, q->proc_name) == 0) && (strcmp(q->name, q->proc_name) > 0)) {
            if ((t->p_proc = (struct TYPE *) malloc(sizeof(struct TYPE))) == NULL) {
                fprintf(stderr, "[ERROR] can't malloc in 'updateExIDTypeProcedure'\n");
                return 0;
            }
            t->p_proc->var_type = q->p_type->var_type;
            t->p_proc->array_size = q->p_type->array_size;
            t->p_proc->p_proc = NULL;
            t = t->p_proc;
        }
    }

    return 1;
}

int updateExIDRefLine(char *name, int ref_line, int type) {
    struct EXID *p;
    struct LINE *node;
    struct LINE **q;

    for (p = local_id_root; p != NULL; p = p->p_next) {
        if ((strcmp(p->name, name) == 0) && (strcmp(p->proc_name, proc_name) == 0) &&
            (type == p->p_type->var_type)) { break; }
    }

    if (p == NULL) {
        for (p = global_id_root; p != NULL; p = p->p_next) {
            if ((strcmp(p->name, name) == 0) && (type == p->p_type->var_type)) { break; }
        }
    }

    if (p == NULL) { return 0; }

    if ((node = (struct LINE *) malloc(sizeof(struct LINE))) == NULL) {
        fprintf(stderr, "[ERROR] can't malloc in 'updateExIDRefLine'\n");
        return 0;
    }

    node->ref_line = ref_line;
    node->p_next = NULL;

    for (q = &(p->p_ref); *q != NULL; q = &((*q)->p_next));

    *q = node;

    return 1;
}

int getLocalVarType(char *name) {
    struct EXID *p;
    for (p = local_id_root; p != NULL; p = p->p_next) {
        if ((strcmp(p->name, name) == 0) && (strcmp(p->proc_name, proc_name) == 0)) { return p->p_type->var_type; }
    }
    return 0;
}

int getGlobalVarType(char *name) {
    struct EXID *p;
    for (p = global_id_root; p != NULL; p = p->p_next) {
        if ((strcmp(p->name, name) == 0)) { return p->p_type->var_type; }
    }
    return 0;
}

int getArraySize(eScope _scope, char *name) {
    struct EXID *p;
    if ((p = existExIDinTable(_scope, name)) == NULL) { return 0; }
    return p->p_type->array_size;
}

int checkMatchDeclerVarAndCallExpression(char *name, int exp_num, int *types) {
    struct EXID *p;
    struct TYPE *t;

    for (p = local_id_root; p != NULL; p = p->p_next) {
        if ((strcmp(p->name, name) == 0) && (strcmp(p->proc_name, proc_name) == 0) &&
            (p->p_type->var_type == TPPROC)) { break; }
    }

    if (p == NULL) {
        for (p = global_id_root; p != NULL; p = p->p_next) {
            if ((strcmp(p->name, name) == 0) && (p->p_type->var_type == TPPROC)) { break; }
        }
    }

    if (p == NULL) { return 0; }

    int param_num = 0, i = 0;
    for (t = p->p_type; t != NULL; t = t->p_proc) {
        if (t->var_type != TPPROC) { param_num++; }
    }
    if (exp_num != param_num) { return errorWithReturn(getLineNum(), "unmatch num of params"); }

    for (t = p->p_type; t != NULL; t = t->p_proc) {
        if (t->var_type != TPPROC) {
            if (t->var_type != types[i]) { return errorWithReturn(getLineNum(), "unmatch type"); }
            i++;
        }
    }
    return 1;
}
int isStandardType(int type) {
    switch (type) {
        case TINTEGER:
        case TCHAR:
        case TBOOLEAN:
        case TPINT:
        case TPCHAR:
        case TPBOOL:
            return TRUE;
        case TARRAY:
        case TPARRAYINT:
        case TPARRAYCHAR:
        case TPARRAYBOOL:
            return FALSE;
        default:
            return errorWithReturn(getLineNum(), "unknown type");
    }
}

int keywordToType(int key, int is_array) {
    if (is_array) {
        switch (key) {
            case TINTEGER:
                return TPARRAYINT;
            case TCHAR:
                return TPARRAYCHAR;
            case TBOOLEAN:
                return TPARRAYBOOL;
            default:
                return errorWithReturn(getLineNum(), "unknown type");
        }
    } else {
        switch (key) {
            case TINTEGER:
                return TPINT;
            case TCHAR:
                return TPCHAR;
            case TBOOLEAN:
                return TPBOOL;
            default:
                return errorWithReturn(getLineNum(), "unknown type");
        }
    }
}

int arrayTypeToStandardType(int type) {
    switch (type) {
        case TPARRAYINT:
            return TPINT;
        case TPARRAYCHAR:
            return TPCHAR;
        case TPARRAYBOOL:
            return TPBOOL;
        default:
            return errorWithReturn(getLineNum(), "unknown type");
    }
}

void debugExIDTable() {
    struct EXID *p;
    struct TYPE *t;
    struct LINE *l;
    printf("global : \n");
    for (p = global_id_root; p != NULL; p = p->p_next) {
        printf("%d\t%d\t%s\t%s\t", p->def_line, p->p_type->array_size, p->name, p->proc_name);
        if (p->p_type->var_type == TPPROC) {
            for (t = p->p_type; t != NULL; t = t->p_proc) {
                printf("%d\t", t->var_type);
            }
        } else {
            printf("%d\t", p->p_type->var_type);
        }
        printf("|\t");
        for (l = p->p_ref; l != NULL; l = l->p_next) {
            printf("%d\t", l->ref_line);
        }
        printf("\n");
    }
    printf("\nlocal : \n");
    for (p = local_id_root; p != NULL; p = p->p_next) {
        printf("%d\t%d\t%s\t%s\t", p->def_line, p->p_type->array_size, p->name, p->proc_name);
        if (p->p_type->var_type == TPPROC) {
            for (t = p->p_type; t != NULL; t = t->p_proc) {
                printf("%d\t", t->var_type);
            }
        } else {
            printf("%d\t", p->p_type->var_type);
        }
        printf("|\t");
        for (l = p->p_ref; l != NULL; l = l->p_next) {
            printf("%d\t", l->ref_line);
        }
        printf("\n");
    }
}