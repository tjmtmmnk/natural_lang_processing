#ifndef NATURAL_LANG_PROCESS_LEXICAL_ANALYSIS_H
#define NATURAL_LANG_PROCESS_LEXICAL_ANALYSIS_H

#include "token_list.h"
#include "common.h"

#define MAX_WORD_LENGTH 1000
#define UNSIGNED_INT_MAX 32767
#define SCAN_END -1

void initScan();

void setFileName(char *_file_name);

void closeFile();

extern void countUpID(char *np);

extern void debugIDTable();

// @return : token_code
extern int scanTokenOneEach();

extern int num_attr;
extern char str_attr[MAX_WORD_LENGTH];


#endif //NATURAL_LANG_PROCESS_LEXICAL_ANALYSIS_H
