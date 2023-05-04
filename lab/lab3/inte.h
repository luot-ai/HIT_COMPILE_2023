#ifndef _INTE_H_
#define _INTE_H_
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tree.h"
#include "sema.h"

typedef struct Operand_ *Operand;
typedef struct Operands_ *Operands;
typedef struct InterCode_ *InterCode;
typedef struct InterCodes_ *InterCodes;

// int tmpNumT;
// int tmpNumLabel;
// InterCodes tail; // 每条中间代码生成时，需要与tail进行连接

enum opKind
{
    VARIABLE, // 数组名 和 ldst 无关
    RELOP,
    FUNCTION,

    CONSTANT,//#
    SIZE,//DEC

    TEMPVAR,//tx
    LABEL//labelx
};

enum icKind
{
    LABELDEF, // 1
    FUNCDEF,  // 1

    ASSIGN, // 2
    GETADDR, //2
    LOAD,   // 2                           //只有对数组某元素访问才涉及到ldst，对于数组名(普通变量)是按照assign处理的
    STORE,  // 2

    ADD, // 3
    SUB,
    MUL,
    DIV,

    GOTO,   // 1
    IFGOTO, // 4

    RETURN,//1
    DEC,//2******************
    ARG,//1
    CALL,//2*****************
    PARAM,//1
    READ,//1
    WRITE//1

};

struct Operand_
{
    enum opKind kind;
    union
    {
        // TEMPVAR(tx)  LABEL(labelx)
        char *name; // VARIABLE(普通变量及数组名) RELOP(关系表达式) FUNCTION(函数名)
        // CONSTANT(#x)

        int size;
    } u;
};

struct Operands_
{
    Operand op;
    struct Operands_ *next;
};

struct InterCode_
{
    enum icKind kind;
    union
    {
        struct
        {
            Operand op1, op2;
        } twoOps;
        struct
        {
            Operand result, op1, op2;
        } binop;
        struct
        {
            Operand op1;
        } oneop;
        struct
        {
            Operand op1, op, op2, label_true;
        } ifgoto;
    } u;
};

struct InterCodes_
{
    InterCode code;
    struct InterCodes_ *prev, *next;
};

void connectCodes(InterCodes genCode);
Operand newOpWithName(enum opKind kind, char *name); // 传参时需要 已定义变量的名字
Operand newTemp(enum opKind kind, varTable *V); // 传参时 无需 已定义变量的名字
Operand newVal(enum opKind kind,int value);
InterCodes genInteCode_basicPart(enum icKind kind);
void genInteCode_1op(enum icKind kind, Operand op1);
void genInteCode_2ops(enum icKind kind, Operand op1, Operand op2);
void genInteCode_3ops(enum icKind kind, Operand result, Operand op1, Operand op2);
void genInteCode_4ops(enum icKind kind, Operand op1, Operand op, Operand op2, Operand label_true);
InterCodes inteGen(funcTable *F, varTable *V, Node *Program);
void translate_ExtDefList(funcTable *F, varTable *V, Node *ExtDefList);
void translate_ExtDecList(Node *ExtDecList, funcTable *F, varTable *V);
void translate_VarDec(Node *VarDec, funcTable *F, varTable *V);
void translate_FunDec(Node *FunDec, funcTable *F, varTable *V);//FunDec → ID LP VarList RP | ID LP RP
void translate_VarList(Node* VarList, funcTable *F, varTable *V);//VarList → ParamDec COMMA VarList | ParamDec
void translate_ParamDec(Node* ParamDec, funcTable *F, varTable *V);//ParamDec → Specifier VarDec
void translate_CompSt(Node *Compst, funcTable *F, varTable *V);
void translate_DefList(Node *DefList, funcTable *F, varTable *V);//DefList → Def DefList | 空
void translate_DecList(Node *DecList, funcTable *F, varTable *V);//DecList → Dec | Dec COMMA DecList
void translate_Dec(Node* Dec,funcTable *F, varTable *V);//Dec → VarDec | VarDec ASSIGNOP Exp
void translate_StmtList(Node *StmtList, funcTable *F, varTable *V);
void translate_Stmt(Node *Stmt, funcTable *F, varTable *V);
void translate_Exp(Node *Exp, funcTable *F, varTable *V, Operand place);
Operand translate_ID(Node *ID, funcTable *F, varTable *V);
Operand translate_Array(Node *Exp1, Node *Exp2, funcTable *F, varTable *V); // exp1 lb exp2 rb
void translate_Cond(Node *Exp, Operand label_true, Operand label_false, funcTable *F, varTable *V);
void translate_Args(Node *Args, funcTable *F, varTable *V, Operands arg_list_tail);
Operands connectArgsList(Operands arg_list_tail, Operand newOp);
void inte2File(InterCodes head,FILE* out);




#endif