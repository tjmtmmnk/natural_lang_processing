#include "syntatic_analysis.h"
#include "lexical_analysis.h"
#include "cross_reference.h"
#include "output_objectfile.h"

static int token;
static int can_call; // flag for recursive call regulation
static int is_array;
static int is_formal_param;
static int is_address_hand; // if false -> value hand
static int is_main_call;
static int is_need_malloc;
static int access_idx;
static int tab_num;
static int break_label;
static int cnt_iteration, cnt_break;
static int while_nest;
static int parseStandardType(int _is_array, int has_set_type);
static int parseArrayType();
static int parseType();
static int parseName();
static int parseVarNames();
static int parseVarDecler();
static int parseFormalParam();
static int parseTerm();
static int parseSimpleExpression();
static int parseExpression();
static int parseFactor();
static int parseSubProgramDecler();
static int parseConditionState();
static int parseIterationState();
static int parseCompoundState();
static int parseCallState();
static int parseExpressions(int *exp_num, int *types);
static int parseAssignState();
static int parseInputState();
static int parseOutputFormat();
static int parseOutputState();
static int parseState();
static int parseVariable();
static int parseBlock();
static void scanWithErrorJudge();
static void printWithTub(char *str, int tab_num, int exist_space);

static void scanWithErrorJudge() {
    if ((token = scanTokenOneEach()) == NONE) { exit(1); }
}

static void printWithTub(char *str, int tab_num, int exist_space) {
    rep(i, 0, tab_num) {
        printf("    ");
    }
    if (exist_space) {
        printf("%s ", str);
    } else {
        printf("%s", str);
    }
}

static int parseStandardType(int _is_array, int size) {
    if (token == TINTEGER || token == TBOOLEAN || token == TCHAR) {
        int type = token;
        if (updateExIDType(token, _is_array, size) == ERROR) { return ERROR; }
        printf("%s", token_str[token]);
        scanWithErrorJudge();
        return type;
    }
    return errorWithReturn(getLineNum(), "'standard type' is not found");
}

static int parseArrayType() {
    if (token == TARRAY) {
        printf("array");
        scanWithErrorJudge();

        if (token != TLSQPAREN) { return errorWithReturn(getLineNum(), "'[' is not found"); }

        printf("[");
        scanWithErrorJudge();

        if (token != TNUMBER) { return errorWithReturn(getLineNum(), "'NUMBER' is not found"); }

        printf("%s", getStrAttr());
        int size = 0;
        sscanf(getStrAttr(), "%d", &size);
        scanWithErrorJudge();

        if (token != TRSQPAREN) { return errorWithReturn(getLineNum(), "']' is not found"); }

        printf("] ");
        scanWithErrorJudge();

        if (token != TOF) { return errorWithReturn(getLineNum(), "'of' is not found"); }

        printf("of ");
        scanWithErrorJudge();
        int type;
        if ((type = parseStandardType(TRUE, size)) == ERROR) { return ERROR; }

        return keywordToType(type, TRUE);
    }
    return errorWithReturn(getLineNum(), "'array' is not found");
}

static int parseType() {
    if (token == TINTEGER || token == TBOOLEAN || token == TCHAR || token == TARRAY) {
        int type;
        if (token == TINTEGER || token == TBOOLEAN || token == TCHAR) {
            if ((type = parseStandardType(FALSE, 0)) == ERROR) { return ERROR; }
        }

        if (token == TARRAY) {
            if ((type = parseArrayType()) == ERROR) { return ERROR; }
        }
        return type;
    }
    return errorWithReturn(getLineNum(), "type error");
}

static int parseName() {
    if (token == TNAME) {
        printWithTub(getStrAttr(), 0, FALSE);
        scanWithErrorJudge();
        return OK;
    }
    return errorWithReturn(getLineNum(), "'NAME' is not found");
}

static int parseVarNames() {
    if (token == TNAME) {
        int sp = 0; // stack pointer
        char stack[100][MAX_WORD_LENGTH];
        if (is_formal_param) {
            char label[MAX_WORD_LENGTH] = {'\0'};
            snprintf(label, MAX_WORD_LENGTH, "$%s%%%s", getStrAttr(), getProcName());
            strcpy(stack[sp++], label);
        }
        if (registerExID(getStrAttr(), getLineNum(), FALSE, is_formal_param) == ERROR) { return ERROR; }
        if (parseName() == ERROR) { return ERROR; }

        while (token == TCOMMA) {
            printf(", ");
            scanWithErrorJudge();
            if (registerExID(getStrAttr(), getLineNum(), FALSE, is_formal_param) == ERROR) { return ERROR; }
            if (is_formal_param) {
                char label[MAX_WORD_LENGTH] = {'\0'};
                snprintf(label, MAX_WORD_LENGTH, "$%s%%%s", getStrAttr(), getProcName());
                strcpy(stack[sp++], label);
            }
            if (parseName() == ERROR) { return ERROR; }
        }

        while (sp--) {
            writeObjectCode("POP\tgr1");
            writeObjectCode("ST\tgr1,\t%s", stack[sp]);
        }

        return OK;
    }
    return errorWithReturn(getLineNum(), "'var name' is not found");
}

static int parseVarDecler() {
    if (token == TVAR) {
        printWithTub("var\n", tab_num, FALSE);
        scanWithErrorJudge();
        tab_num++;
        printWithTub("", tab_num, FALSE);
        if (parseVarNames() == ERROR) { return ERROR; }
        tab_num--;

        if (token != TCOLON) { return errorWithReturn(getLineNum(), "':' is not found"); }

        printf(": ");
        scanWithErrorJudge();

        if (parseType() == ERROR) { return ERROR; }

        if (token != TSEMI) { return errorWithReturn(getLineNum(), "';' is not found"); }
        printf(";\n");
        scanWithErrorJudge();

        while (token == TNAME) {
            tab_num++;
            printWithTub("", tab_num, FALSE);
            if (parseVarNames() == ERROR) { return ERROR; }
            tab_num--;

            if (token != TCOLON) { return errorWithReturn(getLineNum(), "':' is not found"); }

            printf(": ");
            scanWithErrorJudge();

            if (parseType() == ERROR) { return ERROR; }

            if (token != TSEMI) { return errorWithReturn(getLineNum(), "';' is not found"); }

            printf(";\n");
            scanWithErrorJudge();
        }

        return OK;
    }
    return errorWithReturn(getLineNum(), "'var' is not found");
}

static int parseFormalParam() {
    if (token == TLPAREN) {
        setScope(LOCAL);
        is_formal_param = TRUE;
        writeObjectCode("POP\tgr2");

        scanWithErrorJudge();
        printf("(");
        if (parseVarNames() == ERROR) { return ERROR; }
        printf(" ");

        if (token != TCOLON) { return errorWithReturn(getLineNum(), "':' is not found"); }

        printf(": ");
        scanWithErrorJudge();
        int type;
        if ((type = parseType()) == ERROR) { return ERROR; }
        if (!isStandardType(type)) { return errorWithReturn(getLineNum(), "must be standard type"); }

        while (token == TSEMI) {
            scanWithErrorJudge();
            printf("; ");
            if (parseVarNames() == ERROR) { return ERROR; }

            if (token != TCOLON) { return errorWithReturn(getLineNum(), "':' is not found"); }

            printf(" : ");
            scanWithErrorJudge();

            if ((type = parseType()) == ERROR) { return ERROR; }
            if (!isStandardType(type)) { return errorWithReturn(getLineNum(), "must be standard type"); }
        }

        if (token != TRPAREN) { return errorWithReturn(getLineNum(), "')' is not found"); }

        printf(")");
        scanWithErrorJudge();

        writeObjectCode("PUSH\t0,gr2");

        is_formal_param = FALSE;
        return OK;
    }
    return errorWithReturn(getLineNum(), "'(' is not found");
}

static int parseTerm() {
    if (token == TNAME || token == TNUMBER || token == TFALSE || token == TTRUE || token == TSTRING ||
        token == TLPAREN || token == TNOT || token == TINTEGER || token == TBOOLEAN || token == TCHAR ||
        token == TPLUS || token == TMINUS || token == TSTRING) {
        int type, result_type;
        int is_simple_factor = TRUE;
        if ((type = parseFactor()) == ERROR) { return ERROR; }

        while (token == TSTAR || token == TDIV || token == TAND) {
            is_need_malloc = TRUE;
            is_address_hand = FALSE;
            is_simple_factor = FALSE;
            writeObjectCode("PUSH\t0,gr1");
            int ope = token; //multi operator
            printf(" ");
            printWithTub(token_str[token], 0, TRUE);
            scanWithErrorJudge();

            if (ope == TAND && type != TPBOOL) { return errorWithReturn(getLineNum(), "must be boolean"); }
            if (ope == TAND) { result_type = TPBOOL; }
            if ((ope == TSTAR || ope == TDIV) && (type != TPINT)) {
                return errorWithReturn(getLineNum(), "must be integer");
            }
            if ((ope == TSTAR || ope == TDIV)) { result_type = TPINT; }

            if ((type = parseFactor()) == ERROR) { return ERROR; }

            writeTermObjectCode(ope);
        }
        if (is_simple_factor) { result_type = type; }

        return result_type;
    }
    return errorWithReturn(getLineNum(), "term error");
}

static int parseFactor() {
    if (token != TNAME && token != TNUMBER && token != TSTRING) {
        printWithTub(getStrAttr(), 0, FALSE);
    }
    int type;
    int number = -1, exp_type = -1;
    int select_token = token;
    char str[MAX_WORD_LENGTH] = {'\0'};
    switch (token) {
        case TNAME: {
            if ((type = parseVariable()) == ERROR) { return ERROR; }
            break;
        }
        case TFALSE:
        case TTRUE: {
            is_need_malloc = TRUE;
            type = TPBOOL;
            scanWithErrorJudge();
            break;
        }
        case TNUMBER: {
            is_need_malloc = TRUE;
            type = TPINT;
            if (is_array) {
                sscanf(getStrAttr(), "%d", &access_idx);
            }
            sscanf(getStrAttr(), "%d", &number);
            printf("%s", getStrAttr());
            scanWithErrorJudge();
            break;
        }
        case TSTRING: {
            is_need_malloc = TRUE;
            strcpy(str, getStrAttr());
            printf("'");
            printf("%s", getStrAttr());
            printf("'");
            if (strlen(getStrAttr()) != 1) { return errorWithReturn(getLineNum(), "wrong word length"); }
            type = TPCHAR;
            scanWithErrorJudge();
            break;
        }
        case TLPAREN: {
            is_address_hand = FALSE;
            scanWithErrorJudge();

            if ((type = parseExpression()) == ERROR) { return ERROR; }

            if (token != TRPAREN) { return errorWithReturn(getLineNum(), "')' is not found"); }

            printf(")");
            scanWithErrorJudge();
            break;
        }
        case TNOT: {
            is_address_hand = FALSE;
            printf(" ");
            scanWithErrorJudge();
            if ((type = parseFactor()) != TPBOOL) { return errorWithReturn(getLineNum(), "must be boolean"); }
            break;
        }
        case TINTEGER:
        case TBOOLEAN:
        case TCHAR: {
            is_address_hand = FALSE;
            int standard_type = keywordToType(token, FALSE);
            type = standard_type;

            scanWithErrorJudge();

            if (token != TLPAREN) { return errorWithReturn(getLineNum(), "'(' is not found"); }

            printf("(");
            scanWithErrorJudge();

            if ((exp_type = parseExpression()) == ERROR) { return ERROR; }
            if (!isStandardType(exp_type)) { return errorWithReturn(getLineNum(), "must be standard type"); }

            if (token != TRPAREN) { return errorWithReturn(getLineNum(), "')' is not found"); }

            printf(")");
            scanWithErrorJudge();
            break;
        }
        default:
            return errorWithReturn(getLineNum(), "factor error");
    }
    writeFactorObjectCode(select_token, number, exp_type, str);
    return type;
}

static int parseConditionState() {
    int type;
    printf(" ");
    scanWithErrorJudge();
    is_address_hand = FALSE;

    if ((type = parseExpression()) == ERROR) { return ERROR; }
    if (type != TPBOOL) { return errorWithReturn(getLineNum(), "must be boolean"); }

    writeObjectCode("CPA\tgr1,gr0");
    writeObjectCodeRaw("\tJZE\t");
    writeJumpLabel(getIncLabel());
    int jump_label = getLabel();
    getIncLabel();

    if (token != TTHEN) { return errorWithReturn(getLineNum(), "'then' is not found"); }

    printf(" then\n");
    scanWithErrorJudge();
    tab_num++;

    if (parseState() == ERROR) { return ERROR; }

    if (token != TELSE) { tab_num--; }

    if (token == TELSE) {
        writeObjectCodeRaw("\tJUMP\t");
        writeJumpLabel(jump_label + 1);
        writeJumpLabel(jump_label);

        printf("\n");
        tab_num--;
        printWithTub("else\n", tab_num, FALSE);
        tab_num++;
        scanWithErrorJudge();
        if (parseState() == ERROR) { return ERROR; }
        writeJumpLabel(jump_label + 1);
        tab_num--;
    } else {
        writeJumpLabel(jump_label);
    }
    return OK;
}

static int parseSubProgramDecler() {
    if (token == TPROCEDURE) {
        setScope(GLOBAL);
        printWithTub("procedure", tab_num, TRUE);
        scanWithErrorJudge();

        setProcName(getStrAttr());
        writeVarLabel(getStrAttr(), TRUE);

        if (registerExID(getStrAttr(), getLineNum(), FALSE, FALSE) == ERROR) { return ERROR; }
        if (parseName() == ERROR) { return ERROR; }

        if (token == TLPAREN) {
            setScope(LOCAL);
            if (parseFormalParam() == ERROR) { return ERROR; }
        }

        if (token != TSEMI) { return errorWithReturn(getLineNum(), "';' is not found"); }

        printf(";\n");
        if (updateExIDTypeProcedure() == ERROR) { return ERROR; }
        scanWithErrorJudge();

        if (token == TVAR) {
            setScope(LOCAL);
            if (parseVarDecler() == ERROR) { return ERROR; }
        }
        can_call = FALSE;
        if (parseCompoundState() == ERROR) { return ERROR; }

        if (token != TSEMI) { return errorWithReturn(getLineNum(), "';' is not found"); }

        printf(";\n");

        writeObjectCode("RET");

        scanWithErrorJudge();

        return OK;
    }
    return errorWithReturn(getLineNum(), "'procedure' is not found");
}

static int parseSimpleExpression() {
    if (token == TNAME || token == TNUMBER || token == TFALSE || token == TTRUE || token == TSTRING ||
        token == TLPAREN || token == TNOT || token == TINTEGER || token == TBOOLEAN || token == TCHAR ||
        token == TPLUS || token == TMINUS) {
        int is_simple_term = TRUE;
        int type, result_type, sign;

        if (token == TPLUS || token == TMINUS) {
            is_address_hand = FALSE;
            is_simple_term = FALSE;
            sign = token;
            result_type = TPINT;
            printf(" ");
            printWithTub(token_str[token], 0, FALSE);
            scanWithErrorJudge();
        }

        if ((type = parseTerm()) == ERROR) { return ERROR; }
        if (!is_simple_term && (type != TPINT)) { return errorWithReturn(getLineNum(), "must be integer"); }

        if (sign == TMINUS) {
            writeObjectCode("POP\tgr2");
            writeObjectCode("LAD\tgr1,0");
            writeObjectCode("SUBA\tgr1,gr2");
            writeObjectCode("JOV\tEOVF");
            writeObjectCode("PUSH\t0,gr1");
        }

        while (token == TPLUS || token == TMINUS || token == TOR) {
            is_address_hand = FALSE;
            is_need_malloc = TRUE;
            is_simple_term = FALSE;
            writeObjectCode("PUSH\t0,gr1");
            int ope = token;
            printf(" ");
            printWithTub(token_str[token], 0, TRUE);
            scanWithErrorJudge(); //add operator

            if (ope == TOR && type != TPBOOL) { return errorWithReturn(getLineNum(), "must be boolean"); }
            if (ope == TOR) { result_type = TPBOOL; }

            if ((ope == TPLUS || ope == TMINUS) && (type != TPINT)) {
                return errorWithReturn(getLineNum(), "must be integer");
            }
            if ((ope == TPLUS || ope == TMINUS)) { result_type = TPINT; }

            if ((type = parseTerm()) == ERROR) { return ERROR; }

            writeSimpleExpObjectCode(ope);
        }

        if (is_simple_term) { result_type = type; }

        //if one-term -> don't need to pop

        return result_type;
    }
    return errorWithReturn(getLineNum(), "simple expression error");
}

static int parseExpression() {
    if (token == TNAME || token == TNUMBER || token == TFALSE || token == TTRUE || token == TSTRING ||
        token == TLPAREN || token == TNOT || token == TINTEGER || token == TBOOLEAN || token == TCHAR ||
        token == TPLUS || token == TMINUS) {
        int left_type, right_type;
        int is_simple_expression = TRUE;

        if ((left_type = parseSimpleExpression()) == ERROR) { return ERROR; }
        if (!isStandardType(left_type)) { return errorWithReturn(getLineNum(), "must be standard type"); }

        while (token == TEQUAL || token == TNOTEQ || token == TLE || token == TLEEQ || token == TGR || token == TGREQ) {
            is_need_malloc = TRUE;
            is_address_hand = FALSE;
            is_simple_expression = FALSE;
            int ope = token;
            writeObjectCode("PUSH\t0,gr1");
            printf(" ");
            printWithTub(token_str[token], 0, TRUE);
            scanWithErrorJudge(); //relational operator
            if ((right_type = parseSimpleExpression()) == ERROR) { return ERROR; }
            if (!isStandardType(right_type)) { return errorWithReturn(getLineNum(), "must be standard type"); }
            if (left_type != right_type) { return errorWithReturn(getLineNum(), "left and right factor must be same"); }

            writeExpObjectCode(ope);
        }
        return (is_simple_expression) ? left_type : TPBOOL;
    } else {
        return errorWithReturn(getLineNum(), "expression error");
    }
}

static int parseExpressions(int *exp_num, int *types) {
    if (token == TNAME || token == TNUMBER || token == TFALSE || token == TTRUE || token == TSTRING ||
        token == TLPAREN || token == TNOT || token == TINTEGER || token == TBOOLEAN || token == TCHAR ||
        token == TPLUS || token == TMINUS) {
        int type;
        is_address_hand = is_need_malloc;
        if ((type = parseExpression()) == ERROR) { return ERROR; }

        if (is_address_hand) {
            writeObjectCode("PUSH\t0,gr1");
        }
        int malloc_label = -1;
        if (is_need_malloc) {
            static int is_first = 1;
            if ((is_first--) == 1) { // To recycle memory, use the same address
                malloc_label = getIncLabel();
            }
            writeObjectCodeRaw("\tLAD\tgr2,\t");
            writeJumpLabel(malloc_label);
            registerDCLabel(malloc_label, NULL);
            writeObjectCode("ST\tgr1,0,gr2");
            writeObjectCode("PUSH\t0,gr2");
        }

        types[*exp_num] = type;
        *exp_num = *exp_num + 1;

        while (token == TCOMMA) {
            printf(", ");
            scanWithErrorJudge();

            is_address_hand = is_need_malloc;
            if ((type = parseExpression()) == ERROR) { return ERROR; }

            if (is_address_hand) {
                writeObjectCode("PUSH\t0,gr1");
            }
            if (is_need_malloc) {
                writeObjectCodeRaw("\tLAD\tgr2,\t");
                writeJumpLabel(malloc_label);
                registerDCLabel(malloc_label, NULL);
                writeObjectCode("ST\tgr1,0,gr2");
                writeObjectCode("PUSH\t0,gr2");
            }
            types[*exp_num] = type;
            *exp_num = *exp_num + 1;
        }
        return OK;
    }
    return errorWithReturn(getLineNum(), "expressions error");
}

static int parseIterationState() {
    is_address_hand = FALSE;
    int type;
    scanWithErrorJudge();

    printf(" ");

    writeJumpLabel(getIncLabel());
    int loop_label = getLabel();
    getIncLabel(); // while need break label

    if ((type = parseExpression()) == ERROR) { return ERROR; }
    writeObjectCode("CPA\tgr1,gr0");
    writeObjectCodeRaw("\tJZE\t");
    writeJumpLabel(loop_label + 1);
    break_label = loop_label + 1;

    if (type != TPBOOL) { return errorWithReturn(getLineNum(), "must be boolean"); }
    printf(" ");

    if (token != TDO) { return errorWithReturn(getLineNum(), "'do' is not found"); }

    printWithTub("do\n", 0, FALSE);
    scanWithErrorJudge();
    if (parseState() == ERROR) { return ERROR; }

    writeObjectCodeRaw("\tJUMP\t");
    writeJumpLabel(loop_label);

    writeJumpLabel(loop_label + 1);
    return OK;
}

static int parseCallState() {
    setScope(LOCAL);
    printf(" ");
    scanWithErrorJudge();
    int types[100];
    int exp_num = 0;
    char name[100];

    strcpy(name, getStrAttr());

    if (updateExIDRefLine(name, getLineNum(), TPPROC) == ERROR) { return ERROR; }

    if (!isPrevDefined(name)) { return errorWithReturn(getLineNum(), "undefine variable"); }

    if (!can_call) { return errorWithReturn(getLineNum(), "can't recursive call"); }

    if (parseName() == ERROR) { return ERROR; }

    if (token == TLPAREN) {
        printf("(");
        scanWithErrorJudge();

        if (parseExpressions(&exp_num, types) == ERROR) { return ERROR; }

        if (checkMatchDeclerVarAndCallExpression(name, exp_num, types) == ERROR) { return ERROR; }

        if (token != TRPAREN) {
            return errorWithReturn(getLineNum(), "')' is not found");
        }
        printf(")");
        scanWithErrorJudge();
    }

    writeObjectCode("CALL\t$%s", name);

    return OK;
}

static int parseAssignState() {
    int left_type, right_type;
    is_address_hand = TRUE;
    if ((left_type = parseVariable()) == ERROR) { return ERROR; }
    if (!isStandardType(left_type)) { return errorWithReturn(getLineNum(), "must be standard type"); }
    writeObjectCode("PUSH\t0,gr1");
    printf(" ");
    if (token != TASSIGN) {
        return errorWithReturn(getLineNum(), "':=' is not found");
    }
    printWithTub(":=", 0, TRUE);
    scanWithErrorJudge();

    is_address_hand = FALSE;
    if ((right_type = parseExpression()) == ERROR) { return ERROR; }

    if (!isStandardType(right_type)) { return errorWithReturn(getLineNum(), "must be standard type"); }

    if (left_type != right_type) { return errorWithReturn(getLineNum(), "left and right must be same type"); }

    writeObjectCode("POP\tgr2");
    writeObjectCode("ST\tgr1,0,gr2");

    return OK;
}

static int parseInputState() {
    int input_type = token; //TREAD or TREADLN
    int type;
    scanWithErrorJudge();

    if (token == TLPAREN) {
        is_address_hand = TRUE;
        printWithTub("(", 0, FALSE);
        scanWithErrorJudge();

        if ((type = parseVariable()) == ERROR) { return ERROR; }
        if ((type != TPINT) && (type != TPCHAR)) { return errorWithReturn(getLineNum(), "must be integer or char"); }

        while (token == TCOMMA) {
            printWithTub(",", 0, TRUE);
            scanWithErrorJudge();
            if ((type = parseVariable()) == ERROR) { return ERROR; }
            if ((type != TPINT) && (type != TPCHAR)) {
                return errorWithReturn(getLineNum(), "must be integer or char");
            }
        }
        if (token != TRPAREN) { return errorWithReturn(getLineNum(), "')' is not found"); }

        printWithTub(")", 0, FALSE);

        writeInputObjectCode(type);
        if (input_type == TREADLN) {
            writeObjectCode("CALL\tREADLINE");
        }

        scanWithErrorJudge();
    }
    return OK;
}

static int parseOutputState() {
    int write_type = token; // TWRITE or TWRITELN

    scanWithErrorJudge();

    if (token == TLPAREN) {
        is_address_hand = FALSE;
        printWithTub("(", 0, FALSE);
        scanWithErrorJudge();

        if (parseOutputFormat() == ERROR) { return ERROR; }

        while (token == TCOMMA) {
            printWithTub(",", 0, TRUE);
            scanWithErrorJudge();
            if (parseOutputFormat() == ERROR) { return ERROR; }
        }
        if (token != TRPAREN) { return errorWithReturn(getLineNum(), "')' is not found"); }

        printWithTub(")", 0, FALSE);

        scanWithErrorJudge();
    }

    if (write_type == TWRITELN) {
        writeObjectCode("CALL\tWRITELINE");
    }
    return OK;
}

static int parseOutputFormat() {
    if (token == TNAME || token == TNUMBER || token == TFALSE || token == TTRUE || token == TSTRING ||
        token == TLPAREN || token == TNOT || token == TINTEGER || token == TBOOLEAN || token == TCHAR ||
        token == TPLUS || token == TMINUS) {
        int type;
        if (token == TSTRING) {
            int str_len = strlen(getStrAttr());
            if (!(str_len == 0 || str_len >= 2)) {
                return errorWithReturn(getLineNum(), "string length wrong(0 or more than 2)");
            }

            writeObjectCodeRaw("\tLAD\tgr1,\t");
            writeJumpLabel(getIncLabel());
            writeOutputObjectCode(type);
            writeObjectCode("LD\tgr2,gr0");
            writeObjectCode("CALL\tWRITESTR");

            registerDCLabel(getLabel(), getStrAttr());

            printWithTub("'", 0, FALSE);
            printWithTub(getStrAttr(), 0, FALSE);
            printWithTub("'", 0, FALSE);
            scanWithErrorJudge();
        } else {
            is_address_hand = FALSE;
            if ((type = parseExpression()) == ERROR) { return ERROR; }
            if (!isStandardType(type)) { return errorWithReturn(getLineNum(), "must be standard type"); }

            int digits = 0;
            if (token == TCOLON) {
                printf(":");
                scanWithErrorJudge();
                sscanf(getStrAttr(), "%d", &digits);
                if (token != TNUMBER) { return errorWithReturn(getLineNum(), "'number' is not found"); }
                printf("%s", getStrAttr());
                scanWithErrorJudge();
            }
            //parseExpressionで変数のコードを生成
            if (digits > 0) {
                writeObjectCode("LAD\tgr2,%d", digits);
            } else {
                writeObjectCode("LD\tgr2,gr0");
            }
            writeOutputObjectCode(type);
        }

        return OK;
    }
    return errorWithReturn(getLineNum(), "output format error");
}

// [state] is nothing -> It dosen't need an error-branch.
static int parseState() {
    if (token != TNAME && token != TBEGIN) {
        printWithTub(token_str[token], tab_num, FALSE);
    }
    switch (token) {
        case TNAME: {
            printWithTub("", tab_num, FALSE);   // for indent
            if (parseAssignState() == ERROR) { return ERROR; }
            break;
        }
        case TIF: {
            if (parseConditionState() == ERROR) { return ERROR; }
            break;
        }
        case TWHILE: {
            cnt_iteration++;
            while_nest++;
            if (parseIterationState() == ERROR) { return ERROR; }
            while_nest--;
            break;
        }
        case TBREAK: {
            if (while_nest > 0) {
                cnt_break++;
            } else if (while_nest == 0) {
                return errorWithReturn(getLineNum(), "Can't include 'break' out of iteration state");
            }
            writeObjectCodeRaw("\tJUMP\t");
            writeJumpLabel(break_label);
            scanWithErrorJudge(); //parse break
            break;
        }
        case TCALL: {
            if (parseCallState() == ERROR) { return ERROR; }
            break;
        }
        case TRETURN:
            writeObjectCode("RET");
            scanWithErrorJudge();
            break;
        case TREAD:
        case TREADLN: {
            if (parseInputState() == ERROR) { return ERROR; }
            break;
        }
        case TWRITE:
        case TWRITELN: {
            if (parseOutputState() == ERROR) { return ERROR; }
            break;
        }
        case TBEGIN: {
            if (parseCompoundState() == ERROR) { return ERROR; }
            break;
        }
        default:
            scanWithErrorJudge(); //empty state
            break;
    }
    return OK;
}

static int parseCompoundState() {
    if (token == TBEGIN) {
        if (!is_main_call) {
            is_need_malloc = FALSE;
        }
        printWithTub("begin\n", tab_num, FALSE);
        scanWithErrorJudge();
        tab_num++;

        if (parseState() == ERROR) { return ERROR; }

        while (token == TSEMI) {
            printf(";\n");
            scanWithErrorJudge();
            if (parseState() == ERROR) { return ERROR; }
        }

        if (token == TEND) { printf("\n"); }

        if (token != TEND) { return errorWithReturn(getLineNum(), "'end' is not found"); }

        tab_num--;
        printWithTub("end", tab_num, FALSE);
        scanWithErrorJudge();
        return OK;
    }
    return errorWithReturn(getLineNum(), "'begin' is not found");
}

// equal to "left part"
static int parseVariable() {
    if (token == TNAME) {
        is_need_malloc = FALSE;
        is_array = FALSE;
        int is_use_local = TRUE;
        int line = getLineNum();
        char name[100];
        strcpy(name, getStrAttr());

        int global_type = getGlobalVarType(name);
        int local_type = getLocalVarType(name);

        if (!isPrevDefined(getStrAttr())) {
            return errorWithReturn(getLineNum(), "undefine variable");
        }
        if (parseName() == ERROR) { return ERROR; }

        if (token == TLSQPAREN) { //array
            is_array = TRUE;
            int array_size = 0;

            // if both local and global variable are not 'array' -> compile error
            if (local_type != 0 && !isStandardType(local_type)) { // local var is array type
                is_use_local = TRUE;
                if (updateExIDRefLine(name, line, local_type) == ERROR) { return ERROR; }
                if ((local_type = arrayTypeToStandardType(local_type)) == ERROR) { return ERROR; }
                array_size = getArraySize(LOCAL, name);
            } else if (global_type != 0 && !isStandardType(global_type)) { // global var is array type
                is_use_local = FALSE;
                if (updateExIDRefLine(name, line, global_type) == ERROR) { return ERROR; }
                if ((global_type = arrayTypeToStandardType(global_type)) == ERROR) { return ERROR; }
                array_size = getArraySize(GLOBAL, name);
            } else {
                return errorWithReturn(getLineNum(), "must be array type");
            }

            printf("[");
            scanWithErrorJudge();
            if (parseExpression() == ERROR) { return ERROR; }
            writeObjectCode("PUSH\t0,gr1");

            int _scope = (local_type) ? LOCAL : GLOBAL;
            writeArrayVarObjectCode(_scope, is_address_hand, name, array_size);

            if (token != TRSQPAREN) { return errorWithReturn(getLineNum(), "']' is not found"); }
            printf("]");
            scanWithErrorJudge();
        }
        if (!is_array) {
            if (local_type != 0 && isStandardType(local_type)) {
                is_use_local = TRUE;
                if (updateExIDRefLine(name, line, local_type) == ERROR) { return ERROR; }
            } else if (global_type != 0 && isStandardType(global_type)) {
                is_use_local = FALSE;
                if (updateExIDRefLine(name, line, global_type) == ERROR) { return ERROR; }
            } else {
                return errorWithReturn(getLineNum(), "must be standard type");
            }

            int _scope = (local_type) ? LOCAL : GLOBAL;
            writeStandardVarObjectCode(_scope, is_address_hand, name);
        }
        return (is_use_local) ? local_type : global_type;
    }
    return errorWithReturn(getLineNum(), "undefineded the variable name");
}

static int parseBlock() {
    if (token == TVAR || token == TPROCEDURE || token == TBEGIN) {
        while (token == TVAR || token == TPROCEDURE) {
            if (token == TVAR) {
                setScope(GLOBAL);
                setProcName("global");
                if (parseVarDecler() == ERROR) { return ERROR; }
            }
            if (token == TPROCEDURE) {
                if (parseSubProgramDecler() == ERROR) { return ERROR; }
            }
        }
        if (token != TBEGIN) { return errorWithReturn(getLineNum(), "'begin' is not found"); }
        writeJumpLabel(1);

        can_call = TRUE;
        is_main_call = TRUE;
        if (parseCompoundState() == ERROR) { return ERROR; }
        is_main_call = FALSE;
        can_call = FALSE;

        writeObjectCode("RET");

        return OK;
    }
    return errorWithReturn(getLineNum(), "unexpected beginning character in block");
}

/****************** public function *********************/
int parseProgram() {
    scanWithErrorJudge();
    // initialize
    tab_num = 0;
    cnt_iteration = 0;
    cnt_break = 0;
    while_nest = 0;
    access_idx = 0;
    break_label = 0;
    can_call = FALSE;
    is_array = FALSE;
    is_formal_param = FALSE;
    is_address_hand = FALSE;
    is_need_malloc = FALSE;
    is_main_call = FALSE;
    initGlobalID();
    initLocalID();

    if (token != TPROGRAM) { return errorWithReturn(getLineNum(), "'program' is not found"); }

    printWithTub("program", 0, TRUE);
    scanWithErrorJudge();

    if (token != TNAME) { return errorWithReturn(getLineNum(), "'program name' is not found"); }

    writeVarLabel("", FALSE); // dummy for $$[label]
    writeVarLabel(getStrAttr(), FALSE);
    writeObjectCode("START");
    writeObjectCode("LAD\tgr0,0");
    writeObjectCodeRaw("\tCALL\t");
    writeJumpLabel(getIncLabel());
    writeObjectCode("CALL\tFLUSH");
    writeObjectCode("SVC 0");

    printWithTub(getStrAttr(), 0, FALSE);
    scanWithErrorJudge();

    if (token != TSEMI) { return errorWithReturn(getLineNum(), "';' is not found"); }

    printWithTub(";\n", 0, FALSE);
    scanWithErrorJudge();

    tab_num++;
    if (parseBlock() == ERROR) { return ERROR; }

    if (token != TDOT) { return errorWithReturn(getLineNum(), "'.' is not found"); }

    printf(".\n");

//    if (cnt_iteration > 0 && cnt_break == 0) {
    // can't know line number -> can't return errorWithReturn()
//        fprintf(stderr, "[ERROR] : 'break' must be included at least one in iteration statement\n");
//        return ERROR;
//    }
    writeMalloc();
    writeDCLabel();
    writeLibrary();
    writeObjectCode("END");
    return OK;
}