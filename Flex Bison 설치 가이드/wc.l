%{

#include "wc.tab.h"

%}

%%

[a-zA-Z]+ { yylval.nchars = strlen(yytext); return WORD; }
\n	  { return NEWLINE; }
<<EOF>>   { return END; }
.	  { return ETC; }

%%

