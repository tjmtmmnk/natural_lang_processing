#include "syntatic_analysis.h"
#include "lexical_analysis.h"
#include "cross_reference.h"

static int token;
static int can_call; // flag for recursive call regulation
static int tab_num;
static int cnt_iteration, cnt_break;
static int while_nest;
static int parseStandardType(int is_array, int has_set_type);
static int parseArrayType();
static int parseType();
static int parseVarName();
static int parseProcName();
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
static int parseExpressions();
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
    if ((token = scanTokenOneEach()) == NONE) {
        exit(1);
    }
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

static int parseStandardType(int is_array, int size) {
    if (token == TINTEGER || token == TBOOLEAN || token == TCHAR) {
        int type = token;
        updateExIDType(token, is_array, size);
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

        if (token != TLSQPAREN) {
            return errorWithReturn(getLineNum(), "'[' is not found");
        }
        printf("[");
        scanWithErrorJudge();

        if (token != TNUMBER) {
            return errorWithReturn(getLineNum(), "'NUMBER' is not found");
        }
        printf("%s", getStrAttr());
        int size = 0;
        sscanf(getStrAttr(), "%d", &size);
        scanWithErrorJudge();

        if (token != TRSQPAREN) {
            return errorWithReturn(getLineNum(), "']' is not found");
        }
        printf("] ");
        scanWithErrorJudge();

        if (token != TOF) {
            return errorWithReturn(getLineNum(), "'of' is not found");
        }
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

static int parseVarName() {
    if (token == TNAME) {
        printWithTub(getStrAttr(), 0, FALSE);
        scanWithErrorJudge();
        return OK;
    }
    return errorWithReturn(getLineNum(), "'NAME' is not found");
}

static int parseProcName() {
    if (token == TNAME) {
        printWithTub(getStrAttr(), 0, FALSE);
        scanWithErrorJudge();
        return OK;
    }
    return errorWithReturn(getLineNum(), "'NAME' is not found");
}

static int parseVarNames() {
    if (token == TNAME) {
        registerExID(getStrAttr(), getLineNum(), FALSE);
        if (parseVarName() == ERROR) { return ERROR; }

        while (token == TCOMMA) {
            printf(", ");
            scanWithErrorJudge();
            registerExID(getStrAttr(), getLineNum(), FALSE);
            if (parseVarName() == ERROR) { return ERROR; }
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

        if (token != TCOLON) {
            return errorWithReturn(getLineNum(), "':' is not found");
        }
        printf(": ");
        scanWithErrorJudge();

        if (parseType() == ERROR) { return ERROR; }

        if (token != TSEMI) {
            return errorWithReturn(getLineNum(), "';' is not found");
        }
        printf(";\n");
        scanWithErrorJudge();

        while (token == TNAME) {
            tab_num++;
            printWithTub("", tab_num, FALSE);
            if (parseVarNames() == ERROR) { return ERROR; }
            tab_num--;

            if (token != TCOLON) {
                return errorWithReturn(getLineNum(), "':' is not found");
            }
            printf(": ");
            scanWithErrorJudge();

            if (parseType() == ERROR) { return ERROR; }

            if (token != TSEMI) {
                return errorWithReturn(getLineNum(), "';' is not found");
            }
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
        scanWithErrorJudge();
        printf("(");

        if (parseVarNames() == ERROR) { return ERROR; }
        printf(" ");

        if (token != TCOLON) {
            return errorWithReturn(getLineNum(), "':' is not found");
        }
        printf(": ");
        scanWithErrorJudge();
        int type;
        if ((type = parseType()) == ERROR) { return ERROR; }
        if (!isStandardType(type)) { return errorWithReturn(getLineNum(), "must be standard type"); }

        while (token == TSEMI) {
            scanWithErrorJudge();
            printf("; ");

            if (parseVarNames() == ERROR) { return ERROR; }

            if (token != TCOLON) {
                return errorWithReturn(getLineNum(), "':' is not found");
            }
            printf(" : ");
            scanWithErrorJudge();

            if ((type = parseType()) == ERROR) { return ERROR; }
            if (!isStandardType(type)) { return errorWithReturn(getLineNum(), "must be standard type"); }
        }

        if (token != TRPAREN) {
            return errorWithReturn(getLineNum(), "')' is not found");
        }
        printf(")");
        scanWithErrorJudge();
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
            int ope = token;
            is_simple_factor = FALSE;
            printf(" ");
            printWithTub(token_str[token], 0, TRUE);
            scanWithErrorJudge(); //multi operator

            if (ope == TAND && type != TPBOOL) { return errorWithReturn(getLineNum(), "must be boolean"); }
            if (ope == TAND) { result_type = TPBOOL; }
            if ((ope == TSTAR || ope == TDIV) && (type != TPINT)) {
                return errorWithReturn(getLineNum(), "must be integer");
            }
            if ((ope == TSTAR || ope == TDIV)) { result_type = TPINT; }

            if ((type = parseFactor()) == ERROR) { return ERROR; }
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
    switch (token) {
        case TNAME: {
            if ((type = parseVariable()) == ERROR) { return ERROR; }
            break;
        }
        case TFALSE:
        case TTRUE: {
            type = TPBOOL;
            scanWithErrorJudge();
            break;
        }
        case TNUMBER: {
            type = TPINT;
            printf("%s", getStrAttr());
            scanWithErrorJudge();
            break;
        }
        case TSTRING: {
            printf("'");
            printf("%s", getStrAttr());
            printf("'");
            if (strlen(getStrAttr()) > 1) { return errorWithReturn(getLineNum(), "too long words"); }
            type = TPCHAR;
            scanWithErrorJudge();
            break;
        }
        case TLPAREN: {
            scanWithErrorJudge();

            if ((type = parseExpression()) == ERROR) { return ERROR; }

            if (token != TRPAREN) {
                return errorWithReturn(getLineNum(), "')' is not found");
            }
            printf(")");
            scanWithErrorJudge();
            break;
        }
        case TNOT: {
            printf(" ");
            scanWithErrorJudge();
            if ((type = parseFactor()) != TPBOOL) { return errorWithReturn(getLineNum(), "must be boolean"); }
            break;
        }
        case TINTEGER:
        case TBOOLEAN:
        case TCHAR: {
            int _token = token;
            scanWithErrorJudge();

            if (token != TLPAREN) {
                return errorWithReturn(getLineNum(), "'(' is not found");
            }
            printf("(");
            scanWithErrorJudge();

            if (parseExpression() == ERROR) { return ERROR; }

            if (token != TRPAREN) {
                return errorWithReturn(getLineNum(), "')' is not found");
            }
            printf(")");
            scanWithErrorJudge();
            if (_token == TINTEGER) {
                return TPINT;
            } else if (_token == TBOOLEAN) {
                return TPBOOL;
            } else if (_token == TCHAR) {
                return TPCHAR;
            }
            break;
        }
        default:
            return errorWithReturn(getLineNum(), "factor error");
    }
    return type;
}

static int parseConditionState() {
    printf(" ");
    scanWithErrorJudge();

    if (parseExpression() == ERROR) { return ERROR; }

    if (token != TTHEN) {
        return errorWithReturn(getLineNum(), "'then' is not found");
    }
    printf(" then\n");
    scanWithErrorJudge();
    tab_num++;

    if (parseState() == ERROR) { return ERROR; }

    if (token != TELSE) {
        tab_num--;
    }
    if (token == TELSE) {
        printf("\n");
        tab_num--;
        printWithTub("else\n", tab_num, FALSE);
        tab_num++;
        scanWithErrorJudge();
        if (parseState() == ERROR) { return ERROR; }
        tab_num--;
    }
    return OK;
}

static int parseSubProgramDecler() {
    if (token == TPROCEDURE) {
        setScope(GLOBAL);
        printWithTub("procedure", tab_num, TRUE);
        scanWithErrorJudge();

        setProcName(getStrAttr());
        registerExID(getStrAttr(), getLineNum(), FALSE);
        if (parseProcName() == ERROR) { return ERROR; }

        if (token == TLPAREN) {
            setScope(LOCAL);
            if (parseFormalParam() == ERROR) { return ERROR; }
        }

        if (token != TSEMI) {
            return errorWithReturn(getLineNum(), "';' is not found");
        }
        printf(";\n");
        updateExIDTypeProcedure();
        scanWithErrorJudge();

        if (token == TVAR) {
            setScope(LOCAL);
            if (parseVarDecler() == ERROR) { return ERROR; }
        }
        can_call = FALSE;
        if (parseCompoundState() == ERROR) { return ERROR; }

        if (token != TSEMI) {
            return errorWithReturn(getLineNum(), "';' is not found");
        }
        printf(";\n");

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
        int type, result_type;

        if (token == TPLUS || token == TMINUS) {
            is_simple_term = FALSE;
            result_type = TPINT;
            printf(" ");
            printWithTub(token_str[token], 0, FALSE);
            scanWithErrorJudge();
        }

        if ((type = parseTerm()) == ERROR) { return ERROR; }
        if (is_simple_term && (type != TPINT)) {
            return errorWithReturn(getLineNum(), "must be integer");
        }
        if (is_simple_term) {
            result_type = type;
        }

        while (token == TPLUS || token == TMINUS || token == TOR) {
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
        }
        return result_type;
    }
    return errorWithReturn(getLineNum(), "simple expression error");
}

static int parseExpression() {
    if (token == TNAME || token == TNUMBER || token == TFALSE || token == TTRUE || token == TSTRING ||
        token == TLPAREN || token == TNOT || token == TINTEGER || token == TBOOLEAN || token == TCHAR ||
        token == TPLUS || token == TMINUS) {
        int left_type, right_type;
        if ((left_type = parseSimpleExpression()) == ERROR) { return ERROR; }

        while (token == TEQUAL || token == TNOTEQ || token == TLE || token == TLEEQ || token == TGR || token == TGREQ) {
            printf(" ");
            printWithTub(token_str[token], 0, TRUE);
            scanWithErrorJudge(); //relational operator
            if ((right_type = parseSimpleExpression()) == ERROR) { return ERROR; }
            if (left_type != right_type) { return errorWithReturn(getLineNum(), "left and right factor must be same"); }
        }
        return OK;
    } else {
        return errorWithReturn(getLineNum(), "expression error");
    }
}

static int parseExpressions() {
    if (token == TNAME || token == TNUMBER || token == TFALSE || token == TTRUE || token == TSTRING ||
        token == TLPAREN || token == TNOT || token == TINTEGER || token == TBOOLEAN || token == TCHAR ||
        token == TPLUS || token == TMINUS) {
        if (parseExpression() == ERROR) { return ERROR; }

        while (token == TCOMMA) {
            printf(", ");
            scanWithErrorJudge();
            if (parseExpression() == ERROR) { return ERROR; }
        }
        return OK;
    }
    return errorWithReturn(getLineNum(), "expressions error");
}

static int parseIterationState() {
    scanWithErrorJudge();

    printf(" ");
    if (parseExpression() == ERROR) { return ERROR; }
    printf(" ");

    if (token != TDO) {
        return errorWithReturn(getLineNum(), "'do' is not found");
    }
    printWithTub("do\n", 0, FALSE);
    scanWithErrorJudge();
    if (parseState() == ERROR) { return ERROR; }
    return OK;
}

static int parseCallState() {
    setScope(LOCAL);
    printf(" ");
    scanWithErrorJudge();

    updateExIDRefLine(getStrAttr(), getLineNum(), TPPROC);
    if (!isPrevDefined(getStrAttr())) {
        return errorWithReturn(getLineNum(), "undefine variable");
    }
    if (!can_call) {
        return errorWithReturn(getLineNum(), "can't recursive call");
    }

    if (parseProcName() == ERROR) { return ERROR; }

    if (token == TLPAREN) {
        printf("(");
        scanWithErrorJudge();

        if (parseExpressions() == ERROR) { return ERROR; }

        if (token != TRPAREN) {
            return errorWithReturn(getLineNum(), "')' is not found");
        }
        printf(")");
        scanWithErrorJudge();
    }
    return OK;
}

static int parseAssignState() {
    int type;
    if ((type = parseVariable()) == ERROR) { return ERROR; }
    printf(" ");
    if (token != TASSIGN) {
        return errorWithReturn(getLineNum(), "':=' is not found");
    }
    printWithTub(":=", 0, TRUE);
    scanWithErrorJudge();

    if (parseExpression() == ERROR) { return ERROR; }
    return OK;
}

static int parseInputState() {
    scanWithErrorJudge();

    if (token == TLPAREN) {
        printWithTub("(", 0, FALSE);
        scanWithErrorJudge();

        if (parseVariable() == ERROR) { return ERROR; }

        while (token == TCOMMA) {
            printWithTub(",", 0, TRUE);
            scanWithErrorJudge();
            if (parseVariable() == ERROR) { return ERROR; }
        }
        if (token != TRPAREN) {
            return errorWithReturn(getLineNum(), "')' is not found");
        }
        printWithTub(")", 0, FALSE);
        scanWithErrorJudge();
    }
    return OK;
}

static int parseOutputState() {
    scanWithErrorJudge();

    if (token == TLPAREN) {
        printWithTub("(", 0, FALSE);
        scanWithErrorJudge();

        if (parseOutputFormat() == ERROR) { return ERROR; }

        while (token == TCOMMA) {
            printWithTub(",", 0, TRUE);
            scanWithErrorJudge();
            if (parseOutputFormat() == ERROR) { return ERROR; }
        }
        if (token != TRPAREN) {
            return errorWithReturn(getLineNum(), "')' is not found");
        }
        printWithTub(")", 0, FALSE);
        scanWithErrorJudge();
    }
    return OK;
}

static int parseOutputFormat() {
    if (token == TNAME || token == TNUMBER || token == TFALSE || token == TTRUE || token == TSTRING ||
        token == TLPAREN || token == TNOT || token == TINTEGER || token == TBOOLEAN || token == TCHAR ||
        token == TPLUS || token == TMINUS) {
        if (token == TSTRING) {
            printWithTub("'", 0, FALSE);
            printWithTub(getStrAttr(), 0, FALSE);
            printWithTub("'", 0, FALSE);
            scanWithErrorJudge();
        } else {
            if (parseExpression() == ERROR) { return ERROR; }

            if (token == TCOLON) {
                printf(":");
                scanWithErrorJudge();
                if (token != TNUMBER) {
                    return errorWithReturn(getLineNum(), "'number' is not found");
                }
                printf("%s", getStrAttr());
                scanWithErrorJudge();
            }
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
            scanWithErrorJudge(); //parse break
            break;
        }
        case TCALL: {
            if (parseCallState() == ERROR) { return ERROR; }
            break;
        }
        case TRETURN:
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
        printWithTub("begin\n", tab_num, FALSE);
        scanWithErrorJudge();
        tab_num++;

        if (parseState() == ERROR) { return ERROR; }

        while (token == TSEMI) {
            printf(";\n");
            scanWithErrorJudge();
            if (parseState() == ERROR) { return ERROR; }
        }

        if (token == TEND) {
            printf("\n");
        }

        if (token != TEND) {
            return errorWithReturn(getLineNum(), "'end' is not found");
        }

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
        int global_type = getGlobalVarType(getStrAttr());
        int local_type = getLocalVarType(getStrAttr());
        int is_array = FALSE;
        int is_use_local = TRUE;
        char name[100];
        strcpy(name, getStrAttr());
        int line = getLineNum();

        if (!isPrevDefined(getStrAttr())) {
            return errorWithReturn(getLineNum(), "undefine variable");
        }
        if (parseVarName() == ERROR) { return ERROR; }
        if (token == TLSQPAREN) {
            is_array = TRUE;
            // if both local and global variable are not 'array' -> compile error
            if (local_type != 0 && !isStandardType(local_type)) {
                is_use_local = TRUE;
                updateExIDRefLine(name, line, local_type);
            } else if (global_type != 0 && !isStandardType(global_type)) {
                is_use_local = FALSE;
                updateExIDRefLine(getStrAttr(), getLineNum(), global_type);
            } else {
                return errorWithReturn(getLineNum(), "must be array type");
            }
            printf("[");
            scanWithErrorJudge();
            if (parseExpression() == ERROR) { return ERROR; }
            if (token != TRSQPAREN) {
                return errorWithReturn(getLineNum(), "']' is not found");
            }
            printf("]");
            scanWithErrorJudge();
        }
        if (!is_array) {
            if (local_type != 0 && isStandardType(local_type)) {
                is_use_local = TRUE;
                updateExIDRefLine(name, line, local_type);
            } else if (global_type != 0 && isStandardType(global_type)) {
                is_use_local = FALSE;
                updateExIDRefLine(name, line, global_type);
            } else {
                return errorWithReturn(getLineNum(), "must be standard type");
            }
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
        if (token != TBEGIN) {
            return errorWithReturn(getLineNum(), "'begin' is not found");
        }
        can_call = TRUE;
        if (parseCompoundState() == ERROR) { return ERROR; }
        can_call = FALSE;

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
    can_call = FALSE;
    initGlobalID();
    initLocalID();

    if (token != TPROGRAM) {
        return errorWithReturn(getLineNum(), "'program' is not found");
    }
    printWithTub("program", 0, TRUE);
    scanWithErrorJudge();

    if (token != TNAME) {
        return errorWithReturn(getLineNum(), "'program name' is not found");
    }
    printWithTub(getStrAttr(), 0, FALSE);
    scanWithErrorJudge();

    if (token != TSEMI) {
        return errorWithReturn(getLineNum(), "';' is not found");
    }
    printWithTub(";\n", 0, FALSE);
    scanWithErrorJudge();

    tab_num++;
    if (parseBlock() == ERROR) { return ERROR; }

    if (token != TDOT) {
        return errorWithReturn(getLineNum(), "'.' is not found");
    }
    printf(".\n");

    if (cnt_iteration > 0 && cnt_break == 0) {
        // can't know line number -> can't return errorWithReturn()
        fprintf(stderr, "[ERROR] : 'break' must be included at least one in iteration statement\n");
        return ERROR;
    }
    debugExIDTable();
    return OK;
}