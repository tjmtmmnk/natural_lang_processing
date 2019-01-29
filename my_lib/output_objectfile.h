#ifndef NATURAL_LANG_PROCESS_OUTPUT_OBJECTFILE_H
#define NATURAL_LANG_PROCESS_OUTPUT_OBJECTFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "string.h"
#include "lexical_analysis.h"
#include "syntatic_analysis.h"

extern void setOutputFileName(const char *name);
extern void writeObjectCode(const char *format, ...);
extern void writeObjectCodeWithoutTab(const char *restrict format, ...);
extern void writeObjectCodeRaw(const char *restrict format, ...);
extern void writeVarLabel(const char *label, int is_newline);
extern void initializeCompiler();
extern void finalizeCompiler();
extern void writeMalloc();
extern void writeLibrary();
extern void writeSimpleExpObjectCode(int ope);
extern void writeExpObjectCode(int ope);
extern void writeJumpLabel(int num);
extern void writeOutputObjectCode(int type);
extern int getIncLabel();
extern int getDecLabel();
extern int getLabel();

#endif //NATURAL_LANG_PROCESS_OUTPUT_OBJECTFILE_H
