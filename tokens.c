#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "tokens.h"
#include "utils.h"

char* cmds[] = {
    "-nocmd-",
    "PRINT",
    "INPUT",
    "IF",
    "GOTO",
    "GOSUB",
    "RETURN",
    "REM",
    "END",
    ""
};

char* errorMsgs[] = {
    "ok",
    "command or variable expected",
};

char* cur;
token* curTok;
token* prevTok;
char* parseError;
char parseErrorCode;

int tokenSize(token* t) {
    switch (t->type) {
        case TT_NUMBER:
            return 1 + sizeof(t->body.integer);
        case TT_NAME:
        case TT_LITERAL:
            return 2 + t->body.str.len;
        case TT_SYMBOL:
        case TT_COMMAND:
            return 1 + sizeof(t->body.symbol);
        case TT_NONE:
        case TT_ERROR:
            return 1;
    }
    return 0;
}

void setError(char* pos, char code) {
    parseErrorCode = code;
}

void advance(char* s) {
    cur = skipSpaces(s);
    prevTok = curTok;
    curTok = ((void*) curTok) + tokenSize(curTok);
}

int trySubstCmd(void) {
    short i = 0;
    for (i = 0; cmds[i][0] != 0; i++) {
        if (tokenNameEqual(curTok, cmds[i])) {
            curTok->type == TT_COMMAND;
            curTok->body.command = i;
            return i;
        }
    }
    return 0;
}

int parseName(char checkCmd) {
    short i = 0;
    if (!isalpha(*cur)) {
        return 0;
    }
    curTok->type = TT_NAME;
    while (isalnum(cur[i])) {
        curTok->body.str.text[i] = toupper(cur[i]);
        i++;
    }
    curTok->body.str.len = i;
    if (checkCmd) {
        trySubstCmd();
    }
    advance(cur + i);
    return 1;
}

int parseNumber(void) {
    if (!isdigit(*cur)) {
        return 0;
    }
    curTok->type = TT_NUMBER;
    curTok->body.integer = 0;
    while (isdigit(*cur)) {
        curTok->body.integer = curTok->body.integer * 10 + (*cur) - '0';
        cur++;
    }
    advance(cur);
    return 1;
}

void parseComment(void) {
    unsigned char len = strlen(cur);
    curTok->type == TT_COMMENT;
    curTok->body.str.len = len;
    memcpy(&(curTok->body.str.text), cur, len);
    advance(cur + len);
}

char* parseLiteral(char* s, token* tokens) {
    int i = 1;
    tokens->type = TT_LITERAL;
    while (s[i] != 0 && s[i] != '"') {
        tokens->body.str.text[i - 1] = s[i];
        i++;
    }
    tokens->body.str.len = i - 1;
    return skipSpaces(s + i + (s[i] == '"' ? 1 : 0));
}

void parseSymbol() {
    curTok->type = TT_SYMBOL;
    char c = 0;
    if (cur[0] == '<') {
        if (cur[1] == '>') {
            c = '!';
        } else if (cur[1] == '=') {
            c = '{';
        }
    } else if (cur[0] == '>' && cur[1] == '=') {
        c = '}';
    }
    if (c != 0) {
        cur++;
    } else {
        c = cur[0];
    }
    curTok->body.symbol = c;
    advance(cur + 1);
}

void parseNone(void) {
}

/*
char* parseToken(char* s, token* tokens) {
    if (*s == 0) {
        tokens->type = TT_NONE;
        return NULL;
    } else if (isalpha(*s)) {
        return parseName(s, tokens);
    } else if (isdigit(*s)) {
        return parseNumber(s, tokens);
    } else if (*s == '"') {
        return parseLiteral(s, tokens);
    } else if (strchr(",+-*%/()<>=", *s) != NULL) {
        return parseSymbol(s, tokens);
    }
    tokens->type = TT_ERROR;
    parseError = s;
    return s;
}
*/

void parseExpression(void) {
}

void skipLineNumber(void) {
    parseNumber();
}

void parseAssignment(void) {
    if (*cur != '=') {
        setError(cur, 2);
    }
    parseSymbol();
    parseExpression();
}

void parseExprList(void) {
}

void parseVarList(void) {
}

void parseStatement(void) {
    char cmd;
    if (!parseName(1)) {
        setError(cur, 1);
    } else if (prevTok->type != TT_COMMAND) {
        parseAssignment();
    }
    cmd = prevTok->body.command;
    if (cmd == CMD_REM) {
        parseComment();
    } else if (cmd == CMD_GOTO || cmd == CMD_GOSUB) {
        if (parseNumber()) {
            parseNone();
        }
    } else if (cmd == CMD_RETURN || cmd == CMD_END) {
        parseNone();
    } else if (cmd == CMD_PRINT) {
        parseExprList();
    } else if (cmd == CMD_INPUT) {
        parseVarList();
    }
}

void parseLine(char* line, void* tokens) {
    cur = line;
    curTok = tokens;
    setError(NULL, 0);
    skipLineNumber();
    parseStatement();
    /*
    char* s = tokens;
    do {
        line = parseToken(line, tokens);
        if (parseError != NULL) {
            break;
        }
        tokens += tokenSize(tokens);
    } while (line != NULL);
    if (parseError == NULL) {
        ((token*)tokens)->type = TT_NONE;
    }
    */
}

int tokenClass(token* t) {
    return t->type & 0xF0;
}

int tokenNameEqual(token* t, char* s) {
    if (tokenClass(t) != TT_NAME) {
        return 0;
    }
    if (t->body.str.len != strlen(s)) {
        return 0;
    }
    return memcmp(t->body.str.text, s, t->body.str.len) == 0;
}

char* getParseError(void) {
    return parseError;
}

