/* A Bison parser, made by GNU Bison 3.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_TOKEN_H_INCLUDED
# define YY_YY_TOKEN_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 1 "grammar.y" /* yacc.c:1919  */

    #include "ast.h"

#line 48 "token.h" /* yacc.c:1919  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ID = 258,
    INT_VAL = 259,
    CHAR_VAL = 260,
    IF = 261,
    ELSE = 262,
    FOR = 263,
    WHILE = 264,
    INT = 265,
    CHAR = 266,
    VOID = 267,
    RETURN = 268,
    SIZEOF = 269,
    LE = 270,
    GE = 271,
    NE = 272,
    EQ = 273,
    LS = 274,
    RS = 275,
    OR = 276,
    AND = 277,
    PLE = 278,
    SBE = 279,
    MLE = 280,
    DVE = 281,
    MDE = 282,
    RSE = 283,
    LSE = 284,
    DEC = 285,
    INC = 286,
    XRE = 287,
    ORE = 288,
    ANE = 289,
    STRING_VAL = 290,
    PRINTF = 291,
    TIME = 292,
    SRAND = 293,
    RAND = 294
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 26 "grammar.y" /* yacc.c:1919  */

    int int_t;
    char * string_t;
    char  char_t;

    Stmnt * statement_t;
    Expr * expression_t;
    StmntList * statement_list_t;
	ExprList * expression_list_t;

#line 111 "token.h" /* yacc.c:1919  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_TOKEN_H_INCLUDED  */
