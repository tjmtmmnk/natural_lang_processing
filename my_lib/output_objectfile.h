#ifndef NATURAL_LANG_PROCESS_OUTPUT_OBJECTFILE_H
#define NATURAL_LANG_PROCESS_OUTPUT_OBJECTFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "string.h"
#include "lexical_analysis.h"
#include "syntatic_analysis.h"
#include "cross_reference.h"

//:param label: Lxxxx DC [content]
struct DCLabel {
    char *label;
    struct DCLabel *next;
} *label_root;

extern void setOutputFileName(const char *name);
extern void writeObjectCode(const char *format, ...);
extern void writeObjectCodeWithoutTab(const char *format, ...);
extern void writeObjectCodeRaw(const char *format, ...);
extern void writeVarLabel(const char *label, int is_newline);
extern void initializeCompiler();
extern void finalizeCompiler();
extern void writeMalloc();
extern void writeLibrary();
extern void writeSimpleExpObjectCode(int ope);
extern void writeExpObjectCode(int ope);
extern void writeJumpLabel(int num);
extern void writeOutputObjectCode(int type);
extern void writeTermObjectCode(int ope);
extern void writeArrayVarObjectCode(eScope scope, int is_address_hand, char *name, int size);
extern void writeStandardVarObjectCode(eScope scope, int is_address_hand, char *name);
extern void writeFactorObjectCode(int token, int number, int exp_type, char *str);
extern void writeInputObjectCode(int type);
extern void registerDCLabel(int label, char *str);
extern void writeDCLabel();
extern int getIncLabel();
extern int getLabel();

#endif //NATURAL_LANG_PROCESS_OUTPUT_OBJECTFILE_H
