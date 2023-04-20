%{
    #include <stdio.h>
    #include "lex.yy.c"
    #include "tree.h"
    //#define YYSTYPE Node*
    Node* parser_tree;
    extern int syntax_error;

%}
%token INT     
%token FLOAT   
%token ID      
%token SEMI    
%token COMMA   
%token ASSIGNOP
%token RELOP   
%token PLUS    
%token MINUS   
%token STAR    
%token DIV     
%token AND     
%token OR      
%token DOT     
%token NOT     
%token TYPE    
%token LP      
%token RP      
%token LB      
%token RB      
%token LC      
%token RC      
%token STRUCT  
%token RETURN  
%token IF      
%token ELSE    
%token WHILE   
%token OMIT	
%token ENTER 

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left MINUS 
%left PLUS
%left STAR DIV
%right NOT
%left LP RP LB RB DOT

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%%
//high-level definitions
Program : ExtDefList{
	$$ = create_gra_node("Program", @$.first_line,5, $1,NULL);
    parser_tree = $$;
} ;
ExtDefList : ExtDef ExtDefList { 
	$$ = create_gra_node("ExtDefList", @$.first_line, 5, $1, NULL);
	connect_node($1,$2); 
	}
| { 
	$$ = create_gra_node("ExtDefList", @$.first_line, 5, NULL, NULL); 
	};
ExtDef : Specifier ExtDecList SEMI { 
	$$ = create_gra_node("ExtDef", @$.first_line, 5, $1, NULL);
	connect_node($1,$2);
	connect_node($2,$3);
	}
| Specifier SEMI{ 
	$$ = create_gra_node("ExtDef", @$.first_line, 5, $1, NULL);
	connect_node($1,$2); 
	}
| Specifier FunDec CompSt  { 
	$$ = create_gra_node("ExtDef", @$.first_line, 5, $1, NULL);
	connect_node($1,$2);
	connect_node($2,$3); 
	}|Specifier error SEMI{syntax_error++;};
ExtDecList : VarDec{ 
	$$ = create_gra_node("ExtDecList", @$.first_line, 5, $1, NULL); 
	}
| VarDec COMMA ExtDecList { 
	$$ = create_gra_node("ExtDecList", @$.first_line, 5, $1, NULL);
	connect_node($1,$2);
	connect_node($2,$3); 
	} ;

//specifiers
Specifier : TYPE{ 
	$$ = create_gra_node("Specifier", @$.first_line, 5, $1, NULL); 
	}
| StructSpecifier{ 
	$$ = create_gra_node("Specifier", @$.first_line, 5, $1, NULL); 
	}   ;
StructSpecifier : STRUCT OptTag LC DefList RC{ 
	$$ = create_gra_node("StructSpecifier", @$.first_line, 5, $1, NULL);
	connect_node($1,$2);
	connect_node($2,$3);
	connect_node($3,$4);
	connect_node($4,$5); 
	}
| STRUCT Tag  { 
	$$ = create_gra_node("StructSpecifier", @$.first_line, 5, $1, NULL);
	connect_node($1,$2); 
	} | STRUCT error {syntax_error++;} ;
OptTag : ID{ 
	$$ = create_gra_node("OptTag", @$.first_line, 5, $1, NULL); 
	}
|      { 
	$$ = create_gra_node("OptTag", @$.first_line, 5, NULL, NULL); 
	}         ;
Tag : ID   { 
	$$ = create_gra_node("Tag", @$.first_line, 5, $1, NULL); 
	}     ;

//declarators
VarDec : ID{ 
	$$ = create_gra_node("VarDec", @$.first_line, 5, $1, NULL); 
	}
| VarDec LB INT RB   { 
	$$ = create_gra_node("VarDec", @$.first_line, 5, $1, NULL); 
	connect_node($1,$2);
	connect_node($2,$3);
	connect_node($3,$4);
	};
	//|VarDec LB error RB{syntax_error++;};
FunDec : ID LP VarList RP{ 
	$$ = create_gra_node("FunDec", @$.first_line, 5, $1, NULL); 
	connect_node($1,$2);
	connect_node($2,$3);
	connect_node($3,$4);
	}
| ID LP RP{ 
	$$ = create_gra_node("FunDec", @$.first_line, 5, $1, NULL); 
	connect_node($1,$2);
	connect_node($2,$3);
	}
	|ID error{syntax_error++;};
VarList : ParamDec COMMA VarList{ 
	$$ = create_gra_node("VarList", @$.first_line, 5, $1, NULL);
	connect_node($1,$2);
	connect_node($2,$3); 
	}
| ParamDec{ 
	$$ = create_gra_node("VarList", @$.first_line, 5, $1, NULL); 
	};
ParamDec : Specifier VarDec{ $$ = create_gra_node("ParamDec", @$.first_line, 5, $1, NULL);connect_node($1,$2); }|Specifier error{syntax_error++;};

//statements
CompSt : LC DefList StmtList RC{ 
	$$ = create_gra_node("CompSt", @$.first_line, 5, $1, NULL);
	connect_node($1,$2);
	connect_node($2,$3);
	connect_node($3,$4); 
	}|error RC{syntax_error++;};
StmtList : Stmt StmtList{ 
	$$ = create_gra_node("StmtList", @$.first_line, 5, $1, NULL);
	connect_node($1,$2); 
	}
| { $$ = create_gra_node("StmtList", @$.first_line, 5, NULL, NULL); };
Stmt : Exp SEMI{ 
	$$ = create_gra_node("Stmt", @$.first_line, 5, $1, NULL);
	connect_node($1,$2); 
	}
| CompSt{ 
	$$ = create_gra_node("Stmt", @$.first_line, 5, $1, NULL); 
	}
| RETURN Exp SEMI{ 
	$$ = create_gra_node("Stmt", @$.first_line, 5, $1, NULL);
	connect_node($1,$2);
	connect_node($2,$3); 
	}
| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE{ 
	$$ = create_gra_node("Stmt", @$.first_line, 5, $1, NULL); 
	connect_node($1,$2);
	connect_node($2,$3);
	connect_node($3,$4);
	connect_node($4,$5);}
| IF LP Exp RP Stmt ELSE Stmt{ 
	$$ = create_gra_node("Stmt", @$.first_line, 5, $1, NULL); 
	connect_node($1,$2);
	connect_node($2,$3);
	connect_node($3,$4);
	connect_node($4,$5);
	connect_node($5,$6);
	connect_node($6,$7);	}
| WHILE LP Exp RP Stmt{ 
	$$ = create_gra_node("Stmt", @$.first_line, 5, $1, NULL);
	connect_node($1,$2);
	connect_node($2,$3);
	connect_node($3,$4);
	connect_node($4,$5); 
	}
	|error SEMI{syntax_error++;};

//local definitions
DefList : Def DefList{ 
	$$ = create_gra_node("DefList", @$.first_line, 5, $1, NULL);
	connect_node($1,$2); }
| { $$ = create_gra_node("DefList", @$.first_line, 5, NULL, NULL); };
Def : Specifier DecList SEMI{ 
	$$ = create_gra_node("Def", @$.first_line, 5, $1, NULL);
	connect_node($1,$2);
	connect_node($2,$3); 
	}|Specifier error SEMI{syntax_error++;}//error semi result in conflict
	;
DecList : Dec{ 
	$$ = create_gra_node("DecList", @$.first_line, 5, $1, NULL); 
	}
| Dec COMMA DecList{ 
	$$ = create_gra_node("DecList", @$.first_line, 5, $1, NULL); 
	connect_node($1,$2);
	connect_node($2,$3);
	};
Dec : VarDec{ 
	$$ = create_gra_node("Dec", @$.first_line, 5, $1, NULL); 
	}
| VarDec ASSIGNOP Exp{ 
	$$ = create_gra_node("Dec", @$.first_line, 5, $1, NULL); 
	connect_node($1,$2);
	connect_node($2,$3);
	};

//expression
Exp : Exp ASSIGNOP Exp{ $$ = create_gra_node("Exp", @$.first_line, 5, $1, NULL);connect_node($1,$2);connect_node($2,$3); }
| Exp AND Exp{ $$ = create_gra_node("Exp", @$.first_line, 5, $1, NULL);connect_node($1,$2);connect_node($2,$3); }
| Exp OR Exp{ $$ = create_gra_node("Exp", @$.first_line, 5, $1, NULL);connect_node($1,$2);connect_node($2,$3); }
| Exp RELOP Exp{ $$ = create_gra_node("Exp", @$.first_line, 5, $1, NULL);connect_node($1,$2);connect_node($2,$3); }
| Exp PLUS Exp{ $$ = create_gra_node("Exp", @$.first_line, 5, $1, NULL);connect_node($1,$2);connect_node($2,$3); }
| Exp MINUS Exp{ $$ = create_gra_node("Exp", @$.first_line, 5, $1, NULL);connect_node($1,$2);connect_node($2,$3); }
| Exp STAR Exp{ $$ = create_gra_node("Exp", @$.first_line, 5, $1, NULL);connect_node($1,$2);connect_node($2,$3); }
| Exp DIV Exp{ $$ = create_gra_node("Exp", @$.first_line, 5, $1, NULL);connect_node($1,$2);connect_node($2,$3); }
| LP Exp RP{ $$ = create_gra_node("Exp", @$.first_line, 5, $1, NULL);connect_node($1,$2);connect_node($2,$3); }
| MINUS Exp{ $$ = create_gra_node("Exp", @$.first_line, 5, $1, NULL);connect_node($1,$2); }
| NOT Exp{ $$ = create_gra_node("Exp", @$.first_line, 5, $1, NULL);connect_node($1,$2); }
| ID LP Args RP{ $$ = create_gra_node("Exp", @$.first_line, 5, $1, NULL);connect_node($1,$2);connect_node($2,$3);connect_node($3,$4); }
| ID LP RP{ $$ = create_gra_node("Exp", @$.first_line, 5, $1, NULL);connect_node($1,$2);connect_node($2,$3); }
| Exp LB Exp RB{ $$ = create_gra_node("Exp", @$.first_line, 5, $1, NULL);connect_node($1,$2);connect_node($2,$3);connect_node($3,$4); }
| Exp DOT ID{ $$ = create_gra_node("Exp", @$.first_line, 5, $1, NULL);connect_node($1,$2);connect_node($2,$3); }
| ID{ $$ = create_gra_node("Exp", @$.first_line, 5, $1, NULL); }
| INT{ $$ = create_gra_node("Exp", @$.first_line, 5, $1, NULL); }
| FLOAT{ $$ = create_gra_node("Exp", @$.first_line, 5, $1, NULL); }
//| LP error RP{syntax_error++;}
//| Exp LB error RB{syntax_error++;}
;
Args : Exp COMMA Args{ $$ = create_gra_node("Exp", @$.first_line, 5, $1, NULL);connect_node($1,$2);connect_node($2,$3); }
| Exp{ $$ = create_gra_node("Exp", @$.first_line, 5, $1, NULL); };

%%
yyerror(char* msg) {
    // 输出错误信息
    fprintf(stderr, "Error type B at line %d: %s\n",yylineno, msg);
}

