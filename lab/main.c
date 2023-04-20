#include <stdlib.h>
#include <stdio.h>
#include "tree.h"
#include "sema.h"

int yydebug;
int token_error=0;
int syntax_error=0;
extern Node *parser_tree;


int id_num=0;




int main(int argc, char** argv)
{
	if (argc <= 1) return 1;

	// int i=1;
	// while(i < argc)
	// {
	// 	FILE *f = fopen(argv[i], "r");
	// 	if (!f) 
	// 	{
	// 		perror(argv[i]);
	// 		return 1;
	// 	}
	// 	yyrestart(f);
	// 	yydebug = 1;
	// 	yyparse();
	// 	if(token_error==0&&syntax_error==0)
	// 	{
	// 		//print_parser_node(parser_tree,0);
	// 	}
	// 	varTable* V = (varTable*)malloc(sizeof(varTable));
	// 	funcTable* F = (funcTable*)malloc(sizeof(funcTable));
	// 	initTable(V,F);
	// 	semaAnaly(F,V,parser_tree);
	// 	i++;
	// }




	FILE *f = fopen(argv[1], "r");
	if (!f) 
	{
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	yydebug = 1;
	yyparse();
	if(token_error==0&&syntax_error==0)
	{
		//print_parser_node(parser_tree,0);
	}
	varTable* V = (varTable*)malloc(sizeof(varTable));
	funcTable* F = (funcTable*)malloc(sizeof(funcTable));
	initTable(V,F);
	semaAnaly(F,V,parser_tree);



	return 0; 
  
}
