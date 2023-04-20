#ifndef _TREE_H_
#define _TREE_H_

#include <stdio.h>
typedef struct parser_node
{
 
    char name[15];   // 节点名称
 	int lineno;
	int special;	//0:other 词法单元 1:ID 2:TYPE 3:INT 4:FLOAT 5:语法单元   
	char token[32]; //词素
    struct parser_node* fir_child; // 最左孩子 
    struct parser_node* right_nei; // 右兄弟 
}Node;

Node* create_parser_node(char* name,int lineno,int special,char* token,Node* fir_child,Node* right_nei);
Node* create_gra_node(char* name,int lineno,int special,Node* fir_child,Node* right_nei);
void print_parser_node(Node* parser_node,int spa_count);
void connect_node(Node* node_1,Node* node_2);
#endif

