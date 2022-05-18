/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"

/* counter for variable memory locations */
static int location = 0;
static char * currentFuncName;
static ScopeList globalScope;
// TODO input output?

/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */

static void typeError(TreeNode * t, char * message)
{ fprintf(listing,"Error: Type error at line %d: %s\n",t->lineno,message);
  Error = TRUE;
}

static void voidTypeError(TreeNode * t) 
{
    fprintf(listing,"Error: Variable Type cannot be Void at line %d (name : %s)\n", t->lineno,
            t->attr.name);
    Error = TRUE;
}

static void printError(TreeNode * t, char * message) {
    fprintf(listing, "Error: %s at line %d\n", message, t->lineno);
    Error = TRUE;
}

static void ScopeError(TreeNode * t, char * message)
{
    fprintf(listing, "Error: %s '%s' at line %d\n", message, t->attr.name, t->lineno);
    Error = TRUE;
}



static void traverse( TreeNode * t,
               void (* preProc) (TreeNode *),
               void (* postProc) (TreeNode *) )
{ if (t != NULL)
  { preProc(t);
    
    { int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(t->child[i],preProc,postProc);
    }
    postProc(t);
    //fprintf(listing, "hi1 %d\n", t->type);
    traverse(t->sibling,preProc,postProc);
    //fprintf(listing, "hi2 %d\n", t->type);
  }
}

static void popNode( TreeNode * t) {
    switch (t->kind) {
        case FuncK:
        case CompK:
            if (t->attr.name != NULL) break;
            pop();
            break;
        default:
            break;
    }
}

/* nullProc is a do-nothing procedure to 
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t)
{ if (t==NULL) return;
  else return;
}

/* Procedure insertNode inserts 
 * identifiers stored in t into 
 * the symbol table 
 */
static void insertNode( TreeNode * t)
{ 
    
    //fprintf(listing, "InsertNode Start!!\n");
    //fprintf(listing, "kind : %d\n", t->kind);
    switch (t->kind) { 
    case ParamK:
       if (t->type == Void) break;
    case VarDecK:
       {
      if (st_lookup(top()->name, t->attr.name) == NULL) {
          st_insert(top()->name, t->attr.name, t->type, t->lineno, top()->location++);
      }
      else 
          ScopeError(t, "Redefined Variable");
       }
      break;
    case FuncK:
      {
      ScopeList funcScopeList = getScopeList(t->attr.name);
      
      if (st_lookup("global", t->attr.name) == NULL) {
        st_insert("global", t->attr.name, t->type, t->lineno, globalScope->location++);
        
        BucketList l = st_lookup("global", t->attr.name);
        if (l == NULL) fprintf(listing, "NULL!\n");
        l->funcInfo.paramNum = 0;
        l->funcInfo.returnType = t->type;
        TreeNode * child = t->child[0];

        if (child->attr.name == NULL) {
            l->funcInfo.paramTypes[0] = VOID;
        } else {
            while (child != NULL) {
                l->funcInfo.paramTypes[l->funcInfo.paramNum++] = child->type;
                child = child->sibling;
            }
        }
      } else {
        ScopeError(t, "Redefined Function");
        break;
      }

      if (funcScopeList == NULL) {
        ScopeList s = createScopeList(t->attr.name);
        s->parent = globalScope;
        push(s);
      }
      }
      break;
    case CompK:
      {
      if (t->attr.name != NULL) break;
      //if (strcmp(t->attr.name, "\0") != 0) break;
      char scopeName[20];
      sprintf(scopeName, "CompScope_%d", location++);

      ScopeList compScopeList = createScopeList(scopeName);
      compScopeList->parent = top();
      push(compScopeList);
      }
      break;
      /*
    case AssignK:
      if (st_lookup(t->attr.name) == NULL)
        st_insert(t->attr.name,t->lineno,location++);
      else
        st_insert(t->attr.name,t->lineno,0);
      break;
      */
    case OpK: 
      t->type = Integer;
      break;
    case CallK:
      {
      BucketList l;
      if ((l = st_lookup(top()->name, t->attr.name)) == NULL) {
        ScopeError(t, "Undefined Function");
      } else {
        st_insert("global", t->attr.name, t->type, t->lineno, globalScope->location++);
        t->type = l->funcInfo.returnType;
      }
      }
      break;
    case VarK:
      {
      BucketList l;
      if ((l = st_lookup(top()->name, t->attr.name)) == NULL) {
        ScopeError(t, "Undefined Variable");
      } else {
        ScopeList s = findScopeList(t->attr.name);
        st_insert(s->name, t->attr.name, t->type, t->lineno, s->location++);
        if (l->type == Integer) {
            t->type = Integer;
        } else if (l->type == IntegerArray) {
            if (t->child[0] == NULL) {
                t->type = IntegerArray;
            } else t->type = Integer;
        }
      }
      }
      break;
    default:
      break;
      
  }
    //fprintf(listing, "Insert Node End!!!!!!!!\n");
}

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode * syntaxTree)
{ 
    currentFuncName = malloc(30);
    globalScope = createScopeList("global");
    push(globalScope);
    // output input st_insert TODO
    st_insert("global", "input", Integer, 0, globalScope->location++);
    st_insert("global", "output", Void, 0, globalScope->location++);
     
    BucketList l = st_lookup("global", "input");
    
    l->funcInfo.paramNum = 0;
    l->funcInfo.returnType = Integer;
    l->funcInfo.paramTypes[0] = VOID;

    l = st_lookup("global", "output");

    l->funcInfo.paramNum = 1;
    l->funcInfo.returnType = Void;
    l->funcInfo.paramTypes[0] =Integer;

    traverse(syntaxTree,insertNode,popNode);
    pop(globalScope); //TODO
  if (TraceAnalyze)
  { fprintf(listing,"\nSymbol table:\n\n");
    printSymTab(listing);
  }
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode * t)
{ 
    BucketList ret;
    //fprintf(listing, "kind : %d\n", t->kind); 
    switch (t->kind) {
        /*
    case FuncK:
        strcpy(currentFuncName, t->attr.name);
        break;
        */
    case VarDecK:
        if (t->type == Void) voidTypeError(t);
        break;
    case ReturnK:
        ret = st_lookup(top()->name, top()->name);
        //fprintf(listing, "current Func : %s\n", currentFuncName);
        if (ret == NULL) break;
        if (ret->funcInfo.returnType == Void) {
            if (t->child[0]->type != Void) printError(t, "Return something in void function");
        } else {
            TreeNode * child = t->child[0];
            if (child == NULL) printError(t, "Return nothing in non-void function");
            else if (child->type != ret->funcInfo.returnType) {
                //fprintf(listing, "child type : %d, funcType : %d, name : %s", child->type, 
                        //ret->funcInfo.returnType, ret->name);
                printError(t, "Return type is wrong");
            }
        }
        break;
    case BinaryK:
        //fprintf(listing, "type : %d, type : %d\n", t->child[0]->type, t->child[1]->type);        
        if ((t->child[0]->type != Integer ) ||
            (t->child[1]->type != Integer))
          typeError(t,"Op applied to non-integer");
        
        /*
        if ((t->attr.op == EQ) || (t->attr.op == LT) || (t->attr.op == NE) ||
            (t->attr.op == LE) || (t->attr.op == GT) || (t->attr.op == GE))
          t->type = Integer; // all Integer??
        else
          t->type = Integer;
        */
        t->type = Integer;
        break;
      case OpK:
      case ConstK:
        t->type = Integer;
        break;
      case IdK:
      case VarK:
        if (t->child[0] != NULL) {
            if (t->child[0]->type != Integer) printError(t, "Array Indexing Error");
            t->type = Integer;
        } else t->type = Integer;
        break;
      case WhileK:
      case IfK:
        if (t->child[0]->type != Integer) {
            typeError(t->child[0],"if/while condition is not Integer");
        }
        break;
        /*
      case AssignK:
        if ()
          typeError(t->child[0],"assignment of invalid value");
        t->type = t->child[0]->type;
        break;
        */
      case CallK:
        {
        ret = st_lookup("global", t->attr.name);
        TreeNode * args = t->child[0];
        FuncInfo f = ret->funcInfo;
               
        int argNum = 0;
        
        while (args != NULL) {
            argNum++;
            args = args->sibling;
        }
       
        if (argNum != f.paramNum) {
            typeError(t, "Number of parameter mismatch");
            break;
        }
        
        args = t->child[0];
        if (f.paramNum == 0) {
            if (args != NULL) {
                typeError(t, "Parameter Type mismatch");
                break;
            }
        }
        
        args = t->child[0];
        for (int k = 0; k < argNum; ++k) {
            if (args->type != f.paramTypes[k]) typeError(t, "Parameter Type mismatch");
            args = args->sibling;
        }
        }
        break;
      default:
        break;
  }
}

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * syntaxTree)
{ traverse(syntaxTree,nullProc,checkNode);
}
