#include <stdlib.h>
#include <stdio.h>
#include "tree.h"
#include "sema.h"
#include "inte.h"

int yydebug;
int token_error = 0;
int syntax_error = 0;
extern Node *parser_tree;

int id_num = 0;

int main(int argc, char **argv)
{
	if (argc <= 1)
		return 1;

	FILE *f = fopen(argv[1], "r");
	if (!f)
	{
		perror(argv[1]);
		return 1;
	}
	//-----------------------词法分析---------------------
	yyrestart(f);
	yydebug = 1;
	yyparse();
	//print_parser_node(parser_tree,0);
	// 默认 if (token_error == 0 && syntax_error == 0)



	//------------------------语法分析---------------------
	varTable *V = (varTable *)malloc(sizeof(varTable));
	funcTable *F = (funcTable *)malloc(sizeof(funcTable));
	initTable(V, F);
	semaAnaly(F, V, parser_tree);
	// 默认 没有 语法错误



	//------------------------语义分析---------------------
	if (argc > 2)
	{
		InterCodes head = inteGen(F, V, parser_tree);
		FILE *out = fopen(argv[2], "w");
		inte2File(head,out);
	}

	return 0;
}
