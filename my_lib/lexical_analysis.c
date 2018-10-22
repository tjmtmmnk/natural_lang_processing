#include <stdio.h>
#include <stdlib.h>
#include "lexical_analysis.h"
#include "token_list.h"
#include "common.h"

static FILE *fp;
static char *file_name;
static int c_buf;
static int crnt_buf;
int num_attr = -1;
char str_attr[MAX_WORD_LENGTH];

static void openFile() {
	if ((fp = fopen(file_name, "r")) == NULL) {
		fprintf(stderr, "Can't open %s", file_name);
		exit(EXIT_FAILURE);
	}
}

static int isDigit(char c) {
	return c >= '0' && c <= '9';
}

static int isAlphabet(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static int isSplit(char c) {
	rep(i,0,NUM_OF_SPLIT){
		if (c == split[i]) {
			return 1;
		}
	}
	return 0;
}

static int isSymbol(char c) {
	switch (c) {
	case '+':
	case '-':
	case '*':
	case '=':
	case '<':
	case '>':
	case '(':
	case ')':
	case '[':
	case ']':
	case '.':
	case ',':
	case ':':
	case ';':
		return 1;
	default:
		return 0;
	}
}

static int isString(char c) {
	return c == '\'';
}

//連続するシンボルの判定
static int isContSymbol(char c[2]) {
	rep(i,0,NUM_OF_CONT_SYMBOL){
		if (strcmp(cont_sym[i], c) == 0) {
			return 1;
		}
	}
	return 0;
}

/* @input : 数字 or キーワード or シンボル or 名前
 * ここでは例外的な入力は認めない
 */
static int getTokenCode() {

	printf("%s\n", str_attr);
	if (num_attr > 0) {
		return TNUMBER;
	}
	rep(i,0,NUM_OF_KEY){
		if (strcmp(str_attr, key[i].keyword) == 0) {
			return key[i].token_number;
		}
	}

	rep(j,0,NUM_OF_SYMBOL){
		if (strcmp(str_attr, symbol[i].symbol) == 0) {
			return symbol[j].token_number;
		}
	}

	return TNAME;
}

static void clearBuf() {
	rep(i,0,MAX_WORD_LENGTH){
		str_attr[i] = '\0';
	}
	num_attr = 0;
}

void setFileName(char *_file_name) {
	file_name = _file_name;
}

static void updateBuf() {
	crnt_buf = c_buf;
	c_buf = fgetc(fp);
}

void initScan() {
	openFile();
	c_buf = fgetc(fp);
	clearBuf();
	crnt_buf = c_buf;
	c_buf = fgetc(fp);
}

void closeFile() {
	fclose(fp);
}

// @return : トークンコード, 失敗したら-1
int scanTokenOneEach() {
	Mode mode = MODE_NONE;
	int buf_i = 0;
	/* 初めの1字で分岐
	 * 各モードでそれぞれ処理する
	 */
	while (crnt_buf != EOF || c_buf != EOF) {
		if (isAlphabet(crnt_buf)) {
			mode = MODE_ALPHA_NUM;
		} else if (isDigit(crnt_buf)) {
			mode = MODE_NUM;
		} else if (isSplit(crnt_buf)) {
			mode = MODE_SPLIT;
		} else if (isSymbol(crnt_buf) && isSymbol(c_buf)) {
			char temp[2];
			temp[0] = crnt_buf;
			temp[1] = c_buf;
			if (isContSymbol(temp)) {
				mode = MODE_CONT_SYMBOL;
			}
		} else if (isSymbol(crnt_buf)) {
			mode = MODE_SYMBOL;
		} else if (isString(crnt_buf)) {
			mode = MODE_STRING;
		} else {
			error("Invalid word");
		}

		switch (mode) {
		case MODE_SPLIT:
			updateBuf();
			return 0;

		case MODE_ALPHA_NUM:
			while (isAlphabet(crnt_buf) || isDigit(crnt_buf)) {
				str_attr[buf_i++] = crnt_buf;
				updateBuf();
			}
			break;

		case MODE_NUM:
			while (isDigit(crnt_buf)) {
				str_attr[buf_i++] = crnt_buf;
				updateBuf();
			}
			sscanf(str_attr, "%d", &num_attr);
			if (num_attr > UNSIGNED_INT_MAX) {
				error("Over num range(0~32767)");
			}
			break;

		case MODE_SYMBOL:
			str_attr[buf_i++] = crnt_buf;
			updateBuf();
			break;

		case MODE_CONT_SYMBOL:
			str_attr[buf_i++] = crnt_buf;
			str_attr[buf_i++] = c_buf;
			updateBuf();
			break;

		case MODE_STRING:
			updateBuf();
			while (!isString(crnt_buf)) {
				updateBuf();
			}
			updateBuf();
			return TSTRING;
		default:
			break;

		}
		if (buf_i == 0) {
			updateBuf();
		}
		printf("%s\n", str_attr);
		clearBuf();
	}

	return -1;

}

int getLatestFoundTokenLine() {
	return 0;
}

