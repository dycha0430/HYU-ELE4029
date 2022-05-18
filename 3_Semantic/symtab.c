/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "globals.h"

ScopeList ScopeStack[SIZE];
int StackTop = -1;


/* the hash function */
static int hash ( char * key )
{ int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  { temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}
/* the hash table */
//static BucketList hashTable[SIZE]; // remove..?
ScopeList ScopeHashTable[SIZE];

ScopeList getScopeList(char* scope) {
    int scope_h = hash(scope);

    ScopeList scope_l = ScopeHashTable[scope_h];
    while ((scope_l != NULL) && (strcmp(scope, scope_l->name)))
        scope_l = scope_l->next;

    return scope_l;
}

ScopeList createScopeList(char* scopeName) {
    int scope_h = hash(scopeName);
    ScopeList scope_l = ScopeHashTable[scope_h];
    while (scope_l != NULL)
        scope_l = scope_l->next;

    if (scope_l == NULL) {
        scope_l = (ScopeList) malloc(sizeof(struct ScopeListRec));
        scope_l->name = malloc(30);
        strcpy(scope_l->name, scopeName);
        scope_l->next = ScopeHashTable[scope_h];
        scope_l->location = 0;
        scope_l->parent = NULL;
        ScopeHashTable[scope_h] = scope_l;
    } else {
        fprintf(listing, "Error in symtab.c\n");
    }

    return scope_l;   
}

/* Scope Stack Operations */
void push(ScopeList item) {
    ScopeStack[++StackTop] = item;
}

ScopeList pop() {
    if (StackTop < 0) return NULL;
    return ScopeStack[StackTop--];
}

ScopeList top() {
    if (StackTop < 0) return NULL;
    return ScopeStack[StackTop];
}

ScopeList findScopeList(char * name) {
    for (int i = 0; i < SIZE; ++i) {
        if (ScopeHashTable[i] != NULL) {
            ScopeList s = ScopeHashTable[i];
            while (s != NULL) {
                for (int j = 0; j < SIZE; ++j) {
                    if (s->hashTable[j] != NULL) {
                        BucketList l = s->hashTable[j];
                        while (l != NULL) {
                            if (strcmp(l->name, name) == 0) {
                                return s;
                            }
                            l = l->next;
                        }
                    }
                }
                s = s->next;
            }
        }
    }

    return NULL;
}



/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( char * scope, char * name, ExpType type, int lineno, int loc )
{
    ScopeList scopeList = getScopeList(scope);
    
    if (scopeList == NULL) fprintf(listing, "Get Scope Null!!\n");
  int h = hash(name);

  BucketList l = scopeList->hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) /* variable not yet in table */
  { l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->scope = scope;
    l->name = malloc(30);
    strcpy(l->name, name);
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->type = type;
    l->memloc = loc;
    l->lines->next = NULL;
    l->next = scopeList->hashTable[h];
    scopeList->hashTable[h] = l; 
  }
  else /* found in table, so just add line number */
  { LineList t = l->lines;
    while (t->next != NULL) t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
  }
} /* st_insert */

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
BucketList st_lookup ( char * scope, char * name )
{ 
    ScopeList s = getScopeList(scope);
    //if (s == NULL) fprintf(listing, "Scope is not found in st_lookup %s\n", scope);
    BucketList l = NULL;
    int h = hash(name);
    while ((s != NULL)) {
        l = s->hashTable[h];
        
        while ((l != NULL) && (strcmp(name,l->name) != 0))
            l = l->next;
        if (l == NULL) s = s->parent;
        else {
            break;
        }
    }

    if (l == NULL) {
        return NULL;
    }
    else {
        return l;
    }
}

BucketList st_lookup_excluding_parent( char * scope, char * name) {
    ScopeList s = getScopeList(scope);
    //if (s == NULL) fprintf(listing, "Scope is not found in st_lookup excluding %s\n", scope);
    int h = hash(name);
    BucketList l = s->hashTable[h];
    while ((l != NULL) && (strcmp(name, l->name) != 0))
        l = l->next;
    if (l == NULL) return NULL;
    else return l;
    
}

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing)
{ int i;
  fprintf(listing,"Variable Name Variable Type Scope Name Location Line Numbers\n");
  fprintf(listing,"------------- ------------- ---------- -------- ------------\n");
  for (i = 0; i < SIZE; ++i) {
    if (ScopeHashTable[i] != NULL) {
        ScopeList s = ScopeHashTable[i];
        while (s != NULL) {
            for (int j = 0; j < SIZE; ++j) {
                if (s->hashTable[j] != NULL) {
                    BucketList l = s->hashTable[j];
                    while (l != NULL) {
                        LineList t = l->lines;
                        fprintf(listing, "%-14s ", l->name);
                        fprintf(listing, "%-14s ", returnSymTabType(l->type));
                        fprintf(listing, "%-11s ", l->scope);
                        fprintf(listing, "%-8d ", l->memloc);
                        while (t != NULL) {
                            fprintf(listing, "%4d ", t->lineno);
                            t = t->next;
                        }
                        fprintf(listing, "\n");
                        l = l->next;
                    }
                }
            }
            s = s->next;
        }
    }
  }
  /* Original code
  for (i=0;i<SIZE;++i)
  { if (hashTable[i] != NULL)
    { BucketList l = hashTable[i];
      while (l != NULL)
      { LineList t = l->lines;
        fprintf(listing,"%-14s ",l->name);
        fprintf(listing,"%-8d  ",l->memloc);
        while (t != NULL)
        { fprintf(listing,"%4d ",t->lineno);
          t = t->next;
        }
        fprintf(listing,"\n");
        l = l->next;
      }
    }
  }
  */
} /* printSymTab */

char * returnSymTabType(ExpType type) {
    char * ret;
    switch (type) {
        case Integer:
            ret = "Integer";
            break;
        case Void:
            ret = "Void";
            break;
        case Function:
            ret = "Function";
            break;
        case IntegerArray:
            ret = "Integer Array";
            break;
        default:
            ret = "Default";
            break;
    }
    
    return ret;
}
