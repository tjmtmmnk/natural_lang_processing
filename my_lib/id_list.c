#include "token_list.h"

static void initIDTable() {        /* Initialise the table */
    idroot = NULL;
}

static struct ID *existIDinTable(char *np) {    /* search the name pointed by np */
    struct ID *p;

    for (p = idroot; p != NULL; p = p->nextp) {
        if (strcmp(np, p->name) == 0) return (p);
    }
    return (NULL);
}

void countUpID(char *np) {    /* Register and count up the name pointed by np */
    struct ID *p;
    char *cp;

    if ((p = existIDinTable(np)) != NULL) {
        p->count++;
    } else {
        if ((p = (struct ID *) malloc(sizeof(struct ID))) == NULL) {
            printf("can not malloc in id_countup\n");
            return;
        }
        if ((cp = (char *) malloc(strlen(np) + 1)) == NULL) {
            printf("can not malloc-2 in id_countup\n");
            return;
        }
        strcpy(cp, np);
        p->name = cp;
        p->count = 1;
        p->nextp = idroot;
        idroot = p;
    }
}

void debugIDTable() {    /* Output the registered data */
    struct ID *p;
    for (p = idroot; p != NULL; p = p->nextp) {
        if (p->count != 0)
            printf("[Id]\t%-10s\t%20d\n", p->name, p->count);
    }
}

void deleteIDTable() {    /* Release tha data structure */
    struct ID *p, *q;

    for (p = idroot; p != NULL; p = q) {
        free(p->name);
        q = p->nextp;
        free(p);
    }
    initIDTable();
}