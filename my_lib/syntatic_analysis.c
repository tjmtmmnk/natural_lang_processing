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
    if (token != TARRAY) {
        return errorWithReturn(getLineNum(), "'array' is not found");
    }
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

static int parseType() {
    if (token == TINTEGER || token == TBOOLEAN || token == TCHAR || token == TARRAY) {
        if (token == TINTEGER || token == TBOOLEAN || token == TCHAR) {
            if (parseStandardType() == ERROR) { return ERROR; }

            if (token == TARRAY) {
                if (parseArrayType() == ERROR) { return ERROR; }
            }
        }
    } else {
        return errorWithReturn(getLineNum(), "type error");
    }
    return OK;
}

static int parseName() {
    if (token != TNAME) {
        return errorWithReturn(getLineNum(), "'NAME' is not found");
    }
    token = scanTokenOneEach();
    return OK;
}

static int parseVarNames() {
    if (parseName() == ERROR) { return ERROR; }

    while (token == TCOMMA) {
        token = scanTokenOneEach();

        if (parseName() == ERROR) { return ERROR; }
    }
    return OK;
}

static int parseVarDecler() {
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

    if (token != TBEGIN) { // for error (don't matter the action)
        return errorWithReturn(getLineNum(), "invalid word after ';'");
    }
    return OK;
}

static int parseFormalParam() {
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

static int parseTerm() {
    if (parseFactor() == ERROR) { return ERROR; }

    while (token == TSTAR || token == TDIV || token == TAND) {
        token = scanTokenOneEach(); //multi operator

        if (parseFactor() == ERROR) { return ERROR; }
    }
    return OK;
}

static int parseSimpleExpression() {
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

static int parseExpression() {
    if (parseSimpleExpression() == ERROR) { return ERROR; }

    while (token == TEQUAL || token == TNOTEQ || token == TLE || token == TLEEQ || token == TGR || token == TGREQ) {
        token = scanTokenOneEach(); //relational operator

        if (parseSimpleExpression() == ERROR) { return ERROR; }
    }
    return OK;
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

    if (token != TBEGIN) {
        return errorWithReturn(getLineNum(), "'begin' is not found");
    }
    if (parseCompoundState() == ERROR) { return ERROR; }

    if (token != TSEMI) {
        return errorWithReturn(getLineNum(), "';' is not found");
    }
    token = scanTokenOneEach();
    return OK;
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

static int parseExpressions() {
    if (parseExpression() == ERROR) { return ERROR; }

    while (token == TCOMMA) {
        token = scanTokenOneEach();
        if (parseExpression() == ERROR) { return ERROR; }
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
    if (token == TREAD || token == TREADLN) {
        token = scanTokenOneEach();
    } else {
        return errorWithReturn(getLineNum(), "'input state' is not found");
    }
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
            }
        }
    }
    return OK;
}

static int parseOutputState() {
    if (token == TWRITE || token == TWRITELN) {
        token = scanTokenOneEach();
    } else {
        return errorWithReturn(getLineNum(), "'output state' is not found");
    }
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

// equal to "left part"
static int parseVariable() {
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

//ok
static int parseBlock() {
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