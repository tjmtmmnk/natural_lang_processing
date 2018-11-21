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

static int parseConst();

static int parseTerm();

static int parseSimpleExpression();

static int parseRelateOperator();

static int parseExpression();

static int parseFactor();

static int parseSubProgramDecler();

static int parseConditionState();

static int parseIterationState();

static int parseCompoundState();

static int parseExitState();

static int parseCallState();

static int parseReturnState();

static int parseExpressions();

static int parseAssignState();

static int parseInputState();

static int parseOutputFormat();

static int parseOutputState();

static int parseState();

static int parseVariable();

static int parseMultiOperator();

static int parseAddOperator();

static int parseEmptyState();

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

    if (parseStandardType() == ERROR) {
        return ERROR;
    }
    return OK;
}

static int parseType() {
    if (token == TINTEGER || token == TBOOLEAN || token == TCHAR || token == TARRAY) {
        if (token == TINTEGER || token == TBOOLEAN || token == TCHAR) {
            if (parseStandardType() == ERROR) {
                return ERROR;
            }
            if (token == TARRAY) {
                if (parseArrayType() == ERROR) {
                    return ERROR;
                }
            }
        }
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
    if (parseName() == ERROR) {
        return ERROR;
    }
    while (token == TCOMMA) {
        token = scanTokenOneEach();

        if (parseName() == ERROR) {
            return ERROR;
        }
    }
    return OK;
}

static int parseVarDecler() {
    if (token != TVAR) {
        return errorWithReturn(getLineNum(), "'var' is not found");
    }
    token = scanTokenOneEach();

    if (parseVarNames() == ERROR) {
        return ERROR;
    }

    if (token != TCOLON) {
        return errorWithReturn(getLineNum(), "':' is not found");
    }
    token = scanTokenOneEach();

    if (parseType() == ERROR) {
        return ERROR;
    }

    if (token != TSEMI) {
        return errorWithReturn(getLineNum(), "';' is not found");
    }
    token = scanTokenOneEach();

    while (token == TNAME) {
        if (parseVarNames() == ERROR) {
            return ERROR;
        }

        if (token != TCOLON) {
            return errorWithReturn(getLineNum(), "':' is not found");
        }
        token = scanTokenOneEach();

        if (parseType() == ERROR) {
            return ERROR;
        }

        if (token != TSEMI) {
            return errorWithReturn(getLineNum(), "';' is not found");
        }
        token = scanTokenOneEach();
    }

    if (token != TBEGIN) {
        return errorWithReturn(getLineNum(), "invalid word after ';'");
    }
    return OK;
}

static int parseFormalParam() {
    if (token != TLPAREN) {
        return errorWithReturn(getLineNum(), "'(' is not found");
    }
    token = scanTokenOneEach();

    if (parseVarNames() == ERROR) {
        return ERROR;
    }

    if (token != TCOLON) {
        return errorWithReturn(getLineNum(), "':' is not found");
    }
    token = scanTokenOneEach();

    if (parseType() == ERROR) {
        return ERROR;
    }

    while (token == TSEMI) {
        token = scanTokenOneEach();

        if (parseVarNames() == ERROR) {
            return ERROR;
        }

        if (token != TCOLON) {
            return errorWithReturn(getLineNum(), "':' is not found");
        }
        token = scanTokenOneEach();

        if (parseType() == ERROR) {
            return ERROR;
        }
    }

    if (token != TRPAREN) {
        return errorWithReturn(getLineNum(), "')' is not found");
    }
    token = scanTokenOneEach();
    return OK;
}

static int parseConst() {
    if (token == TNUMBER || token == TFALSE || token == TTRUE || token == TSTRING) {
        token = scanTokenOneEach();
    } else {
        return errorWithReturn(getLineNum(), "'constant' is not found");
    }
    return OK;
}

static int parseTerm() {
    if (parseFactor() == ERROR) {
        return ERROR;
    }
    while (token == TSTAR || token == TDIV || token == TAND) {
        if (parseMultiOperator() == ERROR) {
            return ERROR;
        }
        if (parseFactor() == ERROR) {
            return ERROR;
        }
    }
    return OK;
}

static int parseSimpleExpression() {
    if (token == TPLUS || token == TMINUS) {
        token = scanTokenOneEach();
    }

    if (parseTerm() == ERROR) {
        return ERROR;
    }

    while (token == TPLUS || token == TMINUS || token == TOR) {
        if (parseAddOperator() == ERROR) {
            return ERROR;
        }
        if (parseTerm() == ERROR) {
            return ERROR;
        }
    }
    return OK;
}

static int parseRelateOperator() {
    if (token == TEQUAL || token == TNOTEQ || token == TLE || token == TLEEQ || token == TGR || token == TGREQ) {
        token = scanTokenOneEach();
    } else {
        return errorWithReturn(getLineNum(), "'relational operator' is not found");
    }
    return OK;
}

static int parseExpression() {
    if (parseSimpleExpression() == ERROR) {
        return ERROR;
    }
    while (token == TEQUAL || token == TNOTEQ || token == TLE || token == TLEEQ || token == TGR || token == TGREQ) {
        if (parseRelateOperator() == ERROR) {
            return ERROR;
        }
        if (parseSimpleExpression() == ERROR) {
            return ERROR;
        }
    }
    return OK;
}

static int parseFactor() {
    switch (token) {
        case TNAME: {
            if (parseVariable() == ERROR) {
                return ERROR;
            }
            break;
        }
        case TNUMBER:
        case TFALSE:
        case TTRUE:
        case TSTRING: {
            if (parseConst() == ERROR) {
                return ERROR;
            }
            break;
        }
        case TLPAREN: {
            token = scanTokenOneEach();
            if (parseExpression() == ERROR) {
                return ERROR;
            }
            if (token != TRPAREN) {
                return errorWithReturn(getLineNum(), "')' is not found");
            }
            token = scanTokenOneEach();
            break;
        }
        case TNOT: {
            token = scanTokenOneEach();
            if (parseFactor() == ERROR) {
                return ERROR;
            }
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
            if (parseExpression() == ERROR) {
                return ERROR;
            }
            if (token != TRPAREN) {
                return errorWithReturn(getLineNum(), "')' is not found");
            }
            break;
        }
    }
    return OK;
}

static int parseConditionState() {
    if (token != TIF) {
        return errorWithReturn(getLineNum(), "'if' is not found");
    }
    token = scanTokenOneEach();

    if (parseExpression() == ERROR) {
        return ERROR;
    }

    if (token != TTHEN) {
        return errorWithReturn(getLineNum(), "'then' is not found");
    }
    token = scanTokenOneEach();

    if (parseState() == ERROR) {
        return ERROR;
    }

    if (token == TELSE) {
        token = scanTokenOneEach();

        if (parseState() == ERROR) {
            return ERROR;
        }
    }
    return OK;
}


static int parseSubProgramDecler() {
    if (token == TPROCEDURE) {
        token = scanTokenOneEach();
    } else {
        return errorWithReturn(getLineNum(), "'procedure' is not found");
    }
    if (parseName() == ERROR) {
        return ERROR;
    }

    if (parseFormalParam() == ERROR) {
        return ERROR;
    }

    if (token != TSEMI) {
        return errorWithReturn(getLineNum(), "';' is not found");
    }
    token = scanTokenOneEach();

    if (parseVarDecler() == ERROR) {
        return ERROR;
    }

    if (parseCompoundState() == ERROR) {
        return ERROR;
    }

    if (token != TSEMI) {
        return errorWithReturn(getLineNum(), "';' is not found");
    }
    token = scanTokenOneEach();
    return OK;
}

static int parseIterationState() {
    if (token != TWHILE) {
        return errorWithReturn(getLineNum(), "'while' is not found");
    }
    token = scanTokenOneEach();

    if (parseExpression() == ERROR) {
        return ERROR;
    }

    if (token != TDO) {
        return errorWithReturn(getLineNum(), "'do' is not found");
    }
    token = scanTokenOneEach();

    if (parseState() == ERROR) {
        return ERROR;
    }
    return OK;
}

static int parseExitState() {
    if (token != TBREAK) {
        return errorWithReturn(getLineNum(), "'break' is not found");
    }
    token = scanTokenOneEach();
    return OK;
}

static int parseCallState() {
    if (token != TCALL) {
        return errorWithReturn(getLineNum(), "'call' is not found");
    }

    if (parseName() == ERROR) {
        return ERROR;
    }

    if (token == TLPAREN) {
        token = scanTokenOneEach();
        if (parseExpressions() == ERROR) {
            return ERROR;
        }
        if (token != TRPAREN) {
            return errorWithReturn(getLineNum(), "')' is not found");
        }
        token = scanTokenOneEach();
    }
    return OK;
}

static int parseReturnState() {
    if (token != TRETURN) {
        errorWithReturn(getLineNum(), "'return' is not found");
    }
    token = scanTokenOneEach();
    return OK;
}

static int parseExpressions() {
    if (parseExpression() == ERROR) {
        return ERROR;
    }
    while (token == TCOMMA) {
        token = scanTokenOneEach();
        if (parseExpression() == ERROR) {
            return ERROR;
        }
    }
    return OK;
}

static int parseAssignState() {
    if (parseVariable() == ERROR) {
        return ERROR;
    }
    if (token != TASSIGN) {
        return errorWithReturn(getLineNum(), "':=' is not found");
    }
    token = scanTokenOneEach();

    if (parseExpression() == ERROR) {
        return ERROR;
    }
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
        if (parseVariable() == ERROR) {
            return ERROR;
        }
        while (token == TCOMMA) {
            token = scanTokenOneEach();
            if (parseVariable() == ERROR) {
                return ERROR;
            }
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
            if (parseExpression() == ERROR) {
                return ERROR;
            }
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
        if (parseOutputFormat() == ERROR) {
            return ERROR;
        }
        while (token == TCOMMA) {
            token = scanTokenOneEach();
            if (parseOutputFormat() == ERROR) {
                return ERROR;
            }
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
            if (parseAssignState() == ERROR) {
                return ERROR;
            }
            break;
        }
        case TIF: {
            if (parseConditionState() == ERROR) {
                return ERROR;
            }
            break;
        }
        case TWHILE: {
            if (parseIterationState() == ERROR) {
                return ERROR;
            }
            break;
        }
        case TBREAK: {
            if (parseExitState() == ERROR) {
                return ERROR;
            }
            break;
        }
        case TCALL: {
            if (parseCallState() == ERROR) {
                return ERROR;
            }
            break;
        }
        case TREAD:
        case TREADLN: {
            if (parseInputState() == ERROR) {
                return ERROR;
            }
            break;
        }
        case TWRITE:
        case TWRITELN: {
            if (parseOutputState() == ERROR) {
                return ERROR;
            }
            break;
        }
        case TBEGIN: {
            if (parseCompoundState() == ERROR) {
                return ERROR;
            }
            break;
        }
        default:
            if (parseEmptyState() == ERROR) {
                return ERROR;
            }
            break;
    }
    return OK;
}

static int parseCompoundState() {
    if (token != TBEGIN) {
        return errorWithReturn(getLineNum(), "'begin' is not found");
    }
    token = scanTokenOneEach();

    if (parseState() == ERROR) {
        return ERROR;
    }

    while (token == TSEMI) {
        token = scanTokenOneEach();
        if (parseState() == ERROR) {
            return ERROR;
        }
    }
    if (token != TEND) {
        return errorWithReturn(getLineNum(), "'end' is not found");
    }
    token = scanTokenOneEach();
    return OK;
}

// left partと同義
static int parseVariable() {
    if (parseName() == ERROR) {
        return ERROR;
    }
    if (token == TLSQPAREN) {
        token = scanTokenOneEach();
        if (parseExpression() == ERROR) {
            return ERROR;
        }
        if (token != TRSQPAREN) {
            return errorWithReturn(getLineNum(), "']' is not found");
        }
        token = scanTokenOneEach();
    }
    return OK;
}

static int parseMultiOperator() {
    if (token != TSTAR || token != TDIV || token != TAND) {
        return errorWithReturn(getLineNum(), "'multi operator' is not found");
    }
    token = scanTokenOneEach();
    return OK;
}

static int parseAddOperator() {
    if (token == TPLUS || token == TMINUS || token == TOR) {
        token = scanTokenOneEach();
    } else {
        return errorWithReturn(getLineNum(), "'add operator' is not found");
    }

    return OK;
}

static int parseEmptyState() {
    token = scanTokenOneEach();
    return OK;
}

static int parseBlock() {
    while (token == TVAR || token == TPROCEDURE) {
        if (token == TVAR) {
            if (parseVarDecler() == ERROR) {
                return ERROR;
            }
        }
        if (token == TPROCEDURE) {
            if (parseSubProgramDecler() == ERROR) {
                return ERROR;
            }
        }
    }
    if (parseCompoundState() == ERROR) {
        return ERROR;
    }
    return OK;
}

/****************** 外部関数 *********************/
void initSyntaticAnalysis(int _token) {
    token = _token;
}

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

    if (parseBlock() == ERROR) {
        return ERROR;
    }

    if (token != TDOT) {
        return errorWithReturn(getLineNum(), "'.' is not found");
    }
    token = scanTokenOneEach();
    return OK;
}