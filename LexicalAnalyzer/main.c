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
#define V_INT 0
#define V_DOUBLE 1

char tokenNameList[11][10] = {
    "ID", "INT", "REAL", "STR",
    "PLUS", "MINUS", "MULTIPLY", "DIVID", "ASSIGN", "COLON",
    "SEMICOLON"};
char operatorList[7] = {'+', '-', '*', '/', '=', ':', ';'};

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

void rollback(char* history, int len) {  // string
    for (int i = len - 1; i >= 0; i--) {
        ungetc(history[i], stdin);
    }
}
int lineNum = 1;
void skipSpace() {
    while (1) {
        char c = getchar();
        switch (c) {
            case '\n':
                lineNum++;
            case ' ':
            case '\t':
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
void printSymbolTable() {
    printf("\n");
    printf("index|\tsymbol\n");
    printf("----------------\n");
    for (int i = 0; i < symbolTableIndex; i++) {
        printf("%3d  |\t%s\n", i + 1, symbolTable[i]);
    }
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
    --count;
    ungetc(c, stdin);
    text[count++] = '\0';
    /////////////////////////////////////////////////
    int id_len = count > 10 ? 10 : count;

    char symbol[11];
    strncpy(symbol, text, id_len);
    symbol[id_len] = '\0';

    int index = getSymbolIdex(symbol);
    if (index == -1) {
        strncpy(symbolTable[symbolTableIndex++], symbol, id_len);
        index = symbolTableIndex;
    }
    return (Token){ID, index, V_INT};
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
    --count;
    ungetc(c, stdin);
    text[count++] = '\0';
    /////////////////////////////////////////////////
    int i = atoi(text);
    return (Token){INT, i, V_INT};
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
    --count;
    ungetc(c, stdin);
    text[count++] = '\0';
    /////////////////////////////////////////////////
    char* eptr;
    double d = strtod(text, &eptr);
    return (Token){REAL, .value.doubleValue = d, V_DOUBLE};
}

int stringTableIndex = 0;
char stringTable[100][100];

int getStringIdex(char* symbol) {
    for (int i = 0; i < stringTableIndex; i++) {
        if (strcmp(symbol, stringTable[i]) == 0) {
            return i + 1;
        }
    }
    return -1;
}
void printStringTable() {
    printf("\n");
    printf("index|\tstring\n");
    printf("----------------\n");
    for (int i = 0; i < stringTableIndex; i++) {
        printf("%3d  |\t%s\n", i + 1, stringTable[i]);
    }
}

Token getString() {
    int lineChange = 0;
    char text[100];
    int count = 0;
    char c;
    int state = 0;
    while (state != -1) {
        c = getchar();
        text[count++] = c;
        switch (state) {
            case 0:
                if (c == '"') {
                    state = 1;
                } else {
                    rollback(text, count);
                    return failToken;
                }
                break;
            case 1:
                if (c == '\n') {
                    rollback(text, count);
                    return failToken;
                } else if (c == '"') {
                    state = -1;
                } else if (c == '\\') {
                    state = 2;
                } else {
                    state = 1;
                }
                break;
            case 2:
                if (c == '\n') {
                    lineChange++;
                }
                state = 1;
                break;
        }
    }
    // --count;
    // ungetc(c, stdin);
    text[count++] = '\0';
    lineNum += lineChange;
    /////////////////////////////////////////////////

    int index = getStringIdex(text);
    if (index == -1) {
        strncpy(stringTable[stringTableIndex++], text, count);
        index = stringTableIndex;
    }
    return (Token){STR, index, V_INT};
}

Token getNextToken() {
    skipSpace();
    Token token;
    if (!isFailToken(token = getIdentifier())) {
        return token;
    }
    if (!isFailToken(token = getOperator())) {
        return token;
    }
    if (!isFailToken(token = getReal())) {
        return token;
    }
    if (!isFailToken(token = getInteger())) {
        return token;
    }
    if (!isFailToken(token = getString())) {
        return token;
    }
    char failChar = getchar();
    printf("Error: line %d\t\t%c\n", lineNum, failChar);
    return failToken;
}

void printToken(Token token) {
    // if (token.hasValue) {
    if (token.valueType == V_INT && token.name == ID) {
        printf("<%-3s,%2d>\t\t%s\n", tokenNameList[token.name], token.value.intValue, symbolTable[token.value.intValue - 1]);
    } else if (token.valueType == V_INT && token.name == STR) {
        printf("<%-3s,%2d>\t\t%s\n", tokenNameList[token.name], token.value.intValue, stringTable[token.value.intValue - 1]);
    } else if (token.valueType == V_INT) {
        printf("<%-3s,%2d>\t\t%d\n", tokenNameList[token.name], token.value.intValue, token.value.intValue);
    } else if (token.valueType == V_DOUBLE) {
        printf("<%-3s,%2f>\t\t%f\n", tokenNameList[token.name], token.value.doubleValue, token.value.doubleValue);
    } else {
        printf("<%-3s, >\t\t%c\n", tokenNameList[token.name], operatorList[token.name - PLUS]);
    }
}

int main(int argc, char* argv[]) {
    char c;
    while ((c = getchar()) != EOF) {
        ungetc(c, stdin);
        Token token = getNextToken();
        if (!isFailToken(token)) {
            printToken(token);
        }
    }
    printSymbolTable();
    printStringTable();
    return 0;
}
