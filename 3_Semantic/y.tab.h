/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    IF = 258,
    WHILE = 259,
    RETURN = 260,
    INT = 261,
    VOID = 262,
    ID = 263,
    NUM = 264,
    COMMA = 265,
    SEMI = 266,
    ASSIGN = 267,
    EQ = 268,
    LT = 269,
    NE = 270,
    LE = 271,
    GT = 272,
    GE = 273,
    PLUS = 274,
    MINUS = 275,
    TIMES = 276,
    OVER = 277,
    LPAREN = 278,
    RPAREN = 279,
    LBRACE = 280,
    RBRACE = 281,
    LCURLY = 282,
    RCURLY = 283,
    ELSE = 284,
    ERROR = 285
  };
#endif
/* Tokens.  */
#define IF 258
#define WHILE 259
#define RETURN 260
#define INT 261
#define VOID 262
#define ID 263
#define NUM 264
#define COMMA 265
#define SEMI 266
#define ASSIGN 267
#define EQ 268
#define LT 269
#define NE 270
#define LE 271
#define GT 272
#define GE 273
#define PLUS 274
#define MINUS 275
#define TIMES 276
#define OVER 277
#define LPAREN 278
#define RPAREN 279
#define LBRACE 280
#define RBRACE 281
#define LCURLY 282
#define RCURLY 283
#define ELSE 284
#define ERROR 285

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
