%option noyywrap
%option yylineno

%{
#include <string>
#include <iostream>
#include "ast.h"
#include "token.h"
#include <cstdlib>

int colno = 0;
%}

number16 0[xX][0-9a-fA-F]+
number10 [0-9]+
number8  0[0-8]+
id       [_a-zA-Z][_a-zA-Z0-9]*
string  "\"".*"\""
char    "'"."'"

%%


[ \t]       { }
\n          { }
"//".*      { }
"//".*\n    { }
"/*"[^*]*"*"+([^/*][^*]*"*"+)*"/"  { }
"#".*\n     { }

"="         { colno++; return '='; }
"+"         { colno++; return '+'; }
"-"         { colno++; return '-'; }
"*"         { colno++; return '*'; }
"/"         { colno++; return '/'; }
"%"         { colno++; return '%'; }
"("         { colno++; return '('; }
")"         { colno++; return ')'; }
";"         { colno++; return ';'; }
"{"         { colno++; return '{'; }
"}"         { colno++; return '}'; }
","         { colno++; return ','; }
"["         { colno++; return '['; }
"]"         { colno++; return ']'; }
"&"         { colno++; return '&'; }
":"         { colno++; return ':'; }
"?"         { colno++; return '?'; }
"|"         { colno++; return '|'; }
"!"         { colno++; return '!'; }
"^"         { colno++; return '^'; }
"~"         { colno++; return '~'; }
"<"         { colno++; return '<'; }
">"         { colno++; return '>'; }
"<="        { colno+=2; return LE; }
">="        { colno+=2;  return GE; }
"!="        { colno+=2;  return NE; }
"=="        { colno+=2;  return EQ; }
"<<"        { colno+=2;  return LS; }
">>"        { colno+=2;  return RS; }
"||"        { colno+=2;  return OR; }
"&&"        { colno+=2;  return AND; }
"--"        { colno+=2;  return DEC; }
"++"        { colno+=2;  return INC; }
"+="        { colno+=2;  return PLE; }
"-="        { colno+=2;  return SBE; }
"*="        { colno+=2;  return MLE; }
"/="        { colno+=2;  return DVE; }
"%="        { colno+=2;  return MDE; }
">>="       { colno+=3;  return RSE; }
"<<="       { colno+=3;  return LSE; }
"^="        { colno+=2;  return XRE; }
"|="        { colno+=2;  return ORE; }
"&="        { colno+=2;  return ANE; }

"if"        { colno+=2;  return IF; }
"else"      { colno+=4;  return ELSE; }
"while"     { colno+=5;  return WHILE; }
"for"       { colno+=3;  return FOR; }
"int"       { colno+=3;  return INT; }
"char"      { colno+=3;  return CHAR; }
"void"      { colno+=4;  return VOID; }
"return"    { colno+=6;  return RETURN; }
"sizeof"    { colno+=6;  return SIZEOF; }
"printf"    { colno+=6;  return PRINTF; }
"time"      { colno+=4;  return TIME; }
"srand"     { colno+=5;  return SRAND; }
"rand"      { colno+=4;  return RAND; }

{number8}   { yylval.int_t = strtol(yytext, NULL, 8); return INT_VAL; }
{number10}  { yylval.int_t = strtol(yytext, NULL, 10); return INT_VAL; }
{number16}  { yylval.int_t = strtol(yytext, NULL, 16); return INT_VAL; }
{char}     { yylval.char_t = yytext[1]; return CHAR_VAL; }
{id}        { yylval.string_t = strdup(yytext); return ID;}
{string}    { yylval.string_t = strdup(yytext); return STRING_VAL; }

.   { }
%%