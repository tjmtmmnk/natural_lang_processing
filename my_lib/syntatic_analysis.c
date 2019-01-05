#include "syntatic_analysis.h"
#include "lexical_analysis.h"
#include "cross_reference.h"

static eScope scope;
static int token;
static int tab_num;
static int cnt_iteration, cnt_break;
static int while_nest;
static int parseStandardType(int is_array, int has_set_type);
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
        updateExIDType(scope, token, is_array, size);
        printf("%s", token_str[token]);
        scanWithErrorJudge();
        return OK;
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

        if (parseStandardType(TRUE, size) == ERROR) { return ERROR; }
        return OK;
    }
    return errorWithReturn(getLineNum(), "'array' is not found");
}

static int parseType() {
    if (token == TINTEGER || token == TBOOLEAN || token == TCHAR || token == TARRAY) {
        if (token == TINTEGER || token == TBOOLEAN || token == TCHAR) {
            if (parseStandardType(FALSE, 0) == ERROR) { return ERROR; }
        }

        if (token == TARRAY) {
            if (parseArrayType() == ERROR) { return ERROR; }
        }
        return OK;
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
        registerExID(scope, getStrAttr(), getLineNum(), FALSE);
        if (parseName() == ERROR) { return ERROR; }

        while (token == TCOMMA) {
            printf(", ");
            scanWithErrorJudge();
            registerExID(scope, getStrAttr(), getLineNum(), FALSE);
            if (parseName() == ERROR) { return ERROR; }
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
        scope = LOCAL;
        scanWithErrorJudge();
        printf("(");

        if (parseVarNames() == ERROR) { return ERROR; }
        printf(" ");

        if (token != TCOLON) {
            return errorWithReturn(getLineNum(), "':' is not found");
        }
        printf(": ");
        scanWithErrorJudge();

        if (parseType() == ERROR) { return ERROR; }

        while (token == TSEMI) {
            scanWithErrorJudge();
            printf("; ");

            if (parseVarNames() == ERROR) { return ERROR; }

            if (token != TCOLON) {
                return errorWithReturn(getLineNum(), "':' is not found");
            }
            printf(" : ");
            scanWithErrorJudge();

            if (parseType() == ERROR) { return ERROR; }
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
        if (parseFactor() == ERROR) { return ERROR; }

        while (token == TSTAR || token == TDIV || token == TAND) {
            printf(" ");
            printWithTub(token_str[token], 0, TRUE);
            scanWithErrorJudge(); //multi operator

            if (parseFactor() == ERROR) { return ERROR; }
        }
        return OK;
    }
    return errorWithReturn(getLineNum(), "term error");
}

static int parseFactor() {
    if (token != TNAME && token != TNUMBER && token != TSTRING) {
        printWithTub(getStrAttr(), 0, FALSE);
    }
    switch (token) {
        case TNAME: {
            if (parseVariable() == ERROR) { return ERROR; }
            break;
        }
        case TFALSE:
        case TTRUE: {
            scanWithErrorJudge();
            return TPBOOL;
        }
        case TNUMBER: {
            printf("%s", getStrAttr());
            scanWithErrorJudge();
            break;
        }
        case TSTRING: {
            printf("'");
            printf("%s", getStrAttr());
            printf("'");
            scanWithErrorJudge();
            break;
        }
        case TLPAREN: {
            scanWithErrorJudge();

            if (parseExpression() == ERROR) { return ERROR; }

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
            if (parseFactor() != TPBOOL) { return ERROR; } // TODO : Implementation of error
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
    return OK;
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
        scope = GLOBAL;
        printWithTub("procedure", tab_num, TRUE);
        scanWithErrorJudge();

        setProcName(getStrAttr());
        registerExID(scope, getStrAttr(), getLineNum(), FALSE);
        if (parseName() == ERROR) { return ERROR; }

        if (token == TLPAREN) {
            scope = LOCAL;
            if (parseFormalParam() == ERROR) { return ERROR; }
        }

        if (token != TSEMI) {
            return errorWithReturn(getLineNum(), "';' is not found");
        }
        printf(";\n");
        updateExIDTypeProcedure();
        scanWithErrorJudge();

        if (token == TVAR) {
            scope = LOCAL;
            if (parseVarDecler() == ERROR) { return ERROR; }
        }

        if (parseCompoundState() == ERROR) { return ERROR; }

        if (token != TSEMI) {
            return errorWithReturn(getLineNum(), "';' is not found");
        }
        printf(";\n");

        scanWithErrorJudge();
//        initLocalID();
        return OK;
    }
    return errorWithReturn(getLineNum(), "'procedure' is not found");
}

static int parseSimpleExpression() {
    if (token == TNAME || token == TNUMBER || token == TFALSE || token == TTRUE || token == TSTRING ||
        token == TLPAREN || token == TNOT || token == TINTEGER || token == TBOOLEAN || token == TCHAR ||
        token == TPLUS || token == TMINUS) {
        if (token == TPLUS || token == TMINUS) {
            printf(" ");
            printWithTub(token_str[token], 0, FALSE);
            scanWithErrorJudge();
        }

        if (parseTerm() == ERROR) { return ERROR; }

        while (token == TPLUS || token == TMINUS || token == TOR) {
            printf(" ");
            printWithTub(token_str[token], 0, TRUE);
            scanWithErrorJudge(); //add operator

            if (parseTerm() == ERROR) { return ERROR; }
        }
        return OK;
    }
    return errorWithReturn(getLineNum(), "simple expression error");
}

static int parseExpression() {
    if (token == TNAME || token == TNUMBER || token == TFALSE || token == TTRUE || token == TSTRING ||
        token == TLPAREN || token == TNOT || token == TINTEGER || token == TBOOLEAN || token == TCHAR ||
        token == TPLUS || token == TMINUS) {
        if (parseSimpleExpression() == ERROR) { return ERROR; }

        while (token == TEQUAL || token == TNOTEQ || token == TLE || token == TLEEQ || token == TGR || token == TGREQ) {
            printf(" ");
            printWithTub(token_str[token], 0, TRUE);
            scanWithErrorJudge(); //relational operator
            if (parseSimpleExpression() == ERROR) { return ERROR; }
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
    scope = LOCAL;
    printf(" ");
    scanWithErrorJudge();
    setProcName("call");
    if (parseName() == ERROR) { return ERROR; }

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
    if (parseVariable() == ERROR) { return ERROR; }
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
        updateExIDRefLine(getStrAttr(), getLineNum());
        if (parseName() == ERROR) { return ERROR; }
        if (token == TLSQPAREN) {
            printf("[");
            scanWithErrorJudge();
            if (parseExpression() == ERROR) { return ERROR; }
            if (token != TRSQPAREN) {
                return errorWithReturn(getLineNum(), "']' is not found");
            }
            printf("]");
            scanWithErrorJudge();
        }
        return OK;
    }
    return errorWithReturn(getLineNum(), "undefineded the variable name");
}

static int parseBlock() {
    if (token == TVAR || token == TPROCEDURE || token == TBEGIN) {
        while (token == TVAR || token == TPROCEDURE) {
            if (token == TVAR) {
                scope = GLOBAL;
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
        if (parseCompoundState() == ERROR) { return ERROR; }

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
//    debug();
    return OK;
}