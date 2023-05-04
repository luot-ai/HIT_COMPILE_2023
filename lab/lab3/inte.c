#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tree.h"
#include "sema.h"
#include "inte.h"

int tmpNumT=1;
int tmpNumLabel=1;
InterCodes tail; // 每条中间代码生成时，需要与tail进行连接

void connectCodes(InterCodes genCode)
{
    tail->next = genCode;
    //printf("connect a inteCode,kind is %d\n", genCode->code->kind);
    genCode->prev = tail;
    genCode->next = NULL;
    tail = genCode;
}
Operand newOpWithName(enum opKind kind, char *name) // 传参时需要 已定义变量的名字
{
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = kind;
    op->u.name = name;
    return op;
}
Operand newTemp(enum opKind kind, varTable *V) // 传参时 无需 已定义变量的名字
{
    Operand tmp = (Operand)malloc(sizeof(struct Operand_));
    tmp->kind = kind;
    char *str = (char *)malloc(sizeof(char) * 20);
    // 避免 临时变量 和 已定义变量重名
    if (kind == TEMPVAR)
    {
        //printf("temp var num is %d\n", tmpNumT);
        sprintf(str, "t%d", tmpNumT);
        while (searchVarTable(str, V) != NULL)
        {
            tmpNumT++;
            sprintf(str, "t%d", tmpNumT);
        }
        tmpNumT++;
    }

    else if (kind == LABEL)
    {
        sprintf(str, "label%d", tmpNumLabel);
        while (searchVarTable(str, V) != NULL)
        {
            tmpNumLabel++;
            sprintf(str, "label%d", tmpNumLabel);
        }
        tmpNumLabel++;
    }

    tmp->u.name = str;
    return tmp;
}
Operand newVal(enum opKind kind, int value) // CONSTANT:#value   SIZE:value
{
    Operand val = (Operand)malloc(sizeof(struct Operand_));
    val->kind = kind;

    if (kind == CONSTANT)
    {
        char *str = (char *)malloc(sizeof(char) * 20);
        sprintf(str, "#%d", value);
        val->u.name = str;
        return val;
    }
    else if(kind==SIZE)
    {
        val->u.size=value;
    }
}
InterCodes genInteCode_basicPart(enum icKind kind)
{
    InterCodes genCode = (InterCodes)malloc(sizeof(struct InterCodes_));
    genCode->code = (InterCode)malloc(sizeof(struct InterCode_));

    genCode->code->kind = kind;
    return genCode;
}
void genInteCode_1op(enum icKind kind, Operand op1)
{
    InterCodes genCode = genInteCode_basicPart(kind);
    genCode->code->u.oneop.op1 = op1;

    connectCodes(genCode);
}
void genInteCode_2ops(enum icKind kind, Operand op1, Operand op2)
{
    if (op1 == NULL) // TODO:
        return;

    InterCodes genCode = genInteCode_basicPart(kind);
    genCode->code->u.twoOps.op1 = op1;
    genCode->code->u.twoOps.op2 = op2;

    connectCodes(genCode);
}
void genInteCode_3ops(enum icKind kind, Operand result, Operand op1, Operand op2)
{
    InterCodes genCode = genInteCode_basicPart(kind);
    genCode->code->u.binop.op1 = op1;
    genCode->code->u.binop.op2 = op2;
    genCode->code->u.binop.result = result;

    connectCodes(genCode);
}
void genInteCode_4ops(enum icKind kind, Operand op1, Operand op, Operand op2, Operand label_true)
{
    InterCodes genCode = genInteCode_basicPart(kind);
    genCode->code->u.ifgoto.op1 = op1;
    genCode->code->u.ifgoto.op = op;
    genCode->code->u.ifgoto.op2 = op2;
    genCode->code->u.ifgoto.label_true = label_true;

    connectCodes(genCode);
}
InterCodes inteGen(funcTable *F, varTable *V, Node *Program)
{
    InterCodes head = (InterCodes)malloc(sizeof(struct InterCodes_));
    head->code = (InterCode)malloc(sizeof(struct InterCode_));
    head->code = NULL;
    head->prev = NULL;
    head->next = NULL;

    tail = head;

    Node *ExtDefList = Program->fir_child;
    translate_ExtDefList(F, V, ExtDefList);

    return head;
}
void translate_ExtDefList(funcTable *F, varTable *V, Node *ExtDefList)
{
    if (ExtDefList->fir_child != NULL)
    {
        Node *ExtDef = ExtDefList->fir_child;
        Node *specifier = ExtDef->fir_child;
        Node *secondChild = specifier->right_nei; // 产生式第二个语法单元

        if (strcmp(secondChild->name, "ExtDecList") == 0) // TODO:假设:没有全局变量的使用(而且Dec是用于局部变量的)
        {
            translate_ExtDecList(secondChild, F, V);
        }
        else if ((strcmp(secondChild->name, "FunDec") == 0))
        {
            Node *FunDec = secondChild;
            translate_FunDec(FunDec, F, V);
            Node *Compst = FunDec->right_nei;
            translate_CompSt(Compst, F, V);
        }
        ExtDefList = ExtDef->right_nei;
        translate_ExtDefList(F, V, ExtDefList);
    }
}
void translate_ExtDecList(Node *ExtDecList, funcTable *F, varTable *V)
{
    Node *VarDec = ExtDecList->fir_child;
    translate_VarDec(VarDec, F, V);
    if (VarDec->right_nei != NULL)
    {
        ExtDecList = VarDec->right_nei->right_nei;
        translate_ExtDecList(ExtDecList, F, V);
    }
}
void translate_VarDec(Node *VarDec, funcTable *F, varTable *V)
{
    if (strcmp(VarDec->fir_child->name, "ID") == 0)
    {
        // do nothing , 直接使用该变量即可
    }
    else // VarDec->VarDec1 LB INT RB     本次实验中默认为1维数组,也就是说Vardec1->ID
    {
        Node *ID = VarDec->fir_child->fir_child;
        Node *INT = VarDec->fir_child->right_nei->right_nei;

        Operand arrayName = newOpWithName(VARIABLE, ID->token);
        Operand size = newVal(SIZE,atoi(INT->token) * 4);
        genInteCode_2ops(DEC, arrayName, size);
    }
}
void translate_FunDec(Node *FunDec, funcTable *F, varTable *V) // FunDec → ID LP VarList RP | ID LP RP
{
    Node *ID = FunDec->fir_child;
    Operand func = newOpWithName(FUNCTION, ID->token);
    genInteCode_1op(FUNCDEF, func);

    Node *LP = ID->right_nei;
    if (strcmp(LP->right_nei->name, "VarList") == 0)
    {
        translate_VarList(LP->right_nei, F, V);
    }
}
void translate_VarList(Node *VarList, funcTable *F, varTable *V) // VarList → ParamDec COMMA VarList | ParamDec
{
    Node *ParamDec = VarList->fir_child;
    translate_ParamDec(ParamDec, F, V);
    while (ParamDec->right_nei != NULL)
    {
        ParamDec = ParamDec->right_nei->right_nei->fir_child;
        translate_ParamDec(ParamDec, F, V);
    }
}
// 数组不会作为参数传入函数中
void translate_ParamDec(Node *ParamDec, funcTable *F, varTable *V) // ParamDec → Specifier VarDec
{
    Node *VarDec = ParamDec->fir_child->right_nei; // VarDec->ID
    Node *ID = VarDec->fir_child;
    Operand pa = newOpWithName(VARIABLE, ID->token);
    genInteCode_1op(PARAM, pa);
}
void translate_CompSt(Node *Compst, funcTable *F, varTable *V)
{
    Node *LC = Compst->fir_child;
    Node *DefList = LC->right_nei;
    Node *StmtList = DefList->right_nei;

    translate_DefList(DefList, F, V);
    translate_StmtList(StmtList, F, V);
}
void translate_DefList(Node *DefList, funcTable *F, varTable *V) // DefList → Def DefList | 空
{
    Node *Def = DefList->fir_child;
    if (Def != NULL)
    {
        Node *DecList = Def->fir_child->right_nei; // Def → Specifier DecList SEMI
        translate_DecList(DecList, F, V);

        translate_DefList(Def->right_nei, F, V);
    }
}
void translate_DecList(Node *DecList, funcTable *F, varTable *V) // DecList → Dec | Dec COMMA DecList
{
    Node *Dec = DecList->fir_child;
    translate_Dec(Dec, F, V);
    while (Dec->right_nei != NULL)
    {
        Dec = Dec->right_nei->right_nei->fir_child;
        translate_Dec(Dec, F, V);
    }
}
void translate_Dec(Node *Dec, funcTable *F, varTable *V) // Dec → VarDec | VarDec ASSIGNOP Exp
{
    Node *VarDec = Dec->fir_child;
    translate_VarDec(VarDec, F, V);

    if (VarDec->right_nei != NULL) 
    {
        char *varName;
        if (strcmp(VarDec->fir_child->name, "ID") == 0) // VarDec->ID
        {
            varName = strdup(VarDec->fir_child->token);
        }
        else // VarDec->VarDec1 LB INT RB    VarDec1->ID  
        {
            varName = strdup(VarDec->fir_child->fir_child->token);
        }
        Operand var = newOpWithName(VARIABLE, varName);
        Node *Exp = VarDec->right_nei->right_nei;
        translate_Exp(Exp, F, V, var);
    }
}
void translate_StmtList(Node *StmtList, funcTable *F, varTable *V)
{
    if (StmtList->fir_child != NULL)
    {
        Node *Stmt = StmtList->fir_child;
        translate_Stmt(Stmt, F, V);
        StmtList = Stmt->right_nei;
        translate_StmtList(StmtList, F, V);
    }
}
void translate_Stmt(Node *Stmt, funcTable *F, varTable *V)
{
    Node *stmtFirChild = Stmt->fir_child;

    if (strcmp(stmtFirChild->name, "Exp") == 0)
    {
        translate_Exp(stmtFirChild, F, V, NULL);
    }
    else if (strcmp(stmtFirChild->name, "RETURN") == 0)
    {
        Operand t1 = newTemp(TEMPVAR, V);
        //printf("temp var name is t%d\n",tmpNumT-1);
        translate_Exp(stmtFirChild->right_nei, F, V, t1);
        genInteCode_1op(RETURN, t1);
    }
    else if (strcmp(stmtFirChild->name, "CompSt") == 0)
    {
        translate_CompSt(stmtFirChild, F, V);
    }

    else if (strcmp(stmtFirChild->name, "IF") == 0)
    {
        Operand label1 = newTemp(LABEL, V);
        Operand label2 = newTemp(LABEL, V);
        Node *Exp1 = stmtFirChild->right_nei->right_nei;
        Node *Stmt1 = Exp1->right_nei->right_nei;
        translate_Cond(Exp1, label1, label2, F, V);
        genInteCode_1op(LABELDEF, label1);
        translate_Stmt(Stmt1, F, V);

        if (Stmt1->right_nei != NULL) // if else
        {
            Operand label3 = newTemp(LABEL, V);
            Node *Stmt2 = Stmt1->right_nei->right_nei;
            genInteCode_1op(GOTO, label3);
            genInteCode_1op(LABELDEF, label2);
            translate_Stmt(Stmt2, F, V);
            genInteCode_1op(LABELDEF, label3);
        }
        else // if
        {
            genInteCode_1op(LABELDEF, label2);
        }
    }

    else if (strcmp(stmtFirChild->name, "WHILE") == 0)
    {
        Operand label1 = newTemp(LABEL, V);
        Operand label2 = newTemp(LABEL, V);
        Operand label3 = newTemp(LABEL, V);
        Node *Exp1 = stmtFirChild->right_nei->right_nei;
        Node *Stmt1 = Exp1->right_nei->right_nei;
        genInteCode_1op(LABELDEF, label1);
        translate_Cond(Exp1, label2, label3, F, V);

        genInteCode_1op(LABELDEF, label2);
        translate_Stmt(Stmt1, F, V);

        genInteCode_1op(GOTO, label1);
        genInteCode_1op(LABELDEF, label3);
    }
}
// 15结构体略    18浮点常量略
// 1 assign    12/13 函数调用 单独出现时有意义(place可能为NULL)
// 默认 函数调用 除了 read/write外都有参数接受返回值？
void translate_Exp(Node *Exp, funcTable *F, varTable *V, Operand place)
{
    Node *expFirChild = Exp->fir_child;
    //---------------------------12 13 16---------------------------
    if (strcmp(expFirChild->name, "ID") == 0)
    {
        char *name = strdup(expFirChild->token);
        Node *secondChild = expFirChild->right_nei;

        if (secondChild == NULL) // 16:exp->ID
        {
            Node *ID = expFirChild;
            Operand idOperand = translate_ID(ID, F, V); // as for now 数组名(地址)/变量名 都是 VARIABLE类型
            genInteCode_2ops(ASSIGN, place, idOperand); // place := idOperand
        }

        else if (strcmp(secondChild->name, "LP") == 0) // 12 13函数调用
        {
            Node *ID = expFirChild;
            Operand func = newOpWithName(FUNCTION, ID->token);
            if (strcmp(secondChild->right_nei->name, "RP") == 0) // 13 没有参数
            {
                if (place != NULL)
                {
                    if (strcmp(ID->token, "read") == 0)
                    {
                        genInteCode_1op(READ, place);
                    }
                    else
                    {
                        genInteCode_2ops(CALL, place, func);
                    }
                }
            }
            else // 12 说明有形参
            {
                Node *Args = secondChild->right_nei;
                Operands argListHead = (Operands)malloc(sizeof(struct Operands_));
                argListHead->op = NULL;
                argListHead->next = NULL;
                translate_Args(Args, F, V, argListHead);

                if (strcmp(ID->token, "write") == 0)
                {
                    genInteCode_1op(WRITE, argListHead->next->op);
                    if (place != NULL)
                    {
                        Operand val0 = newVal(CONSTANT ,0);
                        genInteCode_2ops(ASSIGN, place, val0);
                    }
                }
                else
                {
                    Operands argListTail = argListHead;
                    while (argListTail->next != NULL)
                    {
                        genInteCode_1op(ARG, argListTail->next->op);
                        argListTail = argListTail->next;
                    }
                    if (place != NULL)
                        genInteCode_2ops(CALL, place, func);
                }
            }
        }
    }

    //---------------------------17---------------------------//浮点常量不会出现
    else if (strcmp(expFirChild->name, "INT") == 0)
    {
        Operand val = newVal(CONSTANT ,atoi(expFirChild->token));

        genInteCode_2ops(ASSIGN, place, val);
    }
    //---------------------------1～8  11 14---------------------------
    else if ((strcmp(expFirChild->name, "Exp") == 0) || (strcmp(expFirChild->name, "NOT") == 0))
    {
        Node *expSecondChild = expFirChild->right_nei;

        if (strcmp(expSecondChild->name, "ASSIGNOP") == 0) // 1  exp->exp1 assign exp2
        {
            if ((strcmp(expFirChild->fir_child->name, "ID") == 0) && expFirChild->fir_child->right_nei == NULL) // exp1->id
            {
                Operand t1 = newTemp(TEMPVAR, V);
                Node *Exp2 = expSecondChild->right_nei;
                translate_Exp(Exp2, F, V, t1);

                Node *ID = expFirChild->fir_child;
                Operand idOperand = translate_ID(ID, F, V); // as for now 数组名(地址)/变量名 都是 VARIABLE类型

                genInteCode_2ops(ASSIGN, idOperand, t1);
                if (place != NULL)
                    genInteCode_2ops(ASSIGN, place, idOperand);
            }
            else // exp->exp1 assign exp2    exp1->exp11 lb exp12 rb
            {
                Operand t1 = newTemp(TEMPVAR, V);
                Node *Exp2 = expSecondChild->right_nei;
                translate_Exp(Exp2, F, V, t1); // t1 := 某

                Node *Exp11 = expFirChild->fir_child;
                Node *Exp12 = Exp11->right_nei->right_nei;
                Operand realAddr = translate_Array(Exp11, Exp12, F, V);

                genInteCode_2ops(STORE, realAddr, t1); //*realAddr := t1
                if (place != NULL)
                    genInteCode_2ops(LOAD, place, realAddr); // place := *realAddr
            }
        }

        else if (strcmp(expSecondChild->name, "PLUS") == 0 || strcmp(expSecondChild->name, "MINUS") == 0 ||
                 strcmp(expSecondChild->name, "STAR") == 0 || strcmp(expSecondChild->name, "DIV") == 0) // 5 6 7 8
        {
            Operand t1 = newTemp(TEMPVAR, V);
            Operand t2 = newTemp(TEMPVAR, V);
            translate_Exp(expFirChild, F, V, t1);
            translate_Exp(expSecondChild->right_nei, F, V, t2);

            if (strcmp(expSecondChild->name, "PLUS") == 0)
                genInteCode_3ops(ADD, place, t1, t2);
            else if (strcmp(expSecondChild->name, "MINUS") == 0)
                genInteCode_3ops(SUB, place, t1, t2);
            else if (strcmp(expSecondChild->name, "STAR") == 0)
                genInteCode_3ops(MUL, place, t1, t2);
            else
                genInteCode_3ops(DIV, place, t1, t2);
        }
        else if (strcmp(expSecondChild->name, "AND") == 0 ||
                 strcmp(expSecondChild->name, "OR") == 0 ||
                 strcmp(expFirChild->name, "NOT") == 0 ||
                 strcmp(expSecondChild->name, "RELOP") == 0) // 2 3 4 11
        {
            Operand label1 = newTemp(LABEL, V);
            Operand label2 = newTemp(LABEL, V);
            Operand val0 = newVal(CONSTANT ,0);
            genInteCode_2ops(ASSIGN, place, val0);
            translate_Cond(Exp, label1, label2, F, V);

            genInteCode_1op(LABELDEF, label1);
            Operand val1 = newVal(CONSTANT ,1);
            genInteCode_2ops(ASSIGN, place, val1);
            genInteCode_1op(LABELDEF, label2);
        }
        else if (strcmp(expSecondChild->name, "LB") == 0) // 14数组引用 exp LB exp RB
        {
            Node *Exp1 = expFirChild;
            Node *Exp2 = Exp1->right_nei->right_nei;
            Operand realAddr = translate_Array(Exp1, Exp2, F, V);
            if (place != NULL) // 其实不会
            {
                genInteCode_2ops(LOAD, place, realAddr); // place := *realAddr
            }
        }
    }
    //---------------------------9 10  ---------------------------
    else if ((strcmp(expFirChild->name, "LP") == 0) || (strcmp(expFirChild->name, "MINUS") == 0))
    {
        if (strcmp(expFirChild->name, "MINUS") == 0)
        {
            Operand t1 = newTemp(TEMPVAR, V);
            translate_Exp(expFirChild->right_nei, F, V, t1);
            Operand val0 = newVal(CONSTANT ,0);
            genInteCode_3ops(SUB, place, val0, t1);
        }
        else// 括号表达式对括号中的Exp生成中间代码即可
        {
            translate_Exp(expFirChild->right_nei, F, V, place);
        }
    }
}
Operand translate_ID(Node *ID, funcTable *F, varTable *V)
{
    Operand var = newOpWithName(VARIABLE, ID->token);
    return var;
    // varEntry *searchVar = searchVarTable(ID->token, V);
    // if (searchVar->type->kind == BASIC)
    // {
    //     Operand var = newOpWithName(VARIABLE, ID->token);
    //     return var;
    // }
    // else if (searchVar->type->kind == ARRAY)
    // {
    //     Operand arrayBaseAddr = newOpWithName(ADDRESS, ID->token); // 数组名 即为 数组首地址
    //     return arrayBaseAddr;
    // }
}

Operand translate_Array(Node *Exp1, Node *Exp2, funcTable *F, varTable *V) // exp1 lb exp2 rb
{
    Node *ID = Exp1->fir_child; // TODO:这是一个假设

    Operand baseAddr = newOpWithName(VARIABLE, ID->token); // 数组名即为地址
    /*
        取数组名作为地址
        这里还是用了上面那个假设
        Operand baseAddr = newTemp(TEMPVAR,V);
        Operand arrayName = newOpWithName(VARIABLE,ID->token);
        genInteCode_2ops(GETADDR,baseAddr,arrayName);//baseAddr := &name
    */

    Operand index = newTemp(TEMPVAR, V);
    Operand intSize = newVal(CONSTANT ,4);
    Operand offSet = newTemp(TEMPVAR, V);

    Operand realAddr = newTemp(TEMPVAR, V);

    translate_Exp(Exp2, F, V, index);                  // index := 一个整数
    genInteCode_3ops(MUL, offSet, index, intSize);     // offset := index * #4
    genInteCode_3ops(ADD, realAddr, baseAddr, offSet); // realAddr := 数组名 + offSet
    return realAddr;
}

void translate_Cond(Node *Exp, Operand label_true, Operand label_false, funcTable *F, varTable *V)
{
    Node *ExpFirChild = Exp->fir_child;
    if (strcmp(ExpFirChild->name, "Exp") == 0)
    {
        Node *Exp1 = ExpFirChild;
        Node *ExpSecondChild = ExpFirChild->right_nei;
        Node *Exp2 = ExpSecondChild->right_nei;

        if (strcmp(ExpSecondChild->name, "RELOP") == 0)
        {
            Operand t1 = newTemp(TEMPVAR, V);
            Operand t2 = newTemp(TEMPVAR, V);
            translate_Exp(Exp1, F, V, t1);
            translate_Exp(Exp2, F, V, t2);
            Operand op = newOpWithName(RELOP, ExpSecondChild->token);
            genInteCode_4ops(IFGOTO, t1, op, t2, label_true);
            genInteCode_1op(GOTO, label_false);
        }
        else if (strcmp(ExpSecondChild->name, "AND") == 0)
        {
            Operand label1 = newTemp(LABEL, V);
            translate_Cond(Exp1, label1, label_false, F, V);
            genInteCode_1op(LABELDEF, label1);
            translate_Cond(Exp2, label_true, label_false, F, V);
        }
        else if (strcmp(ExpSecondChild->name, "OR") == 0)
        {
            Operand label1 = newTemp(LABEL, V);
            translate_Cond(Exp1, label_true, label1, F, V);
            genInteCode_1op(LABELDEF, label1);
            translate_Cond(Exp2, label_true, label_false, F, V);
        }
    }
    else if (strcmp(ExpFirChild->name, "NOT") == 0)
    {
        Node *Exp = ExpFirChild->right_nei;
        translate_Cond(Exp, label_false, label_true, F, V);
    }
    else
    {
        Operand t1 = newTemp(TEMPVAR, V);
        translate_Exp(Exp, F, V, t1);
        Operand op = newOpWithName(RELOP, "!=");
        Operand val0 = newVal(CONSTANT ,0);
        genInteCode_4ops(IFGOTO, t1, op, val0, label_true);
        genInteCode_1op(GOTO, label_false);
    }
}
void translate_Args(Node *Args, funcTable *F, varTable *V, Operands arg_list_tail)
{
    Node *Exp = Args->fir_child;
    if (Exp->right_nei == NULL) // Args → Exp
    {
        Operand t1 = newTemp(TEMPVAR, V);
        translate_Exp(Exp, F, V, t1);
        arg_list_tail = connectArgsList(arg_list_tail, t1);
    }
    else // Args → Exp COMMA Args
    {
        Operand t1 = newTemp(TEMPVAR, V);
        translate_Exp(Exp, F, V, t1);
        arg_list_tail = connectArgsList(arg_list_tail, t1);
        Node *Args1 = Exp->right_nei->right_nei;
        translate_Args(Args1, F, V, arg_list_tail);
    }
}

Operands connectArgsList(Operands arg_list_tail, Operand newOp)
{
    Operands cur_arg = (Operands)malloc(sizeof(struct Operands_));
    cur_arg->op = newOp;
    cur_arg->next = NULL;

    arg_list_tail->next = cur_arg;
    return cur_arg;
}

// 一共19种         但是x := &y暂时没用上
void inte2File(InterCodes head, FILE *out)
{
    // InterCodes writer = (InterCodes)malloc(sizeof(struct InterCodes_));
    // writer = head->next;

    InterCodes writer = head->next;
    while (writer != NULL)
    {
        InterCode code = writer->code;
        enum icKind codeKind = code->kind;

        if (codeKind == IFGOTO)
        {
            char *op1 = strdup(code->u.ifgoto.op1->u.name);
            char *relop = strdup(code->u.ifgoto.op->u.name);
            char *op2 = strdup(code->u.ifgoto.op2->u.name);
            char *label = strdup(code->u.ifgoto.label_true->u.name);
            fprintf(out, "IF %s %s %s GOTO %s\n", op1, relop, op2, label);
        }

        else if (codeKind == ADD || codeKind == SUB || codeKind == MUL || codeKind == DIV) // 不可能出现取地址这种内容
        {
            char *result = strdup(code->u.binop.result->u.name);
            char *op1 = strdup(code->u.binop.op1->u.name);
            char *op2 = strdup(code->u.binop.op2->u.name);
            switch (codeKind)
            {
            case ADD:
                fprintf(out, "%s := %s + %s\n", result, op1, op2);
                break;
            case SUB:
                fprintf(out, "%s := %s - %s\n", result, op1, op2);
                break;
            case MUL:
                fprintf(out, "%s := %s * %s\n", result, op1, op2);
                break;
            case DIV:
                fprintf(out, "%s := %s / %s\n", result, op1, op2);
                break;
            default:
                break;
            }
        }
        // 通过 代码kind 来区分不同操作，而不是操作数
        else if (codeKind == ASSIGN || codeKind == LOAD || codeKind == STORE || codeKind == CALL || codeKind == GETADDR || codeKind == DEC)
        {
            char *op1 = strdup(code->u.twoOps.op1->u.name);
            char *op2;
            if (codeKind != DEC)
                op2 = strdup(code->u.twoOps.op2->u.name);

            switch (codeKind)
            {
            case ASSIGN:
                fprintf(out, "%s := %s\n", op1, op2);
                break;
            case LOAD:
                fprintf(out, "%s := *%s\n", op1, op2);
                break;
            case STORE:
                fprintf(out, "*%s := %s\n", op1, op2);
                break;
            case CALL:
                fprintf(out, "%s := CALL %s\n", op1, op2);
                break;
            case GETADDR:
                fprintf(out, "%s := &%s\n", op1, op2);
                break;
            case DEC:
                fprintf(out, "DEC %s %d\n", op1, code->u.twoOps.op2->u.size);
                break;
            default:
                break;
            }
        }

        else
        {
            char *op = strdup(code->u.oneop.op1->u.name);
            switch (code->kind)
            {
            case LABELDEF:
                fprintf(out, "LABEL %s :\n", op);
                break;
            case FUNCDEF:
                fprintf(out, "FUNCTION %s :\n", op);
                break;
            case GOTO:
                fprintf(out, "GOTO %s\n", op);
                break;
            case RETURN:
                fprintf(out, "RETURN %s\n", op);
                break;
            case ARG:
                fprintf(out, "ARG %s\n", op);
                break;
            case PARAM:
                fprintf(out, "PARAM %s\n", op);
                break;
            case READ:
                fprintf(out, "READ %s\n", op);
                break;
            case WRITE:
                fprintf(out, "WRITE %s\n", op);
                break;
            default:
                break;
            }
        }
        writer = writer->next;
    }
}
