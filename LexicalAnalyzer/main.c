#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ID 0
#define INT 1
#define REAL 2
#define STR 3
// operator
#define PLUS 4
#define MINUS 5
#define MULTIPLY 6
#define DIVIDE 7
#define ASSIGN 8
#define COLON 9
// other
#define SEMICOLON 10

#define NONE -1
#define INT 0
#define DOUBLE 1

char tokenNameList[11][10] = {
    "ID", "INT", "REAL", "STR",
    "PLUS", "MINUS", "MULTIPLY", "DIVID", "ASSIGN", "COLON",
    "SEMICOLON"};

union value {
    int intValue;
    double doubleValue;
};
struct _Token {
    int name;
    union value value;
    int valueType;
} typedef Token;
Token failToken = {-1, -1, NONE};

int isEqual(Token t1, Token t2) {
    return memcmp(&t1, &t2, sizeof(Token)) == 0;
}
int isFailToken(Token t) {
    return isEqual(t, failToken);
}

void printToken(Token token) {
    // if (token.hasValue) {
    if (token.valueType == INT) {
        printf("<%s,%d>\n", tokenNameList[token.name], token.value.intValue);
    } else if (token.valueType == DOUBLE) {
        printf("<%s,%f>\n", tokenNameList[token.name], token.value.doubleValue);
    } else {
        printf("<%s,>\n", tokenNameList[token.name]);
    }
}

void rollback(char* history, int len) {  // string
    for (int i = len - 1; i >= 0; i--) {
        ungetc(history[i], stdin);
    }
}

void skipSpace() {
    while (1) {
        char c = getchar();
        switch (c) {
            case ' ':
            case '\t':
            case '\n':
                break;
            default:
                rollback(&c, 1);
                return;
        }
    }
}

Token getOperator() {  // includes semicolon
    char c = getchar();
    switch (c) {
        case '+':
            return (Token){PLUS, 0, NONE};
        case '-':
            return (Token){MINUS, 0, NONE};
        case '*':
            return (Token){MULTIPLY, 0, NONE};
        case '/':
            return (Token){DIVIDE, 0, NONE};
        case '=':
            return (Token){ASSIGN, 0, NONE};
        case ':':
            return (Token){COLON, 0, NONE};
        case ';':
            return (Token){SEMICOLON, 0, NONE};
        default:
            rollback(&c, 1);
            return failToken;
            break;
    }
}

int symbolTableIndex = 0;
char symbolTable[100][11];

int getSymbolIdex(char* symbol) {
    for (int i = 0; i < symbolTableIndex; i++) {
        if (strcmp(symbol, symbolTable[i]) == 0) {
            return i + 1;
        }
    }
    return -1;
}

Token getIdentifier() {
    char text[100];
    int count = 0;
    char c;
    int state = 0;
    while (state != -1) {
        c = getchar();
        text[count++] = c;
        switch (state) {
            case 0:
                if ('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z') {
                    state = 1;
                } else {
                    rollback(text, count);
                    return failToken;
                }
                break;
            case 1:
                if ('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z') {
                    state = 1;
                } else if ('0' <= c && c <= '9') {
                    state = 2;
                } else {
                    state = -1;
                }
                break;
            case 2:
                if ('0' <= c && c <= '9') {
                    state = 2;
                } else {
                    state = -1;
                }
                break;
        }
    }
    ungetc(c, stdin);
    text[count] = '\0';
    /////////////////////////////////////////////////
    char symbol[11];
    strncpy(symbol, text, 10);

    int index = getSymbolIdex(symbol);
    if (index == -1) {
        strncpy(symbolTable[symbolTableIndex++], symbol, 11);
        index = symbolTableIndex;
    }
    return (Token){ID, index, INT};
}

Token getInteger() {
    char text[100];
    int count = 0;
    char c;
    int state = 0;
    while (state != -1) {
        c = getchar();
        text[count++] = c;
        switch (state) {
            case 0:
                if ('0' < c && c <= '9') {
                    state = 1;
                } else if (c == '0') {
                    state = 2;
                } else {
                    rollback(text, count);
                    return failToken;
                }
                break;
            case 1:
                if ('0' <= c && c <= '9') {
                    state = 1;
                } else {
                    state = -1;
                }
                break;
            case 2:
                state = -1;
                break;
        }
    }
    ungetc(c, stdin);
    text[count] = '\0';
    /////////////////////////////////////////////////
    int i = atoi(text);
    return (Token){INT, i, INT};
}

Token getReal() {
    char text[100];
    int count = 0;
    char c;
    int state = 0;
    while (state != -1) {
        c = getchar();
        text[count++] = c;
        switch (state) {
            case 0:
                if ('0' <= c && c <= '9') {
                    state = 0;
                } else if (c == '.') {
                    state = 1;
                } else {
                    rollback(text, count);
                    return failToken;
                }
                break;
            case 1:
                if ('0' <= c && c <= '9') {
                    state = 1;
                } else {
                    state = -1;
                }
                break;
        }
    }
    ungetc(c, stdin);
    text[count] = '\0';
    /////////////////////////////////////////////////
    char* eptr;
    double d = strtod(text, &eptr);
    return (Token){REAL, .value.doubleValue = d, DOUBLE};
}

Token getString() {
    char text[100];
    int count = 0;
    char c;
    int state = 0;
    while (state != -1) {
        c = getchar();
        text[count++] = c;
        switch (state) {
            case 0:
                state = -1;
                break;
            case 1:
                break;
            case 2:
                break;
        }
    }
    ungetc(c, stdin);
    text[count] = '\0';
    /////////////////////////////////////////////////

    return failToken;
}

Token getNextToken() {
    skipSpace();
    Token token;

    token = getIdentifier();
    if (!isFailToken(token)) {
        return token;
    }

    token = getOperator();
    if (!isFailToken(token)) {
        return token;
    }

    token = getReal();
    if (!isFailToken(token)) {
        return token;
    }

    token = getInteger();
    if (!isFailToken(token)) {
        return token;
    }

    token = getString();
    if (!isFailToken(token)) {
        return token;
    }

    char failChar = getchar();
    return failToken;
}

int main(int argc, char* argv[]) {
    for (int i = 0; i < 50; i++) {
        printToken(getNextToken());
    }
    return 0;
}
