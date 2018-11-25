#include "syntatic_analysis.h"
#include "lexical_analysis.h"

#define TRUE 1
#define FALSE 0

static int token;

static int tab_num;

static int parseStandardType();

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

static void printWithTub(char *str, int tab_num, int exist_space);

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

static int parseStandardType() {
    if (token == TINTEGER || token == TBOOLEAN || token == TCHAR) {
        printf("%s", token_str[token]);
        token = scanTokenOneEach();
        return OK;
    }
    return errorWithReturn(getLineNum(), "'standard type' is not found");
}

static int parseArrayType() {
    if (token == TARRAY) {
        printf("array");
        token = scanTokenOneEach();

        if (token != TLSQPAREN) {
            return errorWithReturn(getLineNum(), "'[' is not found");
        }
        printf("[");
        token = scanTokenOneEach();

        if (token != TNUMBER) {
            return errorWithReturn(getLineNum(), "'NUMBER' is not found");
        }
        printf("%s", getStrAttr());
        token = scanTokenOneEach();

        if (token != TRSQPAREN) {
            return errorWithReturn(getLineNum(), "']' is not found");
        }
        printf("] ");
        token = scanTokenOneEach();

        if (token != TOF) {
            return errorWithReturn(getLineNum(), "'of' is not found");
        }
        printf("of ");
        token = scanTokenOneEach();

        if (parseStandardType() == ERROR) { return ERROR; }
        return OK;
    }
    return errorWithReturn(getLineNum(), "'array' is not found");
}

static int parseType() {
    if (token == TINTEGER || token == TBOOLEAN || token == TCHAR || token == TARRAY) {
        if (token == TINTEGER || token == TBOOLEAN || token == TCHAR) {
            if (parseStandardType() == ERROR) { return ERROR; }
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
        token = scanTokenOneEach();
        return OK;
    }
    return errorWithReturn(getLineNum(), "'NAME' is not found");
}

static int parseVarNames() {
    if (token == TNAME) {
        tab_num++;
        printWithTub("", tab_num, FALSE);
        if (parseName() == ERROR) { return ERROR; }


        while (token == TCOMMA) {
            printf(", ");
            token = scanTokenOneEach();

            if (parseName() == ERROR) { return ERROR; }
        }
        tab_num--;
        return OK;
    }
    return errorWithReturn(getLineNum(), "'var name' is not found");
}

static int parseVarDecler() {
    if (token == TVAR) {
        printWithTub("var\n", tab_num, FALSE);
        token = scanTokenOneEach();

        if (parseVarNames() == ERROR) { return ERROR; }
        printf(" ");

        if (token != TCOLON) {
            return errorWithReturn(getLineNum(), "':' is not found");
        }
        printf(": ");
        token = scanTokenOneEach();

        if (parseType() == ERROR) { return ERROR; }

        if (token != TSEMI) {
            return errorWithReturn(getLineNum(), "';' is not found");
        }
        printf(";\n");
        token = scanTokenOneEach();

        while (token == TNAME) {
            if (parseVarNames() == ERROR) { return ERROR; }

            if (token != TCOLON) {
                return errorWithReturn(getLineNum(), "':' is not found");
            }
            printf(": ");
            token = scanTokenOneEach();

            if (parseType() == ERROR) { return ERROR; }

            if (token != TSEMI) {
                return errorWithReturn(getLineNum(), "';' is not found");
            }
            printf(";\n");
            token = scanTokenOneEach();
        }

        return OK;
    }
    return errorWithReturn(getLineNum(), "'var' is not found");
}

static int parseFormalParam() {
    if (token == TLPAREN) {
        token = scanTokenOneEach();
        printf("(");

        if (parseVarNames() == ERROR) { return ERROR; }
        printf(" ");

        if (token != TCOLON) {
            return errorWithReturn(getLineNum(), "':' is not found");
        }
        printf(": ");
        token = scanTokenOneEach();

        if (parseType() == ERROR) { return ERROR; }

        while (token == TSEMI) {
            token = scanTokenOneEach();
            printf("; ");

            if (parseVarNames() == ERROR) { return ERROR; }

            if (token != TCOLON) {
                return errorWithReturn(getLineNum(), "':' is not found");
            }
            printf(" : ");
            token = scanTokenOneEach();

            if (parseType() == ERROR) { return ERROR; }
        }

        if (token != TRPAREN) {
            return errorWithReturn(getLineNum(), "')' is not found");
        }
        printf(")");
        token = scanTokenOneEach();
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
            token = scanTokenOneEach(); //multi operator

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
            token = scanTokenOneEach();
            break;
        }
        case TNUMBER:
        case TSTRING: {
            printWithTub(getStrAttr(), 0, FALSE);
            token = scanTokenOneEach();
            break;
        }
        case TLPAREN: {
            token = scanTokenOneEach();

            if (parseExpression() == ERROR) { return ERROR; }

            if (token != TRPAREN) {
                return errorWithReturn(getLineNum(), "')' is not found");
            }
            printf(")");
            token = scanTokenOneEach();
            break;
        }
        case TNOT: {
            token = scanTokenOneEach();
            if (parseFactor() == ERROR) { return ERROR; }
            break;
        }
        case TINTEGER:
        case TBOOLEAN:
        case TCHAR: {
            token = scanTokenOneEach();

            if (token != TLPAREN) {
                return errorWithReturn(getLineNum(), "'(' is not found");
            }
            token = scanTokenOneEach();

            if (parseExpression() == ERROR) { return ERROR; }

            if (token != TRPAREN) {
                return errorWithReturn(getLineNum(), "')' is not found");
            }
            token = scanTokenOneEach();
            break;
        }
        default:
            return errorWithReturn(getLineNum(), "factor error");
    }
    return OK;
}

static int parseConditionState() {
    printf(" ");
    token = scanTokenOneEach();

    if (parseExpression() == ERROR) { return ERROR; }

    if (token != TTHEN) {
        return errorWithReturn(getLineNum(), "'then' is not found");
    }
    printf(" then\n");
    token = scanTokenOneEach();
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
        token = scanTokenOneEach();
        if (parseState() == ERROR) { return ERROR; }
        tab_num--;
    }
    return OK;
}

static int parseSubProgramDecler() {
    if (token == TPROCEDURE) {
        printWithTub("procedure", tab_num, TRUE);
        token = scanTokenOneEach();

        if (parseName() == ERROR) { return ERROR; }

        if (token == TLPAREN) {
            if (parseFormalParam() == ERROR) { return ERROR; }
        }

        if (token != TSEMI) {
            return errorWithReturn(getLineNum(), "';' is not found");
        }
        printf(";\n");
        token = scanTokenOneEach();

        if (token == TVAR) {
            if (parseVarDecler() == ERROR) { return ERROR; }
        }

        if (parseCompoundState() == ERROR) { return ERROR; }

        if (token != TSEMI) {
            return errorWithReturn(getLineNum(), "';' is not found");
        }
        printf(";\n");

        token = scanTokenOneEach();
        return OK;
    }
    return errorWithReturn(getLineNum(), "'procedure' is not found");
}

static int parseSimpleExpression() {
    if (token == TNAME || token == TNUMBER || token == TFALSE || token == TTRUE || token == TSTRING ||
        token == TLPAREN || token == TNOT || token == TINTEGER || token == TBOOLEAN || token == TCHAR ||
        token == TPLUS || token == TMINUS || token == TSTRING) {
        if (token == TPLUS || token == TMINUS) {
            printf(" ");
            printWithTub(token_str[token], 0, FALSE);
            token = scanTokenOneEach();
        }

        if (parseTerm() == ERROR) { return ERROR; }

        while (token == TPLUS || token == TMINUS || token == TOR) {
            printf(" ");
            printWithTub(token_str[token], 0, TRUE);
            token = scanTokenOneEach(); //add operator

            if (parseTerm() == ERROR) { return ERROR; }
        }
        return OK;
    }
    return errorWithReturn(getLineNum(), "simple expression error");
}

static int parseExpression() {
    if (token == TNAME || token == TNUMBER || token == TFALSE || token == TTRUE || token == TSTRING ||
        token == TLPAREN || token == TNOT || token == TINTEGER || token == TBOOLEAN || token == TCHAR ||
        token == TPLUS || token == TMINUS || token == TSTRING) {
        if (parseSimpleExpression() == ERROR) { return ERROR; }

        while (token == TEQUAL || token == TNOTEQ || token == TLE || token == TLEEQ || token == TGR || token == TGREQ) {
            printf(" ");
            printWithTub(token_str[token], 0, TRUE);
            token = scanTokenOneEach(); //relational operator
            if (parseSimpleExpression() == ERROR) { return ERROR; }
        }
        return OK;
    }
    return errorWithReturn(getLineNum(), "expression error");
}

static int parseExpressions() {
    if (token == TNAME || token == TNUMBER || token == TFALSE || token == TTRUE || token == TSTRING ||
        token == TLPAREN || token == TNOT || token == TINTEGER || token == TBOOLEAN || token == TCHAR ||
        token == TPLUS || token == TMINUS || token == TSTRING) {
        if (parseExpression() == ERROR) { return ERROR; }

        while (token == TCOMMA) {
            printf(", ");
            token = scanTokenOneEach();
            if (parseExpression() == ERROR) { return ERROR; }
        }
        return OK;
    }
    return errorWithReturn(getLineNum(), "expressions error");
}

static int parseIterationState() {
    token = scanTokenOneEach();

    printf(" ");
    if (parseExpression() == ERROR) { return ERROR; }
    printf(" ");

    if (token != TDO) {
        return errorWithReturn(getLineNum(), "'do' is not found");
    }
    printWithTub("do\n", 0, FALSE);
    token = scanTokenOneEach();

    if (parseState() == ERROR) { return ERROR; }
    return OK;
}

static int parseCallState() {
    printf(" ");
    token = scanTokenOneEach();

    if (parseName() == ERROR) { return ERROR; }

    if (token == TLPAREN) {
        printf("(");
        token = scanTokenOneEach();

        if (parseExpressions() == ERROR) { return ERROR; }

        if (token != TRPAREN) {
            return errorWithReturn(getLineNum(), "')' is not found");
        }
        printf(")");
        token = scanTokenOneEach();
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
    token = scanTokenOneEach();

    if (parseExpression() == ERROR) { return ERROR; }
    return OK;
}

static int parseInputState() {
    token = scanTokenOneEach();

    if (token == TLPAREN) {
        printWithTub("(", 0, FALSE);
        token = scanTokenOneEach();

        if (parseVariable() == ERROR) { return ERROR; }

        while (token == TCOMMA) {
            printWithTub(",", 0, TRUE);
            token = scanTokenOneEach();
            if (parseVariable() == ERROR) { return ERROR; }
        }
        if (token != TRPAREN) {
            return errorWithReturn(getLineNum(), "')' is not found");
        }
        printWithTub(")", 0, FALSE);
        token = scanTokenOneEach();
    }
    return OK;
}

static int parseOutputState() {
    token = scanTokenOneEach();

    if (token == TLPAREN) {
        printWithTub("(", 0, FALSE);
        token = scanTokenOneEach();

        if (parseOutputFormat() == ERROR) { return ERROR; }

        while (token == TCOMMA) {
            printWithTub(",", 0, TRUE);
            token = scanTokenOneEach();
            if (parseOutputFormat() == ERROR) { return ERROR; }
        }
        if (token != TRPAREN) {
            return errorWithReturn(getLineNum(), "')' is not found");
        }
        printWithTub(")", 0, FALSE);
        token = scanTokenOneEach();
    }
    return OK;
}

static int parseOutputFormat() {
    if (token == TNAME || token == TNUMBER || token == TFALSE || token == TTRUE || token == TSTRING ||
        token == TLPAREN || token == TNOT || token == TINTEGER || token == TBOOLEAN || token == TCHAR ||
        token == TPLUS || token == TMINUS || token == TSTRING) {
        if (token == TSTRING) {
            printWithTub("'", 0, FALSE);
            printWithTub(getStrAttr(), 0, FALSE);
            printWithTub("'", 0, FALSE);
            token = scanTokenOneEach();
        } else {
            if (parseExpression() == ERROR) { return ERROR; }

            if (token == TCOLON) {
                token = scanTokenOneEach();
                if (token != TNUMBER) {
                    return errorWithReturn(getLineNum(), "'number' is not found");
                }
                token = scanTokenOneEach();
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
            if (parseIterationState() == ERROR) { return ERROR; }
            break;
        }
        case TBREAK: {
            token = scanTokenOneEach(); //parse break
            break;
        }
        case TCALL: {
            if (parseCallState() == ERROR) { return ERROR; }
            break;
        }
        case TRETURN:
            token = scanTokenOneEach();
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
            token = scanTokenOneEach(); //empty state
            break;
    }
    return OK;
}

static int parseCompoundState() {
    if (token == TBEGIN) {
        printWithTub("begin\n", tab_num, FALSE);
        token = scanTokenOneEach();
        tab_num++;

        if (parseState() == ERROR) { return ERROR; }

        while (token == TSEMI) {
            printf(";\n");
            token = scanTokenOneEach();
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
        token = scanTokenOneEach();
        return OK;
    }
    return errorWithReturn(getLineNum(), "'begin' is not found");
}

// equal to "left part"
static int parseVariable() {
    if (token == TNAME) {
        if (parseName() == ERROR) { return ERROR; }

        if (token == TLSQPAREN) {
            token = scanTokenOneEach();
            if (parseExpression() == ERROR) { return ERROR; }
            if (token != TRSQPAREN) {
                return errorWithReturn(getLineNum(), "']' is not found");
            }
            token = scanTokenOneEach();
        }
        return OK;
    }
    return errorWithReturn(getLineNum(), "undefineded the variable name");
}

static int parseBlock() {
    if (token == TVAR || token == TPROCEDURE || token == TBEGIN) {
        while (token == TVAR || token == TPROCEDURE) {
            if (token == TVAR) {
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
    token = scanTokenOneEach();
    tab_num = 0;

    if (token != TPROGRAM) {
        return errorWithReturn(getLineNum(), "'program' is not found");
    }
    printWithTub("program", 0, TRUE);
    token = scanTokenOneEach();

    if (token != TNAME) {
        return errorWithReturn(getLineNum(), "'program name' is not found");
    }
    printWithTub(getStrAttr(), 0, FALSE);
    token = scanTokenOneEach();

    if (token != TSEMI) {
        return errorWithReturn(getLineNum(), "';' is not found");
    }
    printWithTub(";\n", 0, FALSE);
    token = scanTokenOneEach();

    tab_num++;
    if (parseBlock() == ERROR) { return ERROR; }

    if (token != TDOT) {
        return errorWithReturn(getLineNum(), "'.' is not found");
    }
    printf(".\n");
    return OK;
}