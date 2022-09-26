wc : 	wc.l wc.y  
	bison -d wc.y
	flex wc.l
	gcc -o $@ wc.tab.c lex.yy.c -ly -lfl

.PHONY : clean 
clean : 
	rm -rf *.tab.c *.tab.h *.yy.c wc
