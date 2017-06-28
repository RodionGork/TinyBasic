#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "tokens.h"
#include "utils.h"

char* parseError;

int tokenSize(token* t) {
    switch (t->type) {
        case TT_NUMBER:
            return 1 + sizeof(t->body.integer);
        case TT_NAME:
        case TT_LITERAL:
            return 2 + t->body.text[0];
        case TT_SYMBOL:
            return 1 + sizeof(t->body.symbol);
        case TT_NONE:
        case TT_ERROR:
            return 1;
    }
    return 0;
}

char* parseName(char* s, token* tokens) {
    int i = 0;
    tokens->type = TT_NAME;
    while (isalnum(s[i])) {
        tokens->body.text[i + 1] = s[i];
        i++;
    }
    tokens->body.text[0] = i;
    return skipSpaces(s + i);
}

char* parseNumber(char* s, token* tokens) {
    tokens->type = TT_NUMBER;
    tokens->body.integer = 0;
    while (isdigit(*s)) {
        tokens->body.integer = tokens->body.integer * 10 + (*s) - '0';
        s++;
    }
    return skipSpaces(s);
}

char* parseLiteral(char* s, token* tokens) {
    int i = 1;
    tokens->type = TT_LITERAL;
    while (s[i] != 0 && s[i] != '"') {
        tokens->body.text[i] = s[i];
        i++;
    }
    tokens->body.text[0] = i - 1;
    return skipSpaces(s + i + (s[i] == '"' ? 1 : 0));
}

char* parseSymbol(char* s, token* tokens) {
    tokens->type = TT_SYMBOL;
    char c = 0;
    if (s[0] == '<') {
        if (s[1] == '>') {
            c = '!';
        } else if (s[1] == '=') {
            c = '{';
        }
    } else if (s[0] == '>' && s[1] == '=') {
        c = '}';
    }
    if (c != 0) {
        s++;
    } else {
        c = s[0];
    }
    tokens->body.symbol = c;
    return skipSpaces(s + 1);
}

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
    } else if (strchr(",+-*/%()<>=", *s) != NULL) {
        return parseSymbol(s, tokens);
    }
    tokens->type = TT_ERROR;
    parseError = s;
    return s;
}

void parseLine(char* line, void* tokens) {
    char* s = tokens;
    parseError = NULL;
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
}

int tokenClass(token* t) {
    return t->type & 0xF0;
}

char* getParseError(void) {
    return parseError;
}
