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

    switch (scope) {
        case GLOBAL: {
            for (p = global_id_root; p != NULL; p = p->p_next) {
                if (strcmp(name, p->name) == 0) { return p; }
            }
            return NULL;
        }
        case LOCAL: {
            for (p = local_id_root; p != NULL; p = p->p_next) {
                if (strcmp(name, p->name) == 0) { return p; }
            }
            return NULL;
        }
    }
    return NULL;
}

void registerExID(eScope scope, char *name, int def_line, int is_formal_param) {
    struct EXID *p;
    char *np;

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
    } else if (scope == LOCAL) {
        p->p_next = local_id_root;
    }
}
