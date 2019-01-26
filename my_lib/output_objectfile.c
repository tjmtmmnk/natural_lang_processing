#include "output_objectfile.h"

static char *file_name;
static FILE *fp;
static int is_initialized = 0, is_set_filename = 0;

/*
 * :param name: *.mpl
 */
void setOutputFileName(const char *name) {
    char *idx;
    char *out_filename = (char *) malloc(sizeof(name) + 10);
    const char mpl[] = ".mpl";
    idx = strstr(name, mpl);
    int range = (int) (idx - name);
    strncpy(out_filename, name + 0, range);
    out_filename[range] = '\0'; // add null char to end
    strcat(out_filename, ".csl");
    printf("aaa %s\n", out_filename);
    is_set_filename = 1;
}

void initializeCompiler() {
    fp = fopen(file_name, "a");
    if (fp == NULL) {
        fprintf(stderr, "Can't open file\n");
        exit(1);
    }
    is_initialized = 1;
}

void write(const char *restrict format, ...) {
    if (!is_initialized || !is_set_filename) { fprintf(stderr, "Please initialize\n"); }
    va_list ap;
    va_start(ap, format);
    vfprintf(fp, format, ap);
    va_end(ap);
}

void finalizeCompiler() {
    fclose(fp);
}