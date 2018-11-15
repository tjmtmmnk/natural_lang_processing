#include "syntatic_analysis.h"
#include "lexical_analysis.h"

static int token;
static int line_num;

void initSyntaticAnalysis(int _token) {
    token = _token;
}

int parseProgram() {
    if (token != TPROGRAM) {
        return errorWithReturn(line_num, "'program' is not found");
    }
    token = scanTokenOneEach();

    if (token != TNAME) {
        return errorWithReturn(line_num, "'program name' is not found");
    }
    token = scanTokenOneEach();

    if (token != TSEMI) {
        return errorWithReturn(line_num, "'program name' is not found");
    }
    token = scanTokenOneEach();

    if (parseBlock() == ERROR) {
        return ERROR;
    }
    token = scanTokenOneEach();

    if (token != TDOT) {
        return errorWithReturn(line_num, "'.' is not found");
    }
    token = scanTokenOneEach();

    return OK;
}

static int parseBlock() {

}

static int parseVarDecler() {

}

static int parseName() {

}

static int parseType() {

}

static int parseStandardType() {

}

static int parseArrayType() {

}

static int parseSubProgramDecler() {

}

static int parseFormalParam() {

}

static int parseCompoundState() {

}

static int parseState() {

}

static int parseIterationState() {

}

static int parseExitState() {

}

static int parseCallState() {

}

// 式の並び
static int parseSeqOfExpression() {

}

static int parseReturnState() {

}

static int parseAssignState() {

}

// left partと同義
static int parseVariable() {

}

static int parseExpression() {

}

static int parseSimpleExpression() {

}

static int parseTerm() {

}

static int parseFactor() {

}

static int parseConst() {

}

static int parseMultiOperator() {

}

static int parseAddOperator() {

}

static int parseInputState() {

}

static int parseOutputState() {

}

static int parseOutputFormat() {

}

static int parseEmptyState() { //TODO : 空文にならない条件の実装
    if (token == TNAME || token == TIF || token == TWHILE) {
        return errorWithReturn(line_num, "Empty is not found");
    }
    token = scanTokenOneEach();
    return OK;

}

static