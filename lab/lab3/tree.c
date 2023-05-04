#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tree.h"


Node* create_parser_node(char* name,int lineno,int special,char* token,Node* fir_child,Node* right_nei){
	Node* cur_node = (Node* )malloc(sizeof(Node));
	
	strcpy(cur_node->name, name); 
	strcpy(cur_node->token, token); 
	
	cur_node->special=special;
	cur_node->lineno=lineno;
	cur_node->fir_child=fir_child;
	cur_node->right_nei=right_nei;
	return cur_node;
};
Node* create_gra_node(char* name,int lineno,int special,Node* fir_child,Node* right_nei){
	create_parser_node(name,lineno,special," ",fir_child,right_nei);
};
void connect_node(Node* node_1,Node* node_2){
	node_1->right_nei=node_2;	
};
void print_parser_node(Node* parser_node,int spa_count){
	//printf("spa_count is %d",spa_count);
	int i=0;
	//非空产生式
	if(parser_node->special!=5 || parser_node->fir_child!=NULL){
		for(i=0;i<spa_count;i++)
		printf(" ");
	}
	
		//词法单元一定是叶节点，叶节点不一定是词法单元 
	if(parser_node->fir_child!=NULL)
		printf("%s (%d)\n", parser_node->name, parser_node->lineno);
	else {
		if(parser_node->special!=5)//非词法单元 
			printf("%s",parser_node->name);//
		switch(parser_node->special)
		{
			case(1):
				printf(": %s\n",parser_node->token);
				break;
			case(2):
				printf(": %s\n",parser_node->token);
				break;
			case(3):
				printf(": %d\n",atoi(parser_node->token));
				break;
			case(4):
				printf(": %f\n",atof(parser_node->token));
				break;
			case(0):
				printf("\n");
				break;
			default:
				break;//空产生式 
		}
	}
	spa_count+=2;
	Node* cur_node=parser_node->fir_child;
	while(cur_node!=NULL){
		print_parser_node(cur_node,spa_count);//TODO:spa_count 作用域 
		cur_node=cur_node->right_nei;
	}
};
