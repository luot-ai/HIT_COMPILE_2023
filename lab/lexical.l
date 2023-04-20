%option yylineno
%{
#include "tree.h"
	#define YYSTYPE Node*
    #include "syntax.tab.h" 
    extern int token_error;
    
    
    //extern id_num;
    
    
    //int yylineno=1;//for 多个文件
    int yycolumn = 1;
	#define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;
        
%}

%option yylineno

DIGIT   [0-9]
DIGIT_O [0-7]
DIGIT_H [0-9A-Fa-f]
LETTER  [a-zA-Z]


INT     (0|[1-9]{DIGIT}*)|(0{DIGIT_O}*)|(0x{DIGIT_H}*) 
FLOAT   ({DIGIT}+"."{DIGIT}+)|(({DIGIT}*\.{DIGIT}+|{DIGIT}+\.)[Ee][+-]?{DIGIT}+) 
ID      ({LETTER}|_)({LETTER}|_|{DIGIT})*
SEMI    ;
COMMA   ,
ASSIGNOP  =
RELOP   >|<|>=|<=|==|!=
PLUS    "+"
MINUS   "-"
STAR    "*"
DIV     "/"
AND     "&&"
OR      "||"
DOT     "."
NOT     "!"
TYPE    int|float
LP      "("
RP      ")"
LB      "["
RB      "]"
LC      "{"
RC      "}"
STRUCT  struct
RETURN  return
IF      if
ELSE    else
WHILE   while
OMIT	[ \t]
ENTER 	\n




%%
{STRUCT}    {
		yylval=create_parser_node("STRUCT",yylloc.first_line,0,yytext,NULL,NULL);
 		return STRUCT; 
		 	}
{RETURN}    {
		yylval=create_parser_node("RETURN",yylloc.first_line,0,yytext,NULL,NULL);
 		return RETURN; 
		 	}
{IF}        {
		yylval=create_parser_node("IF",yylloc.first_line,0,yytext,NULL,NULL);
 		return IF; 
		 	}
{ELSE}      {
		yylval=create_parser_node("ELSE",yylloc.first_line,0,yytext,NULL,NULL);
 		return ELSE; 
		 	}
{WHILE}     {
		yylval=create_parser_node("WHILE",yylloc.first_line,0,yytext,NULL,NULL);
 		return WHILE; 
		 	}

{TYPE}      {
		yylval=create_parser_node("TYPE",yylloc.first_line,2,yytext,NULL,NULL);
 		return TYPE; 
		 	}

{INT}       {
		yylval=create_parser_node("INT",yylloc.first_line,3,yytext,NULL,NULL);
 		return INT; 
		 	}
{FLOAT}     {
		yylval=create_parser_node("FLOAT",yylloc.first_line,4,yytext,NULL,NULL);
 		return FLOAT; 
		 	}
{ID}        {
		yylval=create_parser_node("ID",yylloc.first_line,1,yytext,NULL,NULL);
		//id_num++;
 		return ID; 
		 	}
{SEMI}      {
		yylval=create_parser_node("SEMI",yylloc.first_line,0,yytext,NULL,NULL);
 		return SEMI; 
		 	}
{COMMA}     {
		yylval=create_parser_node("COMMA",yylloc.first_line,0,yytext,NULL,NULL);
 		return COMMA; 
		 	}
{ASSIGNOP}  {
		yylval=create_parser_node("ASSIGNOP",yylloc.first_line,0,yytext,NULL,NULL);
 		return ASSIGNOP; 
		 	}
{RELOP}     {
		yylval=create_parser_node("RELOP",yylloc.first_line,0,yytext,NULL,NULL);
 		return RELOP; 
		 	}
{PLUS}      {
		yylval=create_parser_node("PLUS",yylloc.first_line,0,yytext,NULL,NULL);
 		return PLUS; 
		 	}
{MINUS}     {
		yylval=create_parser_node("MINUS",yylloc.first_line,0,yytext,NULL,NULL);
 		return MINUS; 
		 	}
{STAR}      {
		yylval=create_parser_node("STAR",yylloc.first_line,0,yytext,NULL,NULL);
 		return STAR; 
		 	}
{DIV}       {
		yylval=create_parser_node("DIV",yylloc.first_line,0,yytext,NULL,NULL);
 		return DIV; 
		 	}
{AND}       {
		yylval=create_parser_node("AND",yylloc.first_line,0,yytext,NULL,NULL);
 		return AND; 
		 	}
{OR}        {
		yylval=create_parser_node("OR",yylloc.first_line,0,yytext,NULL,NULL);
 		return OR; 
		 	}
{DOT}       {
		yylval=create_parser_node("DOT",yylloc.first_line,0,yytext,NULL,NULL);
 		return DOT; 
		 	}
{NOT}       {
		yylval=create_parser_node("NOT",yylloc.first_line,0,yytext,NULL,NULL);
 		return NOT; 
		 	}
{LP}        {
		yylval=create_parser_node("LP",yylloc.first_line,0,yytext,NULL,NULL);
 		return LP; 
		 	}
{RP}        {
		yylval=create_parser_node("RP",yylloc.first_line,0,yytext,NULL,NULL);
 		return RP; 
		 	}
{LB}        {
		yylval=create_parser_node("LB",yylloc.first_line,0,yytext,NULL,NULL);
 		return LB; 
		 	}
{RB}        {
		yylval=create_parser_node("RB",yylloc.first_line,0,yytext,NULL,NULL);
 		return RB; 
		 	}
{LC}        {
		yylval=create_parser_node("LC",yylloc.first_line,0,yytext,NULL,NULL);
 		return LC; 
		 	}
{RC}        {
		yylval=create_parser_node("RC",yylloc.first_line,0,yytext,NULL,NULL);
 		return RC; 
		 	}
{OMIT}      {}
{ENTER}     { yycolumn = 1; }
.           {token_error++;printf("Error type A at Line %d: Mysterious character \"%s\" \n",yylineno,yytext);}
 %%

