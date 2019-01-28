#include "output_objectfile.h"

static char *file_name;
static FILE *fp;
static int is_initialized = 0, is_set_filename = 0;

/*
 * :param name: *.mpl
 */
void setOutputFileName(const char *name) {
    char *idx;
    file_name = (char *) malloc(sizeof(name) + 10);
    const char mpl[] = ".mpl";
    idx = strstr(name, mpl);
    int range = (int) (idx - name);
    strncpy(file_name, name + 0, range);
    file_name[range] = '\0'; // add null char to end
    is_set_filename = 1;
}

void initializeCompiler() {
    char out_file_name[MAX_WORD_LENGTH] = {'\0'};
    strcpy(out_file_name, file_name);
    strcat(out_file_name, ".csl");
    fp = fopen(out_file_name, "a");
    if (fp == NULL) {
        fprintf(stderr, "Can't open file\n");
        exit(1);
    }
    is_initialized = 1;
}

void writeObjectCode(const char *restrict format, ...) {
    if (!is_initialized || !is_set_filename) { fprintf(stderr, "Please initialize\n"); }
    fprintf(fp, "\t");
    va_list ap;
    va_start(ap, format);
    vfprintf(fp, format, ap);
    va_end(ap);
    fprintf(fp, "\n");
}

void writeVarLabel(const char *label) {
    if (!is_initialized || !is_set_filename) { fprintf(stderr, "Please initialize\n"); }
    fprintf(fp, "$%s\n", label);
}

void finalizeCompiler() {
    fclose(fp);
}