objs = lex.yy.c \
	syntax.tab.c parser\
	

parser: main.c lex.yy.c syntax.tab.c sema.c
	gcc main.c syntax.tab.c tree.c sema.c -lfl -g -o parser


lex.yy.c: lexical.l
	flex lexical.l

syntax.tab.c: syntax.y
	bison -d -v syntax.y


.PHONY: clean
clean:
	rm -rf $(objs)
