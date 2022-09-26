/* 
* [ONLY USING FLEX] 
* https://www.oreilly.com/library/view/flex-bison/9780596805418/ch01.html 참고 
*  아래 커멘드로 실행가능
* $ flex fb1-1.l
* $ cc lex.yy.c -lfl
* $ ./a.out
*
*/


%{
int chars = 0;
int words = 0;
int lines = 0;
%}

%%

[a-zA-Z]+  { words++; chars += strlen(yytext); }
\n         { chars++; lines++; }
.          { chars++; }

%%

int main(int argc, char **argv)
{
  yylex();
  printf("%8d%8d%8d\n", lines, words, chars);
}