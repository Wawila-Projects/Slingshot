#ifndef _AUX_H_
#define _AUX_H_

#include <string>
#include <map>
#include "ast.h"

using namespace std;

typedef struct {
    string statement_label;
    string end_label;
} Labels;

typedef struct {
    string id;
    int type;
    int stackPos;
    int size;
    
    bool isArray;
    int length;
    
    bool isFunc;
    int paramCount;

    bool isGlobal;
} Variable;

class Aux {
public: 
    int temp_regs [10] = {0};
    int saved_regs [7] = {0};

    int while_count;
    int for_count;
    int if_count;
    int else_count;
    int const_count;
    int ternary_count;
    int and_count;
    int or_count;

    map<string, map<string, Variable>> contexts; 
    map<string, int> stack;

    string nextTemp();
    string nextSaved();
    string nextConst();

    //int assignTemp(string id);
    //int assignSaved(string id);

    bool releaseTemp(string reg);
    Labels getIfLabels(bool hasElse);
    Labels getForLabels();
    Labels getWhileLabels();
    Labels getTernaryLabels();
    Labels getAndLabels();
    Labels getOrLabels();
    bool variableExists(string context, string id);
    bool findVariable(string context, string id, Variable& var);
    bool addVariable(string context, string id, int type);
    bool addArray(string context, string id, int type, int size);
    bool addFunction(string id, int type, int paramCount);

};


#endif