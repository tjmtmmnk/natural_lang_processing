#include "cross_reference.h"
#include <stdlib.h>
#include <stdio.h>
#include "string.h"

void initExIDTable() {
    global_id_root = NULL;
    local_id_root = NULL;
}

static struct EXID *existExIDinTable(eScope scope, char *name) {
    struct EXID *p;
    struct EXID *exid;
    int def_cnt = 0;
    if (scope == GLOBAL) {
        for (p = global_id_root; p != NULL; p = p->p_next) {
            if (strcmp(name, p->name) == 0) {
                def_cnt++;
                exid = p;
            }
        }
        if (def_cnt == 0) { return NULL; }
    } else if (scope == LOCAL) {
        for (p = local_id_root; p != NULL; p = p->p_next) {
            if (strcmp(name, p->name) == 0) {
                def_cnt++;
                exid = p;
            }
        }
        if (def_cnt == 0) { return NULL; }
    }

    if (def_cnt > 1) {
        fprintf(stderr, "You defined the same variable more than twice in the same scope.\n");
        return NULL;
    }

    return exid;
}

void registerExID(eScope scope, char *name, int def_line, int is_formal_param, char *proc_name) {
    struct EXID *p;
    char *np;

    if (scope == SCOPE_NONE) { return; }

    if ((p = existExIDinTable(scope, name)) != NULL) { return; }

    if ((p = (struct EXID *) malloc(sizeof(struct EXID))) == NULL) {
        fprintf(stderr, "can't malloc in 'registerExID'\n");
        return;
    }

    if ((np = (char *) malloc(strlen(name) + 1)) == NULL) {
        fprintf(stderr, "can't malloc in 'registerExID'\n");
        return;
    }
    strcpy(np, name);
    p->name = np;
    p->def_line = def_line;
    p->is_formal_param = is_formal_param;
    if (scope == GLOBAL) {
        p->p_next = global_id_root;
        global_id_root = p;
    } else if (scope == LOCAL) {
        p->p_next = local_id_root;
        p->proc_name = proc_name;
        local_id_root = p;
    }
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
        printf("%s%d\t", p->name, p->def_line);
    }
    printf("\nlocal : \n");
    for (p = local_id_root; p != NULL; p = p->p_next) {
        printf("%s%d\t", p->name, p->def_line);
    }
}