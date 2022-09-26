%{

#include <stdio.h>

int yylex();
int yyerror(char const *);

int chars = 0;
int words = 0;
int lines = 0;
%}

%token END
%token <nchars> WORD
%token NEWLINE ETC

%union { int nchars; }

%%

wordlist: /* nothing */ 
   | wordlist expr END { printf("lines: %d, words: %d, chars: %d\n", lines, words, chars); YYACCEPT; }
   ;

expr: term { /* nothing */ }
    | expr term  { /* nothing */ }

term : WORD { words++; chars += $1; }
     | NEWLINE { chars++; lines++; }
     | ETC { chars++; }
     ;