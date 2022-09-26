#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

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

char tokenNameList[11][10] = {
    "ID", "INT", "REAL", "STR",
    "PLUS", "MINUS", "MULTIPLY", "DIVID", "ASSIGN", "COLON",
    "SEMICOLON"};

struct _Token {
    int name;
    int value;
    int hasValue;
} typedef Token;
Token failToken = {-1, -1, FALSE};

int isEqual(Token t1, Token t2) {
    return memcmp(&t1, &t2, sizeof(Token)) == 0;
}
int isFailToken(Token t) {
    return isEqual(t, failToken);
}

void printToken(Token token) {
    if (token.hasValue) {
        printf("<%s,%d>\n", tokenNameList[token.name], token.value);
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
            return (Token){PLUS, 0, FALSE};
        case '-':
            return (Token){MINUS, 0, FALSE};
        case '*':
            return (Token){MULTIPLY, 0, FALSE};
        case '/':
            return (Token){DIVIDE, 0, FALSE};
        case '=':
            return (Token){ASSIGN, 0, FALSE};
        case ':':
            return (Token){COLON, 0, FALSE};
        case ';':
            return (Token){SEMICOLON, 0, FALSE};
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
    return (Token){ID, index, TRUE};
}

Token getInteger2() {
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
        }
    }
    ungetc(c, stdin);
    text[count] = '\0';
    /////////////////////////////////////////////////
    int i = atoi(text);
    return (Token){INT, i, TRUE};
}

Token getInteger() {
    char signHistory[2];

    int i = 0;
    int sign = 1;
    char c;
    int state = 0;
    while (state != -1) {
        c = getchar();
        switch (state) {
            case 0:
                if (c == '+' || c == '-') {
                    signHistory[0] = c;
                    if (c == '-') {
                        sign = -1;
                    }
                    state = 2;
                } else if ('0' < c && c <= '9') {
                    i = i * 10 + c - '0';
                    state = 1;
                } else if (c == '0') {
                    return (Token){INT, 0, TRUE};
                } else {
                    rollback(&c, 1);
                    return failToken;
                }
                break;
            case 1:
                if ('0' <= c && c <= '9') {
                    i = i * 10 + c - '0';
                    state = 1;
                } else {
                    state = -1;
                }
                break;
            case 2:
                if ('0' < c && c <= '9') {
                    i = i * 10 + c - '0';
                    state = 1;
                } else {
                    signHistory[1] = c;
                    rollback(signHistory, 2);
                    return failToken;
                }
        }
    }
    ungetc(c, stdin);
    return (Token){INT, sign * i, TRUE};
}

Token getReal() {
    return failToken;
}

Token getString() {
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

    token = getInteger2();
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
