#ifndef NATURAL_LANG_PROCESS_OUTPUT_OBJECTFILE_H
#define NATURAL_LANG_PROCESS_OUTPUT_OBJECTFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "string.h"
#include "lexical_analysis.h"

extern void setOutputFileName(const char *name);
extern void writeObjectCode(const char *format, ...);
extern void writeVarLabel(const char *label);
extern void initializeCompiler();
extern void finalizeCompiler();

#endif //NATURAL_LANG_PROCESS_OUTPUT_OBJECTFILE_H
