#include "output_objectfile.h"

static char *file_name;
static FILE *fp;
static int is_initialized = 0, is_set_filename = 0;
static int label = 0;

/*
 * :param name: *.mpl
 */
void setOutputFileName(const char *name) {
    file_name = (char *) malloc(strlen(name) + 10);
    char *idx;
    const char mpl[] = ".mpl";
    idx = strstr(name, mpl);
    int range = (int) (idx - name);
    strncpy(file_name, name + 0, range);
    file_name[range] = '\0'; // add null char to end
    is_set_filename = 1;
}

void initializeCompiler() {
    label_root = NULL;
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

void writeObjectCodeWithoutTab(const char *restrict format, ...) {
    if (!is_initialized || !is_set_filename) { fprintf(stderr, "Please initialize\n"); }
    va_list ap;
    va_start(ap, format);
    vfprintf(fp, format, ap);
    va_end(ap);
    fprintf(fp, "\n");
}

void writeObjectCodeRaw(const char *restrict format, ...) {
    if (!is_initialized || !is_set_filename) { fprintf(stderr, "Please initialize\n"); }
    va_list ap;
    va_start(ap, format);
    vfprintf(fp, format, ap);
    va_end(ap);
}

void writeVarLabel(const char *label, int is_newline) {
    if (!is_initialized || !is_set_filename) { fprintf(stderr, "Please initialize\n"); }
    fprintf(fp, "$%s", label);
    if (is_newline) {
        fprintf(fp, "\n");
    }
}

void writeJumpLabel(int num) {
    writeObjectCodeWithoutTab("L%04d", num);
}

void writeMalloc() {
    struct EXID *p = getMergedList();
    for (; p != NULL; p = p->p_next) {
        if (p->p_type->var_type != TPPROC) {
            writeVarLabel(p->name, FALSE);
            if (p->p_type->array_size > 0) {
                writeObjectCode("\tDS\t%d", p->p_type->array_size);
            } else {
                writeObjectCode("\tDC\t0");
            }
        }
    }
}

void writeSimpleExpObjectCode(int ope) {
    writeObjectCode("POP\tgr2");
    switch (ope) {
        case TPLUS:
            writeObjectCode("ADDA\tgr1,gr2");
            writeObjectCode("JOV\tEOVF");
            break;
        case TMINUS:
            writeObjectCode("SUBA\tgr2,gr1");
            writeObjectCode("JOV\tEOVF");
            writeObjectCode("LD\tgr1,gr2");
            break;
        case TOR:
            writeObjectCode("OR\tgr1,gr2");
            break;
    }
}

int writeExpObjectCode(int ope) {
    writeObjectCode("POP\tgr2");
    writeObjectCode("CPA\tgr2,gr1");

    int break_label = getIncLabel();
    int true_label = getIncLabel();
    int false_label = getIncLabel();

    switch (ope) {
        case TEQUAL: // =
            writeObjectCodeRaw("\tJZE\t");
            writeJumpLabel(true_label);
            break;
        case TNOTEQ: // <>
            writeObjectCodeRaw("\tJNZ\t");
            writeJumpLabel(true_label);
            break;
        case TLE: // <
            writeObjectCodeRaw("\tJMI\t");
            writeJumpLabel(true_label);
            break;
        case TLEEQ: // <=
            writeObjectCodeRaw("\tJMI\t");
            writeJumpLabel(true_label);
            writeObjectCodeRaw("\tJZE\t");
            writeJumpLabel(true_label);
            break;
        case TGR: // >
            writeObjectCodeRaw("\tJPL\t");
            writeJumpLabel(true_label);
            break;
        case TGREQ: // >=
            writeObjectCodeRaw("\tJPL\t");
            writeJumpLabel(true_label);
            writeObjectCodeRaw("\tJZE\t");
            writeJumpLabel(true_label);
            break;
    }

    writeObjectCode("LD\tgr1,gr0");
    writeObjectCodeRaw("\tJUMP\t");
    writeJumpLabel(false_label);

    writeJumpLabel(true_label);
    writeObjectCode("LAD\tgr1,1");
    writeJumpLabel(false_label);

    return break_label;
}

void writeTermObjectCode(int ope) {
    writeObjectCode("POP\tgr2");

    switch (ope) {
        case TAND:
            writeObjectCode("AND\tgr1,gr2");
            break;
        case TSTAR:
            writeObjectCode("MULA\tgr1,gr2");
            writeObjectCode("JOV\tEOVF");
            break;
        case TDIV:
            writeObjectCode("DIVA\tgr2,gr1");
            writeObjectCode("JOV\tE0DIV");
            writeObjectCode("LD\tgr1,gr2");
            break;
    }
}

// WRITELINE is run in parseOutputState() if need
void writeOutputObjectCode(int type) {
    switch (type) {
        case TPINT:
            writeObjectCode("CALL\tWRITEINT");
            break;
        case TPCHAR:
            writeObjectCode("CALL\tWRITECHAR");
            break;
        case TPBOOL:
            writeObjectCode("CALL\tWRITEBOOL");
            break;
    }
}

void writeInputObjectCode(int type) {
    switch (type) {
        case TPINT:
            writeObjectCode("CALL\tREADINT");
            break;
        case TPCHAR:
            writeObjectCode("CALL\tREADCHAR");
            break;
    }
}

void registerDCLabel(int label, char *str) {
    struct DCLabel *p, **q;
    char *np;
    char content[MAX_WORD_LENGTH];

    if ((p = (struct DCLabel *) malloc(sizeof(struct DCLabel))) == NULL) {
        fprintf(stderr, "[ERROR] can't malloc in 'registerDCLabel'\n");
        return;
    }

    if (str == NULL) {
        snprintf(content, MAX_WORD_LENGTH, "L%04d\tDC\t0", label);
    } else {
        snprintf(content, MAX_WORD_LENGTH, "L%04d\tDC\t\'%s\'", label, str);
    }

    if ((np = (char *) malloc(strlen(content) + 1)) == NULL) {
        fprintf(stderr, "[ERROR] can't malloc in 'registerDCLabel'\n");
        return;
    }

    strcpy(np, content);

    p->label = np;
    p->next = NULL;

    for (q = &label_root; *q != NULL; q = &((*q)->next));
    *q = p;
}

void writeDCLabel() {
    struct DCLabel *p;
    for (p = label_root; p != NULL; p = p->next) {
        writeObjectCodeRaw("%s\n", p->label);
    }
}

static void _writeVarLabel(char *var_name, char *proc_name) {
    if (strcmp(proc_name, "global") == 0) {
        writeObjectCodeRaw("$%s\n", var_name);
    } else {
        writeObjectCodeRaw("$%s%%%s\n", var_name, proc_name);
    }
}

void writeArrayVarObjectCode(eScope scope, int is_address_hand, char *name, int size) {
    struct EXID *p = existExIDinTable(scope, name);
    if (!p->is_formal_param) {
        writeObjectCode("POP\tgr2"); //idx
        writeObjectCode("LAD\tgr1,%d", size);
        writeObjectCode("CPA\tgr2,gr1");
        writeObjectCode("JPL\tEROV");
        writeObjectCodeRaw("\tLAD\tgr1,\t");
        _writeVarLabel(p->name, p->proc_name);
        if (!is_address_hand) {
            writeObjectCode("LD\tgr1,0,gr1");
        }
    }
}

void writeStandardVarObjectCode(eScope scope, int is_address_hand, char *name) {
    struct EXID *p = existExIDinTable(scope, name);
    if (p->is_formal_param) {
        writeObjectCodeRaw("\tLD\tgr1,\t");
        _writeVarLabel(p->name, p->proc_name);
        if (!is_address_hand) {
            writeObjectCode("LD\tgr1,0,gr1");
        }
    } else {
        if (is_address_hand) {
            writeObjectCodeRaw("\tLAD\tgr1,\t");
        } else {
            writeObjectCodeRaw("\tLD\tgr1,\t");
        }
        _writeVarLabel(p->name, p->proc_name);
    }
}

void writeFactorObjectCode(int token, int number, int exp_type, char *str) {
    switch (token) {
        case TFALSE:
            writeObjectCode("LAD\tgr1,0");
            break;
        case TTRUE:
            writeObjectCode("LAD\tgr1,1");
            break;
        case TNUMBER:
            writeObjectCode("LAD\tgr1,%d", number);
            break;
        case TSTRING:
            writeObjectCodeRaw("\tLD\tgr1,\t");
            writeJumpLabel(getIncLabel());
            registerDCLabel(getLabel(), str);
            break;
        case TNOT:
            writeObjectCode("POP\tgr2");
            writeObjectCode("LAD\tgr1,1");
            writeObjectCode("XOR\tgr1,gr2");
            break;
        case TINTEGER:
            break;
        case TBOOLEAN: // need below process because the exp value isn't determined 0 or 1
            if ((exp_type == TPINT) || (exp_type == TPCHAR)) {
                writeObjectCode("POP\tgr1");
                writeObjectCode("CPA\tgr1,gr0");
                writeObjectCodeRaw("\tJZE\t");
                writeJumpLabel(getIncLabel());
                writeObjectCode("LAD\tgr1,1");
                writeJumpLabel(getLabel());
            }
            break;
        case TCHAR:
            if (exp_type == TPINT) {
                writeObjectCode("POP\tgr2");
                writeObjectCode("LAD\tgr1,127"); // 0b01111111
                writeObjectCode("AND\tgr1,gr2");
            }
            break;
    }
}

int getIncLabel() {
    return ++label;
}

int getDecLabel() {
    if (label > 0) {
        return --label;
    } else {
        fprintf(stderr, "label is minus");
        return ERROR;
    }
}

int getLabel() {
    return label;
}

void writeLibrary() {
    writeVarLabel("EOVF", TRUE);
    writeObjectCode("CALL  WRITELINE");
    writeObjectCode("LAD  gr1, EOVF1");
    writeObjectCode("LD  gr2, gr0");
    writeObjectCode("CALL  WRITESTR");
    writeObjectCode("CALL  WRITELINE");
    writeObjectCode("SVC  1");
    writeVarLabel("EOVF1    DC  '***** Run-Time Error : Overflow *****'", TRUE);
    writeVarLabel("E0DIV", TRUE);
    writeObjectCode("JNZ  EOVF");
    writeObjectCode("CALL  WRITELINE");
    writeObjectCode("LAD  gr1, E0DIV1");
    writeObjectCode("LD  gr2, gr0");
    writeObjectCode("CALL  WRITESTR");
    writeObjectCode("CALL  WRITELINE");
    writeObjectCode("SVC  2");
    writeVarLabel("E0DIV1    DC  '***** Run-Time Error : Zero-Divide *****'", TRUE);
    writeVarLabel("EROV", TRUE);
    writeObjectCode("CALL  WRITELINE");
    writeObjectCode("LAD  gr1, EROV1");
    writeObjectCode("LD  gr2, gr0");
    writeObjectCode("CALL  WRITESTR");
    writeObjectCode("CALL  WRITELINE");
    writeObjectCode("SVC  3");
    writeVarLabel("EROV1    DC  '***** Run-Time Error : Range-Over in Array Index *****'", TRUE);
    writeVarLabel("WRITECHAR", TRUE);
    writeObjectCode("RPUSH");
    writeObjectCode("LD  gr6, SPACE");
    writeObjectCode("LD  gr7, OBUFSIZE");
    writeVarLabel("WC1", TRUE);
    writeObjectCode("SUBA  gr2, ONE");
    writeObjectCode("JZE  WC2");
    writeObjectCode("JMI  WC2");
    writeObjectCode("ST  gr6, OBUF,gr7");
    writeObjectCode("CALL  BOVFCHECK");
    writeObjectCode("JUMP  WC1");
    writeVarLabel("WC2", TRUE);
    writeObjectCode("ST  gr1, OBUF,gr7");
    writeObjectCode("CALL  BOVFCHECK");
    writeObjectCode("ST  gr7, OBUFSIZE");
    writeObjectCode("RPOP");
    writeObjectCode("RET");
    writeVarLabel("WRITESTR", TRUE);
    writeObjectCode("RPUSH");
    writeObjectCode("LD  gr6, gr1");
    writeVarLabel("WS1", TRUE);
    writeObjectCode("LD  gr4, 0,gr6");
    writeObjectCode("JZE  WS2");
    writeObjectCode("ADDA  gr6, ONE");
    writeObjectCode("SUBA  gr2, ONE");
    writeObjectCode("JUMP  WS1");
    writeVarLabel("WS2", TRUE);
    writeObjectCode("LD  gr7, OBUFSIZE");
    writeObjectCode("LD  gr5, SPACE");
    writeVarLabel("WS3", TRUE);
    writeObjectCode("SUBA  gr2, ONE");
    writeObjectCode("JMI  WS4");
    writeObjectCode("ST  gr5, OBUF,gr7");
    writeObjectCode("CALL  BOVFCHECK");
    writeObjectCode("JUMP  WS3");
    writeVarLabel("WS4", TRUE);
    writeObjectCode("LD  gr4, 0,gr1");
    writeObjectCode("JZE  WS5");
    writeObjectCode("ST  gr4, OBUF,gr7");
    writeObjectCode("ADDA  gr1, ONE");
    writeObjectCode("CALL  BOVFCHECK");
    writeObjectCode("JUMP  WS4");
    writeVarLabel("WS5", TRUE);
    writeObjectCode("ST  gr7, OBUFSIZE");
    writeObjectCode("RPOP");
    writeObjectCode("RET");
    writeVarLabel("BOVFCHECK", TRUE);
    writeObjectCode("  ADDA  gr7, ONE");
    writeObjectCode("  CPA   gr7, BOVFLEVEL");
    writeObjectCode("  JMI  BOVF1");
    writeObjectCode("  CALL  WRITELINE");
    writeObjectCode("  LD gr7, OBUFSIZE");
    writeVarLabel("BOVF1", TRUE);
    writeObjectCode("  RET");
    writeVarLabel("BOVFLEVEL  DC 256", TRUE);
    writeVarLabel("WRITEINT", TRUE);
    writeObjectCode("RPUSH");
    writeObjectCode("LD  gr7, gr0");
    writeObjectCode("CPA  gr1, gr0");
    writeObjectCode("JPL  WI1");
    writeObjectCode("JZE  WI1");
    writeObjectCode("LD  gr4, gr0");
    writeObjectCode("SUBA  gr4, gr1");
    writeObjectCode("CPA  gr4, gr1");
    writeObjectCode("JZE  WI6");
    writeObjectCode("LD  gr1, gr4");
    writeObjectCode("LD  gr7, ONE");
    writeVarLabel("WI1", TRUE);
    writeObjectCode("LD  gr6, SIX");
    writeObjectCode("ST  gr0, INTBUF,gr6");
    writeObjectCode("SUBA  gr6, ONE");
    writeObjectCode("CPA  gr1, gr0");
    writeObjectCode("JNZ  WI2");
    writeObjectCode("LD  gr4, ZERO");
    writeObjectCode("ST  gr4, INTBUF,gr6");
    writeObjectCode("JUMP  WI5");
    writeVarLabel("WI2", TRUE);
    writeObjectCode("CPA  gr1, gr0");
    writeObjectCode("JZE  WI3");
    writeObjectCode("LD  gr5, gr1");
    writeObjectCode("DIVA  gr1, TEN");
    writeObjectCode("LD  gr4, gr1");
    writeObjectCode("MULA  gr4, TEN");
    writeObjectCode("SUBA  gr5, gr4");
    writeObjectCode("ADDA  gr5, ZERO");
    writeObjectCode("ST  gr5, INTBUF,gr6");
    writeObjectCode("SUBA  gr6, ONE");
    writeObjectCode("JUMP  WI2");
    writeVarLabel("WI3", TRUE);
    writeObjectCode("CPA  gr7, gr0");
    writeObjectCode("JZE  WI4");
    writeObjectCode("LD  gr4, MINUS");
    writeObjectCode("ST  gr4, INTBUF,gr6");
    writeObjectCode("JUMP  WI5");
    writeVarLabel("WI4", TRUE);
    writeObjectCode("ADDA  gr6, ONE");
    writeVarLabel("WI5", TRUE);
    writeObjectCode("LAD  gr1, INTBUF,gr6");
    writeObjectCode("CALL  WRITESTR");
    writeObjectCode("RPOP");
    writeObjectCode("RET");
    writeVarLabel("WI6", TRUE);
    writeObjectCode("LAD  gr1, MMINT");
    writeObjectCode("CALL  WRITESTR");
    writeObjectCode("RPOP");
    writeObjectCode("RET");
    writeVarLabel("MMINT    DC  '-32768'", TRUE);
    writeVarLabel("WRITEBOOL", TRUE);
    writeObjectCode("RPUSH");
    writeObjectCode("CPA  gr1, gr0");
    writeObjectCode("JZE  WB1");
    writeObjectCode("LAD  gr1, WBTRUE");
    writeObjectCode("JUMP  WB2");
    writeVarLabel("WB1", TRUE);
    writeObjectCode("LAD  gr1, WBFALSE");
    writeVarLabel("WB2", TRUE);
    writeObjectCode("CALL  WRITESTR");
    writeObjectCode("RPOP");
    writeObjectCode("RET");
    writeVarLabel("WBTRUE    DC  'TRUE'", TRUE);
    writeVarLabel("WBFALSE    DC  'FALSE'", TRUE);
    writeVarLabel("WRITELINE", TRUE);
    writeObjectCode("RPUSH");
    writeObjectCode("LD  gr7, OBUFSIZE");
    writeObjectCode("LD  gr6, NEWLINE");
    writeObjectCode("ST  gr6, OBUF,gr7");
    writeObjectCode("ADDA  gr7, ONE");
    writeObjectCode("ST  gr7, OBUFSIZE");
    writeObjectCode("OUT  OBUF, OBUFSIZE");
    writeObjectCode("ST  gr0, OBUFSIZE");
    writeObjectCode("RPOP");
    writeObjectCode("RET");
    writeVarLabel("FLUSH", TRUE);
    writeObjectCode("RPUSH");
    writeObjectCode("LD gr7, OBUFSIZE");
    writeObjectCode("JZE FL1");
    writeObjectCode("CALL WRITELINE");
    writeVarLabel("FL1", TRUE);
    writeObjectCode("RPOP");
    writeObjectCode("RET");
    writeVarLabel("READCHAR", TRUE);
    writeObjectCode("RPUSH");
    writeObjectCode("LD  gr5, RPBBUF");
    writeObjectCode("JZE  RC0");
    writeObjectCode("ST  gr5, 0,gr1");
    writeObjectCode("ST  gr0, RPBBUF");
    writeVarLabel("RC0", TRUE);
    writeObjectCode("LD  gr7, INP");
    writeObjectCode("LD  gr6, IBUFSIZE");
    writeObjectCode("JNZ  RC1");
    writeObjectCode("IN  IBUF, IBUFSIZE");
    writeObjectCode("LD  gr7, gr0");
    writeVarLabel("RC1", TRUE);
    writeObjectCode("CPA  gr7, IBUFSIZE");
    writeObjectCode("JNZ  RC2");
    writeObjectCode("LD  gr5, NEWLINE");
    writeObjectCode("ST  gr5, 0,gr1");
    writeObjectCode("ST  gr0, IBUFSIZE");
    writeObjectCode("ST  gr0, INP");
    writeObjectCode("JUMP  RC3");
    writeVarLabel("RC2", TRUE);
    writeObjectCode("LD  gr5, IBUF,gr7");
    writeObjectCode("ADDA  gr7, ONE");
    writeObjectCode("ST  gr5, 0,gr1");
    writeObjectCode("ST  gr7, INP");
    writeVarLabel("RC3", TRUE);
    writeObjectCode("RPOP");
    writeObjectCode("RET");
    writeVarLabel("READINT", TRUE);
    writeObjectCode("RPUSH");
    writeVarLabel("RI1", TRUE);
    writeObjectCode("CALL  READCHAR");
    writeObjectCode("LD  gr7, 0,gr1");
    writeObjectCode("CPA  gr7, SPACE");
    writeObjectCode("JZE  RI1");
    writeObjectCode("CPA  gr7, TAB");
    writeObjectCode("JZE  RI1");
    writeObjectCode("CPA  gr7, NEWLINE");
    writeObjectCode("JZE  RI1");
    writeObjectCode("LD  gr5, ONE");
    writeObjectCode("CPA  gr7, MINUS");
    writeObjectCode("JNZ  RI4");
    writeObjectCode("LD  gr5, gr0");
    writeObjectCode("CALL  READCHAR");
    writeObjectCode("LD  gr7, 0,gr1");
    writeVarLabel("RI4", TRUE);
    writeObjectCode("LD  gr6, gr0");
    writeVarLabel("RI2", TRUE);
    writeObjectCode("CPA  gr7, ZERO");
    writeObjectCode("JMI  RI3");
    writeObjectCode("CPA  gr7, NINE");
    writeObjectCode("JPL  RI3");
    writeObjectCode("MULA  gr6, TEN");
    writeObjectCode("ADDA  gr6, gr7");
    writeObjectCode("SUBA  gr6, ZERO");
    writeObjectCode("CALL  READCHAR");
    writeObjectCode("LD  gr7, 0,gr1");
    writeObjectCode("JUMP  RI2");
    writeVarLabel("RI3", TRUE);
    writeObjectCode("ST  gr7, RPBBUF");
    writeObjectCode("ST  gr6, 0,gr1");
    writeObjectCode("CPA  gr5, gr0");
    writeObjectCode("JNZ  RI5");
    writeObjectCode("SUBA  gr5, gr6");
    writeObjectCode("ST  gr5, 0,gr1");
    writeVarLabel("RI5", TRUE);
    writeObjectCode("RPOP");
    writeObjectCode("RET");
    writeVarLabel("READLINE", TRUE);
    writeObjectCode("ST  gr0, IBUFSIZE");
    writeObjectCode("ST  gr0, INP");
    writeObjectCode("ST  gr0, RPBBUF");
    writeObjectCode("RET");

    writeObjectCodeWithoutTab("ONE    DC  1");
    writeObjectCodeWithoutTab("SIX    DC  6");
    writeObjectCodeWithoutTab("TEN    DC  10");
    writeObjectCodeWithoutTab("SPACE    DC  #0020");
    writeObjectCodeWithoutTab("MINUS    DC  #002D");
    writeObjectCodeWithoutTab("TAB    DC  #0009");
    writeObjectCodeWithoutTab("ZERO    DC  #0030");
    writeObjectCodeWithoutTab("NINE    DC  #0039");
    writeObjectCodeWithoutTab("NEWLINE    DC  #000A");
    writeObjectCodeWithoutTab("INTBUF    DS  8");
    writeObjectCodeWithoutTab("OBUFSIZE  DC  0");
    writeObjectCodeWithoutTab("IBUFSIZE  DC  0");
    writeObjectCodeWithoutTab("INP    DC  0");
    writeObjectCodeWithoutTab("OBUF    DS  257");
    writeObjectCodeWithoutTab("IBUF    DS  257");
}

void finalizeCompiler() {
    fclose(fp);
}