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

int registerExID(eScope scope, char *name, int def_line, int is_formal_param) {
    struct EXID *p;
    char *np;

    if (scope == SCOPE_NONE) { return 0; } //do nothing

    if ((p = existExIDinTable(scope, name)) != NULL) {
        return errorWithReturn(getLineNum(), "twice define");
    }

    if ((p = (struct EXID *) malloc(sizeof(struct EXID))) == NULL) {
        return errorWithReturn(getLineNum(), "can't malloc in 'registerExID'");
    }

    if ((np = (char *) malloc(strlen(name) + 1)) == NULL) {
        return errorWithReturn(getLineNum(), "can't malloc in 'registerExID'");
    }
    strcpy(np, name);
    p->name = np;
    p->def_line = def_line;
    p->is_formal_param = is_formal_param;
    p->proc_name = (char *) malloc(PROC_NAME_LENGTH);

    if (scope == GLOBAL) {
        p->p_next = global_id_root;
        strcpy(p->proc_name, "global");
        global_id_root = p;
    } else if (scope == LOCAL) {
        strcpy(p->proc_name, proc_name);
        p->p_next = local_id_root;
        local_id_root = p;
    }

    return 1;
}

void updateExIDType(eScope scope, char *name, struct TYPE *type) {
    struct EXID *p;
    if ((p = existExIDinTable(scope, name)) == NULL) { return; }
    p->p_type = type;
}

void debugExIDTable() {
    struct EXID *p;
    printf("global : \n");
    for (p = global_id_root; p != NULL; p = p->p_next) {
        printf("%s%d\t%s\n", p->name, p->def_line, p->proc_name);
    }
    printf("\nlocal : \n");
    for (p = local_id_root; p != NULL; p = p->p_next) {
        printf("%s%d\t%s\n", p->name, p->def_line, p->proc_name);
    }
}