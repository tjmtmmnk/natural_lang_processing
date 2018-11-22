#include "syntatic_analysis.h"
#include "lexical_analysis.h"

static int token;

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

static int parseStandardType() {
    if (token == TINTEGER || token == TBOOLEAN || token == TCHAR) {
        token = scanTokenOneEach();
        return OK;
    }
    return errorWithReturn(getLineNum(), "'standard type' is not found");
}

static int parseArrayType() {
    if (token == TARRAY) {
        token = scanTokenOneEach();

        if (token != TLSQPAREN) {
            return errorWithReturn(getLineNum(), "'[' is not found");
        }
        token = scanTokenOneEach();

        if (token != TNUMBER) {
            return errorWithReturn(getLineNum(), "'NUMBER' is not found");
        }
        token = scanTokenOneEach();

        if (token != TRSQPAREN) {
            return errorWithReturn(getLineNum(), "']' is not found");
        }
        token = scanTokenOneEach();

        if (token != TOF) {
            return errorWithReturn(getLineNum(), "'of' is not found");
        }
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
        token = scanTokenOneEach();
        return OK;
    }
    return errorWithReturn(getLineNum(), "'NAME' is not found");
}

static int parseVarNames() {
    if (token == TNAME) {
        if (parseName() == ERROR) { return ERROR; }

        while (token == TCOMMA) {
            token = scanTokenOneEach();

            if (parseName() == ERROR) { return ERROR; }
        }
        return OK;
    }
    return errorWithReturn(getLineNum(), "'var name' is not found");
}

static int parseVarDecler() {
    if (token == TVAR) {
        token = scanTokenOneEach();

        if (token != TNAME) {
            return errorWithReturn(getLineNum(), "'NAME' is not found");
        }
        if (parseVarNames() == ERROR) { return ERROR; }

        if (token != TCOLON) {
            return errorWithReturn(getLineNum(), "':' is not found");
        }
        token = scanTokenOneEach();

        if (parseType() == ERROR) { return ERROR; }

        if (token != TSEMI) {
            return errorWithReturn(getLineNum(), "';' is not found");
        }
        token = scanTokenOneEach();

        while (token == TNAME) {
            if (parseVarNames() == ERROR) { return ERROR; }

            if (token != TCOLON) {
                return errorWithReturn(getLineNum(), "':' is not found");
            }
            token = scanTokenOneEach();

            if (parseType() == ERROR) { return ERROR; }

            if (token != TSEMI) {
                return errorWithReturn(getLineNum(), "';' is not found");
            }
            token = scanTokenOneEach();
        }

        return OK;
    }
    return errorWithReturn(getLineNum(), "'var' is not found");
}

static int parseFormalParam() {
    if (token == TLPAREN) {
        token = scanTokenOneEach();

        if (parseVarNames() == ERROR) { return ERROR; }

        if (token != TCOLON) {
            return errorWithReturn(getLineNum(), "':' is not found");
        }
        token = scanTokenOneEach();

        if (parseType() == ERROR) { return ERROR; }

        while (token == TSEMI) {
            token = scanTokenOneEach();

            if (parseVarNames() == ERROR) { return ERROR; }

            if (token != TCOLON) {
                return errorWithReturn(getLineNum(), "':' is not found");
            }
            token = scanTokenOneEach();

            if (parseType() == ERROR) { return ERROR; }
        }

        if (token != TRPAREN) {
            return errorWithReturn(getLineNum(), "')' is not found");
        }
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
            token = scanTokenOneEach(); //multi operator

            if (parseFactor() == ERROR) { return ERROR; }
        }
        return OK;
    }
    return errorWithReturn(getLineNum(), "term error");
}

static int parseFactor() {
    switch (token) {
        case TNAME: {
            if (parseVariable() == ERROR) { return ERROR; }
            break;
        }
        case TNUMBER:
        case TFALSE:
        case TTRUE:
        case TSTRING: {
            token = scanTokenOneEach();
            break;
        }
        case TLPAREN: {
            token = scanTokenOneEach();

            if (parseExpression() == ERROR) { return ERROR; }

            if (token != TRPAREN) {
                return errorWithReturn(getLineNum(), "')' is not found");
            }
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
    token = scanTokenOneEach();

    if (parseExpression() == ERROR) { return ERROR; }

    if (token != TTHEN) {
        return errorWithReturn(getLineNum(), "'then' is not found");
    }
    token = scanTokenOneEach();

    if (parseState() == ERROR) { return ERROR; }

    if (token == TELSE) {
        token = scanTokenOneEach();

        if (parseState() == ERROR) { return ERROR; }
    }
    return OK;
}

static int parseSubProgramDecler() {
    if (token == TPROCEDURE) {
        token = scanTokenOneEach();

        if (parseName() == ERROR) { return ERROR; }

        if (token == TLPAREN) {
            if (parseFormalParam() == ERROR) { return ERROR; }
        }

        if (token != TSEMI) {
            return errorWithReturn(getLineNum(), "';' is not found");
        }
        token = scanTokenOneEach();

        if (token == TVAR) {
            if (parseVarDecler() == ERROR) { return ERROR; }
        }

        if (parseCompoundState() == ERROR) { return ERROR; }

        if (token != TSEMI) {
            return errorWithReturn(getLineNum(), "';' is not found");
        }
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
            token = scanTokenOneEach();
        }

        if (parseTerm() == ERROR) { return ERROR; }

        while (token == TPLUS || token == TMINUS || token == TOR) {
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
            token = scanTokenOneEach();
            if (parseExpression() == ERROR) { return ERROR; }
        }
        return OK;
    }
    return errorWithReturn(getLineNum(), "expressions error");
}

static int parseIterationState() {
    token = scanTokenOneEach();

    if (parseExpression() == ERROR) { return ERROR; }

    if (token != TDO) {
        return errorWithReturn(getLineNum(), "'do' is not found");
    }
    token = scanTokenOneEach();

    if (parseState() == ERROR) { return ERROR; }
    return OK;
}

static int parseCallState() {
    token = scanTokenOneEach();

    if (parseName() == ERROR) { return ERROR; }

    if (token == TLPAREN) {
        token = scanTokenOneEach();

        if (parseExpressions() == ERROR) { return ERROR; }

        if (token != TRPAREN) {
            return errorWithReturn(getLineNum(), "')' is not found");
        }
        token = scanTokenOneEach();
    }
    return OK;
}

static int parseAssignState() {
    if (parseVariable() == ERROR) { return ERROR; }
    if (token != TASSIGN) {
        return errorWithReturn(getLineNum(), "':=' is not found");
    }
    token = scanTokenOneEach();

    if (parseExpression() == ERROR) { return ERROR; }
    return OK;
}

static int parseInputState() {
    token = scanTokenOneEach();

    if (token == TLPAREN) {
        token = scanTokenOneEach();

        if (parseVariable() == ERROR) { return ERROR; }

        while (token == TCOMMA) {
            token = scanTokenOneEach();
            if (parseVariable() == ERROR) { return ERROR; }
        }
        if (token != TRPAREN) {
            return errorWithReturn(getLineNum(), "')' is not found");
        }
        token = scanTokenOneEach();
    }
    return OK;
}

static int parseOutputState() {
    token = scanTokenOneEach();

    if (token == TLPAREN) {
        token = scanTokenOneEach();

        if (parseOutputFormat() == ERROR) { return ERROR; }

        while (token == TCOMMA) {
            token = scanTokenOneEach();
            if (parseOutputFormat() == ERROR) { return ERROR; }
        }
        if (token != TRPAREN) {
            return errorWithReturn(getLineNum(), "')' is not found");
        }
        token = scanTokenOneEach();
    }
    return OK;
}

static int parseOutputFormat() {
    if (token == TNAME || token == TNUMBER || token == TFALSE || token == TTRUE || token == TSTRING ||
        token == TLPAREN || token == TNOT || token == TINTEGER || token == TBOOLEAN || token == TCHAR ||
        token == TPLUS || token == TMINUS || token == TSTRING) {
        if (token == TSTRING) {
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
    switch (token) {
        case TNAME: {
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
        token = scanTokenOneEach();

        if (parseState() == ERROR) { return ERROR; }

        while (token == TSEMI) {
            token = scanTokenOneEach();
            if (parseState() == ERROR) { return ERROR; }
        }

        if (token != TEND) {
            return errorWithReturn(getLineNum(), "'end' is not found");
        }
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

    if (token != TPROGRAM) {
        return errorWithReturn(getLineNum(), "'program' is not found");
    }
    token = scanTokenOneEach();

    if (token != TNAME) {
        return errorWithReturn(getLineNum(), "'program name' is not found");
    }
    token = scanTokenOneEach();

    if (token != TSEMI) {
        return errorWithReturn(getLineNum(), "';' is not found");
    }
    token = scanTokenOneEach();

    if (parseBlock() == ERROR) { return ERROR; }

    if (token != TDOT) {
        return errorWithReturn(getLineNum(), "'.' is not found");
    }
    return OK;
}