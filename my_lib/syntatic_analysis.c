#include "syntatic_analysis.h"
#include "lexical_analysis.h"

static int token;

int parseCompoundState() {

}


int parseStandardType() {
    if (token != TINTEGER || token != TBOOLEAN || token != TCHAR) {
        return errorWithReturn(getLineNum(), "'standart type' is not found");
    }
    token = scanTokenOneEach();
}

int parseArrayType() {
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
}

int parseType() {
    if (parseStandardType() == ERROR || parseArrayType() == ERROR) {
        return ERROR;
    }
}

int parseName() {
    if (token != TNAME) {
        return errorWithReturn(getLineNum(), "'NAME' is not found");
    }
    token = scanTokenOneEach();
}

int parseVarNames() {
    if (parseName() == ERROR) {
        return ERROR;
    }
    while (token == TCOMMA) {
        token = scanTokenOneEach();

        if (parseName() == ERROR) {
            return ERROR;
        }
    }
}

int parseVarDecler() {
    if (token != TVAR) {
        return errorWithReturn(getLineNum(), "'var' is not found");
    }
    token = scanTokenOneEach();

    if (parseVarNames() == ERROR) {
        return ERROR;
    }
    token = scanTokenOneEach();

    if (token != TCOLON) {
        return errorWithReturn(getLineNum(), "':' is not found");
    }
    token = scanTokenOneEach();

    if (parseType() == ERROR) {
        return ERROR;
    }
    token = scanTokenOneEach();

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
        token = scanTokenOneEach();

        if (token != TSEMI) {
            return errorWithReturn(getLineNum(), "';' is not found");
        }
        token = scanTokenOneEach();
    }
}

int parseFormalParam() {
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

    while (token == ";") {
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
}

int parseSubProgramDecler() {
    if (token != TPROCEDURE) {
        return errorWithReturn(getLineNum(), "'procedure' is not found");
    }
    token = scanTokenOneEach();

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
}

int parseExpression() {
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
}

int parseRelateOperator() {
    if (token != TEQUAL || token != TNOTEQ || token != TLE || token != TLEEQ || token != TGR || token != TGREQ) {
        return errorWithReturn(getLineNum(), "'relational operator' is not found");
    }
    token = scanTokenOneEach();
}

int parseConditionState() {
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
}

int parseIterationState() {
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
}

int parseExitState() {
    if (token != TBREAK) {
        return errorWithReturn(getLineNum(), "'break' is not found");
    }
    token = scanTokenOneEach();
}

int parseCallState() {
    if (token != TCALL) {
        return errorWithReturn(getLineNum(), "'call' is not found");
    }

    if (parseName() == ERROR) {
        return ERROR;
    }

    if (token == "(") {
        token = scanTokenOneEach();
        if (parseExpressions() == ERROR) {
            return ERROR;
        }
        if (token != ")") {
            return errorWithReturn(getLineNum(), "')' is not found");
        }
        token = scanTokenOneEach();
    }
}

int parseReturnState() {

}

int parseExpressions() {
    if (parseExpression() == ERROR) {
        return ERROR;
    }
    while (token == ",") {
        token = scanTokenOneEach();
        if (parseExpression() == ERROR) {
            return ERROR;
        }
    }
}

int parseAssignState() {

}

int parseInputState() {

}

int parseOutputState() {

}

int parseState() {
    if (parseAssignState() == ERROR || parseConditionState() == ERROR || parseIterationState() ||
        parseExitState() == ERROR || parseCallState() == ERROR || parseReturnState() == ERROR ||
        parseInputState() == ERROR || parseOutputState() == ERROR || parseCompoundState() == ERROR ||
        parseEmptyState() == ERROR) {
        return ERROR;
    }
}

// 式の並び
int parseSeqOfExpression() {

}

// left partと同義
int parseVariable() {

}

int parseSimpleExpression() {
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
}

int parseTerm() {
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
}

int parseFactor() {

}

int parseConst() {

}

int parseMultiOperator() {

}

int parseAddOperator() {

}

int parseOutputFormat() {

}

int parseEmptyState() { //TODO : 空文にならない条件の実装
    if (token == TNAME || token == TIF || token == TWHILE) {
        return errorWithReturn(getLineNum(), "Empty is not found");
    }
    token = scanTokenOneEach();
    return OK;

}


int parseBlock() {
    while (token == TVAR) {
        if (parseVarDecler() == ERROR) {
            return ERROR;
        }
        if (parseSubProgramDecler() == ERROR) {
            return ERROR;
        }
    }
    if (parseCompoundState() == ERROR) {
        return ERROR;
    }
}

/****************** 外部関数 *********************/
void initSyntaticAnalysis(int _token) {
    token = _token;
}

int parseProgram() {
    if (token != TPROGRAM) {
        return errorWithReturn(getLineNum(), "'program' is not found");
    }
    token = scanTokenOneEach();

    if (token != TNAME) {
        return errorWithReturn(getLineNum(), "'program name' is not found");
    }
    token = scanTokenOneEach();

    if (token != TSEMI) {
        return errorWithReturn(getLineNum(), "'program name' is not found");
    }
    token = scanTokenOneEach();

    if (parseBlock() == ERROR) {
        return ERROR;
    }
    token = scanTokenOneEach();

    if (token != TDOT) {
        return errorWithReturn(getLineNum(), "'.' is not found");
    }
    token = scanTokenOneEach();

    return OK;
}