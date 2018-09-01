%code requires{
    #include "ast.h"
}

%{
    #include <stdio.h>
	#include "ast.h"
    
    using namespace std;

    int yylex();
    extern int yylineno;
    void yyerror(const char* msg){
        printf("Line: %d %s\n", yylineno, msg);
    }
    #define YYERROR_VERBOSE 1
    #define YYDEBUG 1   

	Stmnt * input;

	/* 
		Credit: https://www.lysator.liu.se/c/ANSI-C-grammar-y.html
	*/
%}

%union {
    int int_t;
    char * string_t;
    char  char_t;

    Stmnt * statement_t;
    Expr * expression_t;
    StmntList * statement_list_t;
	ExprList * expression_list_t;
}

%expect 1

%token<string_t> ID
%token<int_t> INT_VAL
%token<char_t> CHAR_VAL
%token IF ELSE FOR WHILE INT CHAR VOID RETURN SIZEOF
%token LE GE NE EQ LS RS OR AND 
%token PLE SBE MLE DVE MDE RSE LSE DEC INC XRE ORE ANE
%token STRING_VAL PRINTF TIME SRAND RAND

%type<string_t> STRING_VAL
%type<statement_list_t> statement_list declaration_list translation_unit parameter_list declarator_list
%type<expression_list_t> initializer_list expression_list argument_list
%type<statement_t> jump_statement iteration_statement selection_statement declaration function_definition
%type<statement_t> compound_statement statement external_declaration parameter_declaration
%type<statement_t> direct_declarator init_declarator declarator built_in_func
%type<expression_t> expression assignment_expression conditional_expression logical_or_expression logical_and_expression 
%type<expression_t> inclusive_or_expression exclusive_or_expression and_expression equality_expression relational_expression
%type<expression_t> shift_expression additive_expression multiplicative_expression unary_expression initializer
%type<expression_t> postfix_expression primary_expression optional_expression
%type<int_t> type_specifier type_name

%start input
%%

input: translation_unit {
	input = $1;
}
;


primary_expression: ID { $$ = new IdExpr($1); }
	| INT_VAL { $$ = new IntExpr($1); }  
	| CHAR_VAL { $$ = new CharExpr($1); }
	| '(' expression ')'  { $$ = $2; }
	;

postfix_expression: primary_expression { $$ = $1; }
	| ID '[' expression ']' { $$ = new ArrayCallExpr($1, $3); }
	| ID '(' ')'  { $$ = new FunctionCallExpr($1, NULL); }
	| ID '(' argument_list ')' { $$ = new FunctionCallExpr($1, $3); }
	| postfix_expression INC { $$ = new PostIncUnExpr($1); }
	| postfix_expression DEC { $$ = new PostDecUnExpr($1); }
	;

argument_list: expression_list { $$ = $1; }
	;

unary_expression: postfix_expression { $$ = $1; }
	| INC unary_expression { $$ = new PreIncUnExpr($2); }
	| DEC unary_expression { $$ = new PreDecUnExpr($2); }
	| '-' unary_expression { $$ = new NegativeUnExpr($2); }
	| '~' unary_expression { $$ = new BitwiseNotUnExpr($2); }
	| '!' unary_expression { $$ = new NotUnExpr($2); }
	| SIZEOF unary_expression { $$ = new SizeOfUnExpr($2); }
	| SIZEOF '(' type_name ')' { $$ = new SizeOfUnExpr(NULL, $3); }
	| RAND '(' ')'	{ $$ = new RandFunc(); }
	| TIME '(' INT_VAL ')' { $$ = new TimeFunc($3); }
	;

multiplicative_expression: unary_expression { $$ = $1; }
	| multiplicative_expression '*' unary_expression { $$ = new MulExpr($1, $3); }
	| multiplicative_expression '/' unary_expression { $$ = new DivExpr($1, $3); }
	| multiplicative_expression '%' unary_expression { $$ = new ModExpr($1, $3); }
	;

additive_expression: multiplicative_expression { $$ = $1; }
	| additive_expression '+' multiplicative_expression { $$ = new PlusExpr($1, $3); }
	| additive_expression '-' multiplicative_expression { $$ = new SubExpr($1, $3); }
	;

shift_expression: additive_expression { $$ = $1; }
	| shift_expression LS additive_expression { $$ = new LeftShiftExpr($1, $3); }
	| shift_expression RS additive_expression { $$ = new RightShiftExpr($1, $3); }
	;

relational_expression: shift_expression { $$ = $1; }
	| relational_expression '<' shift_expression { $$ = new LesserThanExpr($1, $3); }
	| relational_expression '>' shift_expression { $$ = new GreaterThanExpr($1, $3); }
	| relational_expression LE shift_expression { $$ = new LesserEqualExpr($1, $3); }
	| relational_expression GE shift_expression { $$ = new GreaterEqualExpr($1, $3); }
	;

equality_expression: relational_expression { $$ = $1; }
	| equality_expression EQ relational_expression { $$ = new EqualExpr($1, $3); }
	| equality_expression NE relational_expression { $$ = new NotEqualExpr($1, $3); }
	;

and_expression: equality_expression { $$ = $1; }
	| and_expression '&' equality_expression { $$ = new AndExpr($1, $3); }
	;

exclusive_or_expression: and_expression { $$ = $1; }
	| exclusive_or_expression '^' and_expression { $$ = new XorExpr($1, $3); }
	;

inclusive_or_expression: exclusive_or_expression { $$ = $1; }
	| inclusive_or_expression '|' exclusive_or_expression { $$ = new IncOrExpr($1, $3); }
	;

logical_and_expression: inclusive_or_expression { $$ = $1; }
	| logical_and_expression AND inclusive_or_expression { $$ = new LogicalAndExpr($1, $3); }
	;

logical_or_expression: logical_and_expression { $$ = $1; }
	| logical_or_expression OR logical_and_expression { $$ = new LogicalOrExpr($1, $3); }
	;

conditional_expression: logical_or_expression { $$ = $1; }
	| logical_or_expression '?' expression ':' conditional_expression { $$ = new TernaryExpr($1, $3, $5); }
	;

assignment_expression: unary_expression '=' expression { $$ = new AssignExpr($1, $3); }
	| unary_expression PLE expression { $$ = new PlusAssignExpr($1, $3); } 
    | unary_expression SBE expression { $$ = new SubAssignExpr($1, $3); }
    | unary_expression MLE expression { $$ = new MulAssignExpr($1, $3); }
    | unary_expression DVE expression { $$ = new DivAssignExpr($1, $3); }
    | unary_expression MDE expression { $$ = new ModAssignExpr($1, $3); }
    | unary_expression RSE expression { $$ = new RShiftAssignExpr($1, $3); }
    | unary_expression LSE expression { $$ = new LShiftAssignExpr($1, $3); }
    | unary_expression XRE expression { $$ = new XorAssignExpr($1, $3); }
    | unary_expression ORE expression { $$ = new OrAssignExpr($1, $3); }
    | unary_expression ANE expression { $$ = new AndAssignExpr($1, $3); }
	;

expression: assignment_expression { $$ = $1; }
	| conditional_expression	{ $$ = $1; }
	;

expression_list: expression  { $$ = new ExprList(); $$->push_back($1); }
	| expression_list ',' expression { $$ = $1; $$->push_back($3); }
	;

declaration: type_specifier declarator_list ';'  { $$ = new Declarations($1, $2); }
	;

declarator_list: init_declarator { $$ = new StmntList(); $$->push_back($1); }
	| declarator_list ',' init_declarator	{ $$ = $1; $$->push_back($3); }
	;

init_declarator: declarator	{
		if(VarDeclaration * var = dynamic_cast<VarDeclaration*>($1)) {
			$$ =  var;	
		}
		else if (ArrayDeclaration * arr = dynamic_cast<ArrayDeclaration*>($1)) {
			$$ = arr;
		}
	}

	| declarator '=' initializer { 
			
		if(VarDeclaration * var = dynamic_cast<VarDeclaration*>($1)) {
			var->value = $3;
			$$ =  var;	
		}
		else if (ArrayDeclaration * arr = dynamic_cast<ArrayDeclaration*>($1)) {
			if(ExprList * exprs = dynamic_cast<ExprList*>($3))
			{
				arr->value = exprs;
			}
			$$ = arr;
		}
	}
	;

type_specifier: VOID { $$ = 1; }
	| CHAR	{ $$ = 2; }
	| INT	{ $$ = 3; }
	;

type_name: type_specifier { $$ = $1; }
	;

declarator: direct_declarator { $$ = $1; }
	;

direct_declarator: ID	{ $$ = new VarDeclaration($1); }
	| ID '[' INT_VAL ']' { $$ = new ArrayDeclaration($1, $3); }
	| ID '[' ']'	{ $$ = new ArrayDeclaration($1, 0); }
	| ID '(' parameter_list ')' { $3->context = $1; $$ = new FuncDeclaration($1, $3); }
	| ID '(' ')' { $$ = new FuncDeclaration($1, NULL); }
	;

parameter_list: parameter_declaration	{ $$ = new StmntList(); $$->push_back($1); }
	| parameter_list ',' parameter_declaration { $$ = $1; $$->push_back($3); }
	;

parameter_declaration: type_specifier declarator { $$ = new ParamDeclaration($1, $2); }
	| type_specifier { $$ = new ParamDeclaration($1, NULL); } /* TALVES ESTA MALO... ES PARA PROTOTIPOS */
	;

initializer: expression		{ $$ = $1; }
	| '{' initializer_list '}' 	{ $$ = $2; }
	;

initializer_list: expression	{ $$ = new ExprList(); $$->push_back($1); }
	|	initializer_list ',' expression { $$ = $1; $$->push_back($3); }
	;

statement: optional_expression ';' { $$ =  new ExpressionStmnt($1); }
	| selection_statement { $$ =  $1; }
	| iteration_statement { $$ =  $1; }
	| jump_statement { $$ =  $1; }
	| compound_statement { $$ =  $1; }
	| built_in_func ';' { $$ = $1; }
	;

built_in_func: SRAND '(' expression ')'	{ $$ = new SrandFunc($3); }
	| PRINTF '(' STRING_VAL ')'	{ $$ = new PrintfFunc($3); }
	| PRINTF '(' STRING_VAL ',' expression_list ')' { $$ = new PrintfFunc($3, $5); }

;

compound_statement: '{' '}' { $$ = new BlockStmnt(NULL, NULL); }
	| '{' statement_list '}' { $$ = new BlockStmnt(NULL, $2); }
	| '{' declaration_list '}' { $$ = new BlockStmnt($2, NULL); }
	| '{' declaration_list statement_list '}' { $$ = new BlockStmnt($2, $3); }
	;

declaration_list: declaration	{ $$ = new StmntList(); $$->push_back($1); }
	| declaration_list declaration	{ $$ = $1; $$->push_back($2); }
	;

statement_list: statement { $$ = new StmntList(); $$->push_back($1); }
	| statement_list statement { $$ = $1; $$->push_back($2); }
	;

optional_expression: expression { $$ = $1; }
	|  %empty	{ $$ = NULL; }
	;

selection_statement: IF '(' expression ')' statement { $$ = new IfStmnt($3, $5, NULL); }
	| IF '(' expression ')' statement ELSE statement { $$ = new IfStmnt($3, $5, $7); }
	;

iteration_statement: WHILE '(' expression ')' statement { $$ = new WhileStmnt($3, $5); }
	| FOR '(' optional_expression ';' optional_expression ';' optional_expression ')' statement { $$ = new ForStmnt($3, $5, $7, $9); }
	;

jump_statement: RETURN ';' { $$ = new ReturnStmnt(NULL); }
	| RETURN expression ';' { $$ = new ReturnStmnt($2); }
	;

translation_unit: external_declaration	{ $$ = new StmntList(); $$->push_back($1); }
	| translation_unit external_declaration { $$ = $1; $$->push_back($2); }
	;

external_declaration: function_definition	{ $$ = $1; }
	| declaration	{ $$ = $1; }
	;

function_definition: type_specifier declarator compound_statement { $$ = new FuncDefinition($1, $2, $3); }
	;
