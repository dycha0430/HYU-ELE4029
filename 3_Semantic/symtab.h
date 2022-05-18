/****************************************************/
/* File: symtab.h                                   */
/* Symbol table interface for the TINY compiler     */
/* (allows only one symbol table)                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "globals.h"
/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */

#define SIZE 211
#define SHIFT 4
#define MAX_PARAM_NUM 20

/* the list of line numbers of the source 
 38  * code in which a variable is referenced
 39  */
typedef struct LineListRec
   { int lineno;
     struct LineListRec * next;
   } * LineList;

typedef struct _FuncInfo
{
    ExpType returnType;
    int paramNum;
    ExpType paramTypes[MAX_PARAM_NUM];
} FuncInfo;


typedef struct BucketListRec
   { char * name;
     char * scope;
     ExpType type;
     LineList lines;
     FuncInfo funcInfo;
     int memloc ; /* memory location for variable */
     struct BucketListRec * next;
   } * BucketList;
 
typedef struct ScopeListRec
    {
      char * name;
      struct ScopeListRec * parent;
      struct ScopeListRec * next;
      BucketList hashTable[SIZE];
      int location;
    } * ScopeList;
ScopeList createScopeList(char* scope);
ScopeList getScopeList(char* scope);
void push(ScopeList item);
ScopeList pop();
ScopeList top();
void st_insert( char * scope, char * name, ExpType type, int lineno, int loc );

ScopeList findScopeList(char* name);
/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
BucketList st_lookup ( char * scope, char * name );
BucketList st_lookup_excluding_parent (char * scope, char * name);

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing);
char * returnSymTabType(ExpType type);
#endif
