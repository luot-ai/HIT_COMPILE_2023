#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tree.h"
#include "sema.h"

unsigned int hash_pjw(char* name)
{
 unsigned int val = 0, i;
 for (; *name; ++name)
 {
    val = (val << 2) + *name;
    if (i = val & ~BUCKETCOUNT) val = (val ^ (i >> 12)) & BUCKETCOUNT;
 }
 return val;
}

void PrintWrongInfo(enum ErrorType wrongType,int lineno,char* name)
{
	switch(wrongType){
		case UNDEVAR  :
			printf("Error type 1 at Line %d: Undefined variable \"%s\".\n",lineno,name);
		break; 
		
		case UNDEFUNC  :
			printf("Error type 2 at Line %d: Undefined function \"%s\".\n",lineno,name);
		break; 

		case REDEVAR  :
			printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",lineno,name);
		break; 

		case REDEFUNC  :
			printf("Error type 4 at Line %d: Redefined function \"%s\".\n",lineno,name);
		break; 

		case ASSIGNMISMATCH:
			printf("Error type 5 at Line %d: Type mismatched for assignment.\n",lineno);
		break;

		case LEFTNOTVAR:
			printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n",lineno);
		break;

		case OPMISMATCH:
			printf("Error type 7 at Line %d: Type mismatched for operands.\n",lineno);
		break;	

		case RETNMISMATCH:
			printf("Error type 8 at Line %d: Type mismatched for return.\n",lineno);
		break;	
		//9直接在主程序里打印了
		case NOTARRAY:
			printf("Error type 10 at Line %d: \"%s\" is not an array.\n" , lineno , name) ;
		break;	

		case NOTFUNC:
			printf("Error type 11 at Line %d: \"%s\" is not an function.\n" , lineno , name) ;
		break;	
		//12直接在主程序里打印了
		case NOTSTRUCT:
			printf("Error type 13 at Line %d: Illegal use of \"%s\".\n" , lineno , name) ;
		break;

		case NOFIELD:
			printf("Error type 14 at Line %d: Non-existent field \"%s\".\n" , lineno , name) ;
		break;

		case REDEFIELD:
			printf("Error type 15 at Line %d: Redefined field \"%s\".\n" , lineno ,name) ;
		break;	

		case DUPSTNAME:
			printf("Error type 16 at Line %d: Duplicated name \"%s\".\n" , lineno ,name) ;
		break;

		case UNDESTRUCT  :
			printf("Error type 17 at Line %d: Undefined structure \"%s\".\n",lineno,name);
		break; 

		default :
			printf("haha\n");
	}
}

void initTable(varTable* V , funcTable* F )
{
	int i;
	if (V == NULL||F==NULL)return;

	for (i = 0; i < BUCKETCOUNT; ++i) {
			V->bucket[i].name= NULL;
			V->bucket[i].type= NULL;
			V->bucket[i].next= NULL;

    		F->bucket[i].name= NULL;
			F->bucket[i].return_type= NULL;
			F->bucket[i].param_num= 0;
			F->bucket[i].param_types= NULL;
			F->bucket[i].next= NULL;
	}
}

varEntry* constructVarEntry(Type type,char* name)
{
	varEntry* s = (varEntry* )malloc(sizeof(varEntry));
	s->name=strdup(name); 
	s->type=type;
	s->next=NULL;	
	return s;
}

void insertVarTable(varEntry* s,char* name,varTable* t)
{
    	varEntry *p;
		unsigned int addr = hash_pjw(name);
		if(t->bucket[addr].next==NULL)
		{
			t->bucket[addr].next=s;
		}
		else
		{
			p=t->bucket[addr].next;
			while (p->next != NULL) 
				p = p->next;
			p->next = s;
		}
}

void insertFuncTable(char* name,funcTable* t,Type return_type,int param_num,TypeList param_types)
{
    	funcEntry *p;
		funcEntry* s = (funcEntry *)malloc(sizeof(funcEntry));
		s->name=strdup(name); 
		s->return_type=return_type;
		s->param_num=param_num;
		s->param_types=param_types;
		s->next=NULL;

		unsigned int addr = hash_pjw(name);
		if(t->bucket[addr].next==NULL)
			t->bucket[addr].next=s;
		else
		{
			p=t->bucket[addr].next;
			while (p->next != NULL) 
				p = p->next;
			p->next = s;
		}
}


varEntry* searchVarTable(char* name,varTable* t)
{
    varEntry *p,*s;

	unsigned int addr = hash_pjw(name);
	p=t->bucket[addr].next;
	while(p!=NULL)
	{
		if(strcmp(name,p->name)==0)
		{
			return p;
		}
		else 
		{
			p=p->next;
		}
	}
	return NULL;
}
funcEntry* searchFuncTable(char* name,funcTable* t)
{
    funcEntry *p,*s;

	unsigned int addr = hash_pjw(name);
	p=t->bucket[addr].next;
	while(p!=NULL)
	{
		if(strcmp(name,p->name)==0)return p;
		else p=p->next;
	}
	return NULL;
}

void semaAnaly(funcTable* F,varTable* V,Node* parser_node)
{
	//0501
Type read_return_type = (Type)malloc(sizeof(TypeEntry));
read_return_type->kind=BASIC;
read_return_type->u.basic=basic_type_int;
insertFuncTable("read" , F , read_return_type , 0 , NULL);

Type write_return_type = (Type)malloc(sizeof(TypeEntry));
write_return_type->kind=BASIC;
write_return_type->u.basic=basic_type_int;

Type write_param_type = (Type)malloc(sizeof(TypeEntry));
write_param_type->kind=BASIC;
write_param_type->u.basic=basic_type_int;

TypeList p=(TypeList)malloc(sizeof(TLEntry));
p->next=NULL;
p->type=write_param_type;

TypeList write_param_types = (TypeList)malloc(sizeof(TLEntry)) ;
write_param_types->type=NULL;
write_param_types->next=p;

insertFuncTable("write" , F , write_return_type , 1 , write_param_types);




	Node* ExtDefList=parser_node->fir_child;
	ExtDefListHandler(ExtDefList,F,V);
}
void ExtDefListHandler(Node* ExtDefList,funcTable* F,varTable* V)
{
	if (ExtDefList->fir_child!=NULL)
	{
		Node* ExtDef=ExtDefList->fir_child;
		Node* specifier=ExtDef->fir_child;
		Node* secondChild=specifier->right_nei;//产生式第二个语法单元

		if(strcmp(secondChild->name, "ExtDecList") == 0)   
		{
			Type type=getType(specifier,V);
			ExtDecListHandler(type,secondChild,V);//全局变量
		}
		else if((strcmp(secondChild->name, "FunDec") == 0)) {//如果函数的返回类型是没定义过的结构体变量，直接pass
			Type type=getType(specifier,V);
			if(type!=NULL)
			{
				//printf("FuncDec begin\n");
				FunDecHandler(type,secondChild,F,V);//函数定义
				//printf("FuncDec OK\n");
			}
		}
		else if(strcmp(secondChild->name, "SEMI") == 0)		//结构体定义 1
		{
			if(strcmp(specifier->fir_child->name, "StructSpecifier") == 0)//实际上会有无意义的 int； 出现
			{
				//printf("ready to st   \n");
				Node* StructSpecifier = specifier->fir_child;
				structDefineHandler(StructSpecifier , F ,V);
			}
		}								

		//if(ExtDef->right_nei!=NULL)
		//{
			ExtDefList=ExtDef->right_nei;
			ExtDefListHandler(ExtDefList,F,V);
		//}
	}
}

void structDefineHandler(Node* StructSpecifier,funcTable* F,varTable* V)
{
	//----------use the first --------------
	// StructSpecifier → STRUCT OptTag LC DefList RP | STRUCT Tag
	// OptTag → ID|空
	// Tag → ID
	Node* STRUCT = StructSpecifier->fir_child;
	if(strcmp(STRUCT->right_nei->name, "Tag") == 0)
	{
		//do nothing
	}
	else if((strcmp(STRUCT->right_nei->name, "OptTag") == 0)&&(STRUCT->right_nei->fir_child!=NULL))//TODO:opttag产生空？
	{
		Node* ID=STRUCT->right_nei->fir_child;
		char* structDefName=strdup(ID->token);

		varEntry* searchVar = searchVarTable(structDefName,V);
		if(searchVar!=NULL)
		{
			int lineno=STRUCT->right_nei->lineno;
			PrintWrongInfo(DUPSTNAME , lineno , structDefName);
		}
		else
		{
			Node* LC=STRUCT->right_nei->right_nei;
			if(strcmp(LC->right_nei->name, "DefList") == 0)//否则说明没定义域
			{
				Node* DefList=LC->right_nei;
				Type stDefType = (Type)malloc(sizeof(TypeEntry));

				stDefType->kind=STRUCTUREDEF;
				stDefType->u.structure=stFieldHandler(DefList,V);

				varEntry* stDef=constructVarEntry(stDefType,structDefName) ;
				InsertVarDec(stDef,V,STRUCT->right_nei);//rigth_nei: opt_tag just for lineno  TODO:may change
				// =stFieldHandler(DefList,V);
			}
		}
	}
	

}

FieldList stFieldHandler(Node* DefList,varTable* V)
{
	FieldList return_fieldlist=(FieldList)malloc(sizeof(FEntry));
	return_fieldlist->name=NULL;
	return_fieldlist->type=NULL;
	return_fieldlist->tail=NULL;

	while (DefList->fir_child != NULL)
	{
		Node* Def=DefList->fir_child;
		Node* Specifier=Def->fir_child;
		Type fieldType=getType(Specifier,V);
		if(fieldType!=NULL)
		{
			Node* DecList=Specifier->right_nei;
			while (DecList!=NULL)
			{
				Node* Dec=DecList->fir_child;

				Node* VarDec=Dec->fir_child;
				VarDecForST(fieldType , V , VarDec , return_fieldlist);
				//printf("done isner   \n");
				if(Dec->fir_child->right_nei!=NULL)
				{
					varEntry* wrongInit= VarDecHandler(fieldType,V,VarDec,1);
					//PrintWrongInfo(REDEFIELD,VarDec->lineno,wrongInit->name); 
					printf("Error type 15 at Line %d: initial field \"%s\" when define a struct.\n" , VarDec->lineno ,wrongInit->name) ;      //定义结构体的域时不能进行初始化   //但是还是让其构建域链表
				}

				if(Dec->right_nei!=NULL) DecList=Dec->right_nei->right_nei;
				else {DecList=NULL;}
			}			
		}//否则跳过

		DefList=Def->right_nei;
		//printf("right nei is %s\n",Def->right_nei->name);
	}
	return return_fieldlist;
}

void VarDecForST(Type type,varTable* V,Node* VarDec,FieldList FL)
{
	varEntry* varDecEntry= VarDecHandler(type,V,VarDec,1);
	char* name = strdup(varDecEntry->name);
	//printf("name is %s  \n",name);
	//FL->name = strdup (varDecEntry->name) ;

	FieldList p=FL;
	while(p->tail!=NULL)
	{
		//printf("compare name is %s  \n",p->tail->name);
		if(strcmp(name,p->tail->name)==0)
		{
			PrintWrongInfo(REDEFIELD,VarDec->lineno,name);
			return;
		}
		p=p->tail;
	}

	FieldList s = (FieldList)malloc(sizeof(FEntry));
		
		s->name=strdup(name); 
		s->type=type;
		s->tail=NULL;
	p->tail=s;

	//-------TODO：目前结构体域名没有被当作 变量  添加到表内    （不考虑 变量名和域名 以及 不同结构体域名 重复的情况）
	// varEntry* searchVar = searchVarTable(name,V);
	// if(searchVar!=NULL)
	// {
	// 	int lineno=VarDec->lineno;
	// 	PrintWrongInfo(REDEVAR , lineno , name);//这样的话 域内重合 会被打印多次
	// }
	// else
	// {
	//printf("ready to isner   \n");
	// 	insertVarTable(varDecEntry,name,V);
	// }

}



Type getType(Node* Specifier,varTable* V)
{
	Node* cur_node=Specifier->fir_child;
	if(strcmp(cur_node->name, "TYPE") == 0) 
	{
		Type return_type = (Type)malloc(sizeof(TypeEntry));
		return_type->kind=BASIC;
		if(strcmp(cur_node->token,"int")==0)   return_type->u.basic=basic_type_int;
		else if(strcmp(cur_node->token,"float")==0)	return_type->u.basic=basic_type_float;

		return return_type;
	}
	else if(strcmp(cur_node->name,"StructSpecifier")==0)
	{
		Node* TAG=cur_node->fir_child->right_nei;
		Node* stID=TAG->fir_child;
		char* stName=strdup(stID->token);
		varEntry* stDef=searchVarTable(stName,V);

		if(stDef==NULL || stDef->type->kind!=STRUCTUREDEF )
		{
			PrintWrongInfo(UNDESTRUCT,stID->lineno,stName);
			return NULL;
		}
		Type return_type= (Type)malloc(sizeof(TypeEntry));
		return_type->kind=STRUCTUREVAR;
		return_type->u.stTypeName=strdup(stName);
		return return_type;
	}//这里的产生式应该是这样 STRUCT TAG
}


void FunDecHandler(Type return_type,Node* FuncDec,funcTable* F,varTable* V)
{
	
	int param_num;
	TypeList param_types = (TypeList)malloc(sizeof(TLEntry)) ;
	param_types->type=NULL;
	param_types->next=NULL;

	int lineno=FuncDec->lineno;

	Node* ID=FuncDec->fir_child;
	Node* LP=ID->right_nei;

	char* funcName=strdup(ID->token);
	if(strcmp(LP->right_nei->name,"RP")==0) 
	{
		param_num=0;
		param_types=NULL;
	}
	else
	{
		Node* VarList=LP->right_nei;
		param_num = VarListHandler(VarList,V,param_types);
	}

	funcEntry* searchFunc = searchFuncTable(funcName , F);
	//printf("funcName is %s\n",funcName);
	if(searchFunc == NULL){
	insertFuncTable(funcName , F , return_type , param_num , param_types);
	}
	else{
		PrintWrongInfo(REDEFUNC , lineno , funcName);
	}

	Node* Compst=FuncDec->right_nei;
	CompstHandler(V,Compst,F,return_type);
}

void CompstHandler(varTable* V,Node* Compst,funcTable* F,Type return_type)
{
	Node* LC=Compst->fir_child;
	Node* DefList=LC->right_nei;
	Node* StmtList=DefList->right_nei;
	DefListHandler(V,DefList,F);
	//printf("DEFLISTOK\n");
	StmtListHandler(StmtList,V,F,return_type);
	//printf("STMTLISTOK\n");
}

void DefListHandler(varTable* V,Node* DefList,funcTable* F)
{
	Node* Def=DefList->fir_child;
	if(Def==NULL)
		return;
	else
	{
		Node* specifier=Def->fir_child;
		Type type=getType(specifier,V);
		if(type!=NULL)//说明用了未定义的结构体
		{
			//printf("type is %d\n",type->u.basic);
			Node* DecList=specifier->right_nei;
			DecListHandler(type,V,DecList,F);
			//printf("DECLISTOK\n");
		}

		//DefList---Def DefList | 空
		DefListHandler(V,Def->right_nei,F);
	}
}

void DecListHandler(Type type,varTable* V,Node* DecList,funcTable* F)
{
	Node* Dec=DecList->fir_child;
	DecHandler(type,V,Dec,F);
	
	while(Dec->right_nei!=NULL)
	{
		Dec=Dec->right_nei->right_nei->fir_child;//Dec Comma Dec
		DecHandler(type,V,Dec,F);
	}
}

void DecHandler(Type type,varTable* V,Node* Dec,funcTable* F)//TODO:need a type! and V   and Node Dec
{
	Node* VarDec=Dec->fir_child;
	varEntry* varDecEntry= VarDecHandler(type,V,VarDec,0);

	if(varDecEntry!=NULL)//==NULL说明重复定义了
		InsertVarDec(varDecEntry , V , VarDec);
	
	if(VarDec->right_nei!=NULL){
		//TODO: vardec=exp
		Node* Exp=VarDec->right_nei->right_nei;
		Type expResultType= ExpHandler(Exp,V,F);
		int comRes=CompareType(expResultType,varDecEntry->type);
		if(!comRes)
		{
			PrintWrongInfo(ASSIGNMISMATCH,VarDec->lineno,"none");
		}
	}
	
}

void InsertVarDec(varEntry* varDecEntry,varTable* V,Node* VarDec)
{
	//varEntry* varDecEntry= VarDecHandler(type,V,VarDec);
	//Type varDecType=varDecEntry->type;
	char* name = strdup(varDecEntry->name);

	varEntry* searchVar = searchVarTable(name,V);
	if(searchVar!=NULL)
	{
		int lineno=VarDec->lineno;

		if(varDecEntry->type->kind==STRUCTUREDEF)
		{
			PrintWrongInfo(DUPSTNAME , lineno , name);//TODO:结构体名字和域的名字重复
		}
		else
			PrintWrongInfo(REDEVAR , lineno , name);
	}
	else
	{
		insertVarTable(varDecEntry,name,V);
	}

}

varEntry* VarDecHandler(Type type,varTable* V,Node* VarDec,int forST)//TODO:need a type! and V   and Node VarDec
{
	if( strcmp(VarDec->fir_child->name , "ID") == 0 )  
		{
			char* name=
			strdup(VarDec->fir_child->token);

			varEntry* searchVar = searchVarTable(name,V);
			if(searchVar!=NULL &&(!forST))
			{
				int lineno=VarDec->lineno;
				
				PrintWrongInfo(REDEVAR , lineno , name);
				return NULL;
			}
			else//for ST:错误类型15：只关注同一结构体的域名   TODO：暂时不考虑域名和变量名重复的情况
			{
				varEntry* varDecEntry = constructVarEntry(type,name);
				//printf("type is %d   ",varDecEntry->type->kind);
				return varDecEntry;
				//printf("haha3\n");
			}
				
		}

	else //数组定义  VarDec-VarDec LB INT RB
	{
		varEntry* varDecEntry = VarDecHandler(type,V,VarDec->fir_child,forST);
		Node* intNode = VarDec->fir_child->right_nei->right_nei;
		int size=atoi(intNode->token);
		//构建数组type
		Type arrayType = (Type)malloc(sizeof(TypeEntry));
		arrayType->kind=ARRAY;
		arrayType->u.array.size=size;
		//printf("size is %d    ",size);
		arrayType->u.array.elem=varDecEntry->type;
		//printf("elem is \n");

		char* name = strdup(varDecEntry->name);

		varEntry* returnEntry = constructVarEntry(arrayType,name);
		return returnEntry;
	}

}

void StmtListHandler(Node* StmtList,varTable* V,funcTable* F,Type return_type)//V
{
	if(StmtList->fir_child==NULL)
	return;
	Node* Stmt=StmtList->fir_child;
	StmtHandler(Stmt,V,F,return_type);
	StmtList=Stmt->right_nei;
	StmtListHandler(StmtList,V,F,return_type);
}

void StmtHandler(Node* Stmt,varTable* V,funcTable* F,Type return_type)//V
{
			Node* stmtFirChild=Stmt->fir_child;
			if(strcmp(stmtFirChild->name,"Exp")==0)
			{
				Type expResultType=ExpHandler(stmtFirChild,V,F);
			}
			else if(strcmp(stmtFirChild->name,"RETURN")==0)
			{
				int lineno=stmtFirChild->lineno;
				Node* Exp=stmtFirChild->right_nei;
				Type expResultType=ExpHandler(Exp,V,F);
				int comRes=CompareType(expResultType,return_type);
				if(!comRes)
				PrintWrongInfo(RETNMISMATCH,lineno,"none");
			}
			else if(strcmp(stmtFirChild->name,"CompSt")==0)
			{
				CompstHandler(V,stmtFirChild,F,return_type);
			}
			else if((strcmp(stmtFirChild->name,"WHILE")==0) || (strcmp(stmtFirChild->name,"IF")==0))
			{
				Node* Exp=stmtFirChild->right_nei->right_nei;
				Type expResultType=ExpHandler(Exp,V,F);
				if(expResultType==NULL || expResultType->kind!=BASIC || expResultType->u.basic!=basic_type_int)
				{
					//TODO:假设2
				}
				Node* Stmt=Exp->right_nei->right_nei;
				StmtHandler(Stmt,V,F,return_type);
				if(Stmt->right_nei!=NULL)//是else
				{
					if ( strcmp(Stmt->right_nei->name,"ELSE")==0 )
					{
						Node* elseStmt=Stmt->right_nei->right_nei;
						StmtHandler(elseStmt,V,F,return_type);
					}
				}
			}		
}
	




Type ExpHandler(Node* Exp,varTable* V,funcTable* F)//18
{
	Node* expFirChild=Exp->fir_child;
	int lineno=Exp->lineno;

	//---------------------------12 13 16---------------------------
	if(strcmp(expFirChild->name,"ID")==0)
	{
		char* name=strdup(expFirChild->token);
		Node* secondChild=expFirChild->right_nei;

		if(secondChild==NULL)//16:exp->ID 
		{
			varEntry* searchVar = searchVarTable(name,V);
			if(searchVar==NULL)
			{
				PrintWrongInfo(UNDEVAR,lineno,name);
				return NULL;//undefined
			}
			else
			{
				//printf("var type is %d\n",searchVar->type->u.basic);
				return searchVar->type;
			}
		}

		else if(strcmp(secondChild->name,"LP")==0)//12 13
		{
			int callLineno=expFirChild->lineno;
			funcEntry* searchFunc=searchFuncTable(name,F);
			//printf("SERACHFUNCOK\n");
			if(searchFunc==NULL)
			{
				varEntry* searchVar=searchVarTable(name,V);
				if(searchVar!=NULL)
				PrintWrongInfo(NOTFUNC,lineno,name);
				else
				PrintWrongInfo(UNDEFUNC,lineno,name);
				return NULL;
			}

			if(strcmp(secondChild->right_nei->name,"RP")==0)//13 没有形参 
			{
				if(searchFunc->param_num!=0)
				{
					PrintWrongInfo9(searchFunc,NULL,V,F,callLineno);
					return NULL;
				}
				return searchFunc->return_type;
			}
			else//12 说明有形参
			{
				Node* Args=secondChild->right_nei;
				if(searchFunc->param_num==0)//没有实参
				{
					PrintWrongInfo9(searchFunc,Args,V,F,callLineno);
					return NULL;
				}
				else
				{
					//printf("hey param_num: %d\n",searchFunc->param_num);
					int notMatch=ArgsHandler(0,Args,searchFunc,V,F);// 至少一个 形参 和 实参
					if(notMatch)
					{
						PrintWrongInfo9(searchFunc,Args,V,F,callLineno);
						return NULL;
					}	
					return searchFunc->return_type;
				}
			}
		}
	}
	//---------------------------17 18---------------------------
	else if(strcmp(expFirChild->name,"INT")==0||strcmp(expFirChild->name,"FLOAT")==0)
	{
		//printf("a val\n");
		Type return_type = (Type)malloc(sizeof(TypeEntry));
		return_type->kind=BASIC;
		if(strcmp(expFirChild->name,"INT")==0)   return_type->u.basic=basic_type_int;
		else if(strcmp(expFirChild->name,"FLOAT")==0)	return_type->u.basic=basic_type_float;

		return return_type;
	}
	//---------------------------9 10 11---------------------------
	else if((strcmp(expFirChild->name,"LP")==0)||(strcmp(expFirChild->name,"MINUS")==0)||(strcmp(expFirChild->name,"NOT")==0))
	{
		Type return_expType= ExpHandler(expFirChild->right_nei,V,F);
		return return_expType;
	}
	//---------------------------1～8  14/15---------------------------
	else if(strcmp(expFirChild->name,"Exp")==0)
	{
		Type exp1_ResultType= ExpHandler(expFirChild,V,F);
		Node* expSecondChild=expFirChild->right_nei;
		//1～8
		if(strcmp(expSecondChild->name,"ASSIGNOP")==0||strcmp(expSecondChild->name,"AND")==0||
		strcmp(expSecondChild->name,"OR")==0||strcmp(expSecondChild->name,"RELOP")==0||
		strcmp(expSecondChild->name,"PLUS")==0||strcmp(expSecondChild->name,"MINUS")==0||
		strcmp(expSecondChild->name,"STAR")==0||strcmp(expSecondChild->name,"DIV")==0 )
		{
			if(strcmp(expSecondChild->name,"ASSIGNOP")==0)//检查左值错误
			{
				int leftWrong=1;
				if((strcmp(expFirChild->fir_child->name,"ID")==0) &&  expFirChild->fir_child->right_nei==NULL)
				leftWrong=0;
				else if(strcmp(expFirChild->fir_child->name,"Exp")==0)
				{
					if((strcmp(expFirChild->fir_child->right_nei->name,"LB")==0)||strcmp(expFirChild->fir_child->name,"DOT")==0)
					{
						leftWrong=0;
					}
				}
				if(leftWrong)
				PrintWrongInfo(LEFTNOTVAR,lineno,"none");
			}

			Node* Exp2=expSecondChild->right_nei;
			Type exp2_ResultType= ExpHandler(Exp2,V,F);

			//printf("operand is %s\n",expSecondChild->name);
			int comRes = CompareType(exp1_ResultType,exp2_ResultType);
			
			//printf("compare is %d\n",comRes);
			if(comRes==0)
			{
				if(strcmp(expSecondChild->name,"ASSIGNOP")==0)
					PrintWrongInfo(ASSIGNMISMATCH,lineno,"none");
				else
					PrintWrongInfo(OPMISMATCH,lineno,"none");
			}
			else//compare返回1(假设1已被考虑)      但是考虑一些特殊情况
			{
				if(exp1_ResultType==NULL||exp2_ResultType==NULL)
				{
					//不处理
				}
				//除了赋值语句外 其他(加减乘除 relop and or)都只能是int/float
				//假设1 int和float不能相互赋值
				else if(exp1_ResultType->kind!=BASIC && (strcmp(expSecondChild->name,"ASSIGNOP")!=0))
					PrintWrongInfo(OPMISMATCH,lineno,"none");
				///and or 只能是int
				else if(strcmp(expSecondChild->name,"AND")==0||strcmp(expSecondChild->name,"OR")==0)
				{
					if(exp1_ResultType->u.basic!=basic_type_int)
						PrintWrongInfo(OPMISMATCH,lineno,"none");
				}
			}
			return exp1_ResultType;
		}

		else if(strcmp(expSecondChild->name,"LB")==0)
		{
			Node* shouldBeID=expFirChild->fir_child;//TODO: 此处有bug，但是在只有1维数组的情况下没事
			if(exp1_ResultType->kind != ARRAY)
			{
				char* name=strdup(shouldBeID->token);
				PrintWrongInfo(NOTARRAY,lineno,name);
				return NULL;//16:10
			}
			else
			{
				Node* ExpInLBRB = expSecondChild->right_nei;
				Type expin_ResultType= ExpHandler(ExpInLBRB,V,F);
				//char* nameOfExpIn=strdup()
				if(expin_ResultType->kind!=BASIC || expin_ResultType->u.basic!=basic_type_int)
				{
					//PrintWrongInfo(NOTINT,lineno,"none");//说明文字
					char* expName=strdup( LinkName(ExpInLBRB,NULL) );
					printf("Error type 12 at Line %d: \"%s\" is not an integer.\n" , lineno ,expName) ;
					return NULL;
				}
				else 
				{
					return exp1_ResultType->u.array.elem;
				}
			}
		}
		else if(strcmp(expSecondChild->name,"DOT")==0)
		{
			if(exp1_ResultType->kind != STRUCTUREVAR)
			{
				PrintWrongInfo(NOTSTRUCT,expFirChild->lineno,".");
				return NULL;
			}
			else
			{

				Node* dotID=expSecondChild->right_nei;
				char* fieldName=strdup(dotID->token);

				char* stTypeName=strdup(exp1_ResultType->u.stTypeName );
				varEntry* stDef = searchVarTable(stTypeName,V);
				FieldList fList=stDef->type->u.structure;

				while(fList->tail!=NULL)
				{
					if(strcmp(fList->tail->name,fieldName)!=0)
						fList=fList->tail;
					else 
						return fList->tail->type;
				}
				PrintWrongInfo(NOFIELD,dotID->lineno,fieldName);
				return NULL;		
			}			
		}	
	}
}
char* LinkArgsName(Node* Args, char* name_s1,varTable* V,funcTable* F)
{
	Node* Exp=Args->fir_child;
	Type expResultType=ExpHandler(Exp,V,F);
	char* name_s2=strdup(getTypeName(expResultType));
	name_s1=strdup(join(name_s1,name_s2));
	if(Exp->right_nei!=NULL)
	{
		name_s1=strdup(join(name_s1,", "));
		Args=Exp->right_nei->right_nei;
		name_s1=strdup(LinkArgsName(Args,name_s1,V,F));
	}
	return name_s1;
}

char* LinkName(Node* cur_node, char* name_s1)
{
	if(cur_node->fir_child==NULL)
	{
		char* name_s2=strdup(cur_node->token);
		return join(name_s1,name_s2);
	}

	else
	{
		cur_node=cur_node->fir_child;
		while(cur_node!=NULL)
		{
			name_s1=strdup(LinkName(cur_node,name_s1));//emmmmmm
			cur_node=cur_node->right_nei;
		}	
		return name_s1;
	}

}

char* join(char *s1, char *s2)
{
	if(s1==NULL)
	{return s2;}
	else
	{
		char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
		//in real code you would check for errors in malloc here
		if (result == NULL) exit (1);
	
		strcpy(result, s1);
		strcat(result, s2);
		//printf("result is %s\n",result);
		return result;
	}
}


int VarListHandler(Node* VarList,varTable* V,TypeList param_types)
{
	//先求参数个数
	int param_num=1;//attention
	Node* cur_node=VarList->fir_child;
	while(cur_node->right_nei!=NULL)
	{
		param_num++;
		cur_node=cur_node->right_nei->right_nei->fir_child;
	}
	//printf("param num is %d\n",param_num);


	Node* ParamDec=VarList->fir_child;
	ParamDecHandler(ParamDec,V,param_types);	
	while(ParamDec->right_nei!=NULL)
	{
		ParamDec=ParamDec->right_nei->right_nei->fir_child;
		ParamDecHandler(ParamDec,V,param_types);	
	}
	return param_num;
}


void ParamDecHandler(Node* ParamDec,varTable* V,TypeList param_types)
{
	Node* specifier=ParamDec->fir_child;
	Type type=getType(specifier,V);
	if(type!=NULL)//若为空   说明是未定义的结构体 就不往符号表里添了
	{
		Node* VarDec=specifier->right_nei;
		varEntry* paramDecEntry = VarDecHandler(type , V , VarDec , 0);
		if(paramDecEntry!=NULL)//==NULL说明重复定义了
			InsertVarDec(paramDecEntry , V , VarDec);
	}

	TypeList p=param_types;
	while(p->next!=NULL)
	{
		p=p->next;
	}
	TypeList s=(TypeList)malloc(sizeof(TLEntry));
	s->next=NULL;
	s->type=type;

	p->next=s;
}
int ArgsHandler(int i,Node* Args,funcEntry* searchFunc,varTable* V,funcTable* F)
{
	Node* Exp=Args->fir_child;
	Type expResultType=ExpHandler(Exp,V,F);

	TypeList p=searchFunc->param_types->next;
	int comRes=CompareType(expResultType,p->type);
	if(!comRes)return 1;//当前参数类型不匹配

	//当前参数的类型匹配了 
	while(p->next!=NULL)
	{
		if(Exp->right_nei==NULL)return 1;//实参个数小于形参
		p=p->next;
		
		comRes=CompareType(expResultType,p->type);
		if(!comRes)return 1;//类型不匹配
	}	
	if(Exp->right_nei!=NULL)return 1;//实参个数大于形参

	return 0;
		
}

int CompareType(Type type1,Type type2)
{
	if(type1==NULL||type2==NULL)
	{
		return 1;//    not need to compare (type == NULL说明前面已经发生过 某个 最主要 的错误)
	}
	else if(type1->kind != type2->kind)
	{
		return 0;
	}

	else if(type1->kind==BASIC)
	{
		return (type1->u.basic == type2->u.basic);
	}

	else if(type1->kind==ARRAY)
	{
		int comRes = CompareType(type1->u.array.elem,type2->u.array.elem);
		return comRes;
	}

	else if(type1->kind==STRUCTUREVAR)
	{
		//名等价---
		int comRes=!strcmp(type1->u.stTypeName,type2->u.stTypeName);
		return comRes;
	}

	else if(type1->kind==STRUCTUREDEF)//似乎不会出现这种情况
	{
		return 2;//
	}
}

void PrintWrongInfo9(funcEntry* searchFunc,Node* Args,varTable* V,funcTable* F,int lineno)
{
	int i=0;
	char* s1_name=NULL;
	char* s2_name=NULL;
	TypeList p = searchFunc->param_types->next;

	if(p==NULL)
	s1_name=strdup(" ");

	while(p!=NULL)
	{
		s2_name=strdup( getTypeName(p->type) );
		s1_name=strdup(  join(s1_name,s2_name)  );
		if((i+2)<=searchFunc->param_num)
			s1_name=strdup(  join(s1_name,", ")  );
		i++;
		p=p->next;
	}


	if(Args!=NULL)
	{
		char* argsName=strdup( LinkArgsName(Args,NULL,V,F) );
		printf("Error type 9 at Line %d: Function \"%s(%s)\" is not applicable for arguments\"(%s)\".\n",lineno,searchFunc->name,s1_name,argsName);
	}
	else
	{
		printf("Error type 9 at Line %d: Function \"%s(%s)\" is not applicable for arguments\"( ))\".\n",lineno,searchFunc->name,s1_name);
	}

}
char* getTypeName(Type type)
{
	switch (type->kind)
	{
	case BASIC:
		{
			if(type->u.basic==basic_type_int)return "int";
			else return "float";
		}
		break;
	case ARRAY:
		{
			return "array";//TODO:
		}
		break;
	case STRUCTUREVAR:
		{
			return join("struct ",type->u.stTypeName);//TODO:
		}
		break;

	default:
	return "em";//TODO:其实不会出现
		break;
	}
}
void ExtDecListHandler(Type type,Node* ExtDecList,varTable* V)
{
	Node* VarDec=ExtDecList->fir_child;
	varEntry* varDecEntry= VarDecHandler(type,V,VarDec,0);
	if(varDecEntry!=NULL)//==NULL说明重复定义了
		InsertVarDec(varDecEntry , V , VarDec);
	if(VarDec->right_nei!=NULL)
	{
		ExtDecList=VarDec->right_nei->right_nei;
		ExtDecListHandler(type,ExtDecList,V);
	}
}