#ifndef _SEMA_H_
#define _SEMA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"


#define BUCKETCOUNT 100
#define basic_type_int 1
#define basic_type_float 2

enum ErrorType
{ 
    UNDEVAR=1, UNDEFUNC, REDEVAR , REDEFUNC , 
    ASSIGNMISMATCH,LEFTNOTVAR,OPMISMATCH,RETNMISMATCH,
    ARGNUM , NOTARRAY , NOTFUNC , NOTINT , NOTSTRUCT,
    NOFIELD , REDEFIELD ,DUPSTNAME , UNDESTRUCT
};

//---------------------------------------------------------------------
typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct Type_  TypeEntry;//stupid
typedef struct FieldList_  FEntry;//stupid

typedef struct TypeList_* TypeList;
typedef struct TypeList_ TLEntry;

struct Type_
{
    enum { BASIC, ARRAY, STRUCTUREDEF , STRUCTUREVAR} kind;
    union
        {
        // 基本类型
        int basic;
        // 数组类型信息包括元素类型与数组大小构成
        struct Array{ Type elem; int size; } array;
        
        // 结构体类型信息是一个链表
        FieldList structure;
        
        char* stTypeName;
        } u;
 };

 struct FieldList_
 {
    char* name; // 域的名字
    Type type; // 域的类型
    FieldList tail; // 下一个域
 };

 struct TypeList_
 {
    Type type; // 形参类型
    TypeList next; // 下一个域
 }; 
//---------------------------------------------------------------------





//---------------------------------------------------------------------
typedef struct varEntry{//include 局部变量 结构体变量      函数形参？
	char* name;
    Type type;//a pointer points to Type_
	struct varEntry *next;
}varEntry;

typedef struct funcEntry{
    char* name;
    Type return_type;

    int param_num;
    TypeList param_types;//the base addr of pointer array --hold the type(a pointer) of param

 	struct funcEntry *next;
}funcEntry;

typedef struct structDefEntry{  //结构体定义 
    char* name;//结构体名称
    Type type;//类型
 	struct structDefEntry *next;
}structDefEntry;

typedef struct structVarEntry{  //结构体变量 类型名等价
    char* name;//结构体变量名
    structDefEntry* structType;//指向结构体定义
    //Type type;
 	struct structVarEntry *next;
}structVarEntry;
//---------------------------------------------------------------------






//---------------------------------------------------------------------
typedef struct varTable
{
    varEntry bucket[BUCKETCOUNT];  
    //structVarEntry stvBucket[BUCKETCOUNT];
    //structDefEntry stdBucket[BUCKETCOUNT];
}varTable;

typedef struct funcTable
{
    funcEntry bucket[BUCKETCOUNT];  
}funcTable;

// typedef struct structDefTable
// {
//     structDefEntry bucket[BUCKETCOUNT];  
// }structDefTable;

// typedef struct structVarTable
// {
//     structVarEntry bucket[BUCKETCOUNT];  
// }structVarTable;
//---------------------------------------------------------------------


unsigned int hash_pjw(char* name);

void initTable(varTable* V , funcTable* F );


void insertVarTable(varEntry* s,char* name,varTable* t);
varEntry* searchVarTable(char* name,varTable* t);

void insertFuncTable(char* name,funcTable* t,Type return_type,int param_num,TypeList param_types);
funcEntry* searchFuncTable(char* name,funcTable* t);



void semaAnaly(funcTable* F,varTable* V,Node* parser_node);
void ExtDefListHandler(Node* ExtDefList,funcTable* F,varTable* V);
void structDefineHandler(Node* StructSpecifier,funcTable* F,varTable* V);
Type getType(Node* Specifier,varTable* V);

void FunDecHandler(Type return_type,Node* FuncDec,funcTable* F,varTable* V);

void CompstHandler(varTable* V,Node* Compst,funcTable* F,Type return_type);
void DefListHandler(varTable* V,Node* DefList,funcTable* F);
void DecListHandler(Type type,varTable* V,Node* DecList,funcTable* F);
void DecHandler(Type type,varTable* V,Node* Dec,funcTable* F);
varEntry* VarDecHandler(Type type,varTable* V,Node* VarDec,int forST);
void StmtListHandler(Node* StmtList,varTable* V,funcTable* F,Type return_type);
void StmtHandler(Node* Stmt,varTable* V,funcTable* F,Type return_type);//V
Type ExpHandler(Node* Exp,varTable* V,funcTable* F);

int VarListHandler(Node* VarList,varTable* V,TypeList param_types);
void ParamDecHandler(Node* ParamDec,varTable* V,TypeList param_types);
void ExtDecListHandler(Type type,Node* ExtDecList,varTable* V);



void PrintWrongInfo(enum ErrorType wrongType,int lineno,char* name);

int CompareType(Type type1,Type type2);
void InsertVarDec(varEntry* varDecEntry,varTable* V,Node* VarDec);
varEntry* constructVarEntry(Type type,char* name);

FieldList stFieldHandler(Node* DefList,varTable* V);
void VarDecForST(Type type,varTable* V,Node* VarDec,FieldList FL);

char* join(char *s1, char *s2);
char* LinkName(Node* cur_node, char* name_s1);
int ArgsHandler(int i,Node* Args,funcEntry* searchFunc,varTable* V,funcTable* F);

char* LinkArgsName(Node* Args, char* name_s1,varTable* V,funcTable* F);
char* getTypeName(Type type);

void PrintWrongInfo9(funcEntry* searchFunc,Node* Args,varTable* V,funcTable* F,int lineno);
#endif

