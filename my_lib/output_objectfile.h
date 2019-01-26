#ifndef NATURAL_LANG_PROCESS_OUTPUT_OBJECTFILE_H
#define NATURAL_LANG_PROCESS_OUTPUT_OBJECTFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "string.h"

extern void setOutputFileName(const char *name);
extern void write(const char *format,...);
extern void initializeCompiler();
extern void finalizeCompiler();

#endif //NATURAL_LANG_PROCESS_OUTPUT_OBJECTFILE_H
