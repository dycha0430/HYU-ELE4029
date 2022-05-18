/****************************************************/
/* File: tiny.y                                     */
/* The TINY Yacc/Bison specification file           */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode *
static char * savedName; /* for use in assignments */
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* stores syntax tree for later return */
static int yylex(void); // added 11/2/11 to ensure no conflict with lex

%}

%token IF WHILE RETURN INT VOID 
%token ID NUM 
/*%token COMMA SEMI ASSIGN EQ LT NE LE GT GE PLUS MINUS TIMES OVER LPAREN RPAREN LBRACE RBRACE LCURLY RCURLY
*/
%token COMMA SEMI 
%right ASSIGN
%left EQ LT NE LE GT GE 
%left PLUS MINUS 
%left TIMES OVER
%left LPAREN RPAREN LBRACE RBRACE LCURLY RCURLY 
%left ELSE 
%token ERROR

%% /* Grammar for TINY */

program     : declaration_list
                 { savedTree = $1;} 
            ;
declaration_list : declaration_list declaration 
                {
                    YYSTYPE t = $1;
                    if (t != NULL) {
                        while (t->sibling != NULL) t = t->sibling;
                        t->sibling = $2;
                        $$ = $1;
                    } else $$ = $2;
                }
            | declaration {$$ = $1;}
            ;
declaration : var_declaration {
                $$ = $1;
              }
            | fun_declaration {
                $$ = $1;
            }
            ;
var_declaration : type_specifier _id SEMI {
                    $$ = newNode(VarDecK);
                    $$->type = $1->type;
                    $$->attr.name = $2->attr.name;
                    $$->lineno = $2->lineno;
                    $$->isArray = 0;
                  }
            | type_specifier _id LBRACE factor RBRACE SEMI {
                $$ = newNode(VarDecK);
                $$->attr.name = $2->attr.name;
                $$->lineno = $2->lineno;
                $$->type = $1->type;
                $$->isArray = 1;
                $$->child[0] = $4;
            }
            ;
type_specifier : INT {
                    $$ = newNode(TypeK);
                    $$->type = Integer;
                    $$->isArray = 0;
                 }
            | VOID {
                $$ = newNode(TypeK);
                $$->type = Void;
                $$->isArray = 0;
            }
fun_declaration : type_specifier _id LPAREN params RPAREN compound_stmt {
                    $$ = newNode(FuncK);
                    $$->child[0] = $4;
                    $$->child[1] = $6;
                    $$->attr.name = $2->attr.name;
                    $$->lineno = $2->lineno;
                    $$->type = $1->type;
                  }
                ;
params      : param_list {
                $$ = $1;
              }
            | VOID {
                $$ = newNode(ParamK);
                $$->attr.name = NULL;
                $$->type = Void;
                $$->isArray = 0;
            }
            ;
param_list  : param_list COMMA param {
                 YYSTYPE t = $1;
                 if (t != NULL) {
                     while (t->sibling != NULL) t = t->sibling;
                     t->sibling = $3;
                     $$ = $1;
                 } else $$ = $3;
              }
            | param {$$ = $1;}
            ;
param       : type_specifier _id{
                $$ = newNode(ParamK);
                $$->attr.name = $2->attr.name;
                $$->lineno = $2->lineno;
                $$->type = $1->type;
                $$->isArray = 0;
              }
            | type_specifier _id LBRACE RBRACE {
                $$ = newNode(ParamK);
                $$->attr.name = $2->attr.name;
                $$->lineno = $2->lineno;
                $$->type = $1->type;
                $$->isArray = 1;
            }
            ;
compound_stmt : LCURLY local_declarations stmt_list RCURLY {
                  $$ = newNode(CompK);
                  $$->child[0] = $2;
                  $$->child[1] = $3;
                }
            ;
local_declarations : local_declarations var_declaration {
                    YYSTYPE t = $1;
                    if (t != NULL) {
                        while (t->sibling != NULL) t = t->sibling;
                        t->sibling = $2;
                          $$ = $1;
                    } else $$ = $2;
                  }
                | {$$ = NULL;}
                ;
stmt_list     : stmt_list stmt {
                    YYSTYPE t = $1;
                    if (t != NULL) {
                        while (t->sibling != NULL) t = t->sibling;
                        t->sibling = $2;
                        $$ = $1;
                    } else $$ = $2;
                }
            | {$$ = NULL;}
            ;
stmt          : expression_stmt {$$ = $1;}
                | compound_stmt {$$ = $1;}
                | selection_stmt {$$ = $1;}
                | iteration_stmt {$$ = $1;}
                | return_stmt {$$ = $1;}
                ;
expression_stmt : expression SEMI {
                    $$ = $1;
                  }
                | SEMI {$$ = NULL;}
                ;
selection_stmt     : IF LPAREN expression RPAREN stmt
                 { $$ = newNode(IfK);
                   $$->child[0] = $3;
                   $$->child[1] = $5;
                 }
            | IF LPAREN expression RPAREN stmt ELSE stmt
                 { $$ = newNode(IfK);
                   $$->child[0] = $3;
                   $$->child[1] = $5;
                   $$->child[2] = $7;
                 }
            ;
iteration_stmt   : WHILE LPAREN expression RPAREN stmt
                { $$ = newNode(WhileK);
                  $$->child[0] = $3;
                  $$->child[1] = $5;
                }
            ;
return_stmt : RETURN expression SEMI 
                {
                  $$ = newNode(ReturnK);
                  $$->child[0] = $2;
                }
            | RETURN SEMI
                {
                  $$ = newNode(ReturnK);
                }
            ;
expression  : var ASSIGN expression {
                $$ = newNode(AssignK);
                $$->child[0] = $1;
                $$->child[1] = $3;
              }
            | simple_expression {
                $$ = $1;
            }
            ;
var         : _id {
                $$ = $1;
                $$->lineno = savedLineNo;
            }
            | _id LBRACE expression RBRACE {
                $$ = newNode(VarK);
                $$->attr.name = $1->attr.name;
                $$->lineno = $1->lineno;
                $$->child[0] = $3;
            }
            ;
_id         : ID {
                $$ = newNode(VarK);
                $$->attr.name = copyString(tokenString);
                savedLineNo = lineno;
              } 
            ;
simple_expression : additive_expression relop additive_expression {
                        $$ = newNode(BinaryK);
                        $$->attr.op = $2->attr.op;
                        $$->child[0] = $1;
                        $$->child[1] = $3;
                    }
                    | additive_expression {$$ = $1;}
                    ;
relop       : EQ {
                $$ = newNode(OpK);
                $$->attr.op = EQ;
              }
            | NE {
                $$ = newNode(OpK);
                $$->attr.op = NE;
            }
            | LE {
                $$ = newNode(OpK);
                $$->attr.op = LE;
              }
            | LT {
                $$ = newNode(OpK);
                $$->attr.op = LT;
            }
            | GE {
                $$ = newNode(OpK);
                $$->attr.op = GE;
            }
            | GT {
                $$ = newNode(OpK);
                $$->attr.op = GT;
            }
            ;
additive_expression : additive_expression PLUS term 
                 { $$ = newNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = PLUS;
                 }
            | additive_expression MINUS term
                 { $$ = newNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = MINUS;
                 } 
            | term { $$ = $1; }
            ;
term        : term TIMES factor 
                 { $$ = newNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = TIMES;
                 }
            | term OVER factor
                 { $$ = newNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = OVER;
                 }
            | factor { $$ = $1; }
            ;
factor      : LPAREN expression RPAREN
                 { $$ = $2; }
            | NUM
                 { 
                   $$ = newNode(ConstK);
                   $$->attr.val = atoi(tokenString);
                 }
            | var {
                $$ = $1;
            }
            | call { 
                $$ = $1;
            }
            ;
call        : _id LPAREN args RPAREN
                {
                  $$ = newNode(CallK);
                  $$->child[0] = $3;
                  $$->attr.name = $1->attr.name;
                  /*$$->lineno = savedLineNo;*/
                }
            ;
args        : arg_list {
                $$ = $1;
              }
            | {$$ = NULL;}
            ;
arg_list    : arg_list COMMA expression {
                YYSTYPE t = $1;
                if (t != NULL) {
                    while (t->sibling != NULL) t = t->sibling;
                    t->sibling = $3;
                    $$ = $1;
                } else $$ = $3;
              }
            | expression {$$ = $1;}
            ;
%%

int yyerror(char * message)
{ fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(listing,"Current token: ");
  printToken(yychar,tokenString);
  Error = TRUE;
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}

