#include "aux.h"

string Aux:: nextTemp()
{
    for (int i = 0; i < 10; i++)
    {
        if(temp_regs[i] == 1)
            continue;

        temp_regs[i] = 1;
        return "$t"+to_string(i);
    }

    return "";
}

string Aux:: nextSaved()
{
    for (int i = 0; i < 10; i++)
    {
        if(temp_regs[i] == 1)
            continue;

        temp_regs[i] = 1;
        return "$s"+to_string(i);
    }

    return "";
}

bool Aux::findVariable(string context, string id, Variable& var) {

    bool inContext = false;
    bool inGlobal = false;

    if(contexts[context].count(id) != 0) {
        var = contexts[context].at(id);
        inContext = true;
    }
    else
    if(contexts["global"].count(id) != 0) {
        var = contexts["global"].at(id);
        inGlobal = true;
    }

    if (var.stackPos == -1)
        return findVariable(context, var.id, var);

    return inContext || inGlobal;
}

bool Aux::variableExists(string context, string id) {
    if(contexts[context].find(id) != contexts[context].end()) {
        return true;
    }

    return false;
}

bool Aux::addVariable(string context, string id, int type) {
    Variable var;
    
    if(contexts[context].count(id) != 0)
        return false;

    if(context == "global")
        var.isGlobal = true;
    else
        var.isGlobal = false;

    var.type = type;

    var.size = type == 2 ? 1 : 4;

    var.isArray = false;
    var.isFunc = false;

    contexts[context][id] = var;

    return true;
}

bool Aux::addArray(string context, string id, int type, int size) {
    Variable var;
    
    if(contexts[context].count(id) != 0)
        return false;

    if(context == "global")
        var.isGlobal = true;
    else
        var.isGlobal = false;

    var.type = type;
    var.isArray = true;
    var.length = size;
    var.size = type == 2 ? size : size * 4;

    var.isFunc = false;

    contexts[context][id] = var;

    return true;
}

bool Aux::addFunction(string id, int type, int paramCount) {
    Variable var;
    
    if(contexts["global"].count(id) != 0)
        return false;

    var.isGlobal = true;

    var.type = type;
    var.isArray = false;

    var.isFunc = true;
    var.paramCount = paramCount;

    contexts["global"][id] = var;

    return true;
}

string Aux:: nextConst()
{
    return "const"+to_string(const_count++);
}

bool Aux:: releaseTemp(string reg)
{
    char last = reg.back();
    int index = last - '0';

    if(temp_regs[index] == 0)
        return false;
    
    temp_regs[index] = 0;
    return true;
}

Labels Aux::getIfLabels(bool hasElse)
{
    Labels labels;  

    labels.end_label = "endif_"+to_string(if_count);
    
    if(hasElse)
        labels.statement_label = "falseif_"+to_string(if_count);
    else
        labels.statement_label = "endif_"+to_string(if_count);

    if_count++;

    return labels;
}

Labels Aux::getForLabels()
{
    Labels labels;  

    labels.statement_label = "for_"+to_string(for_count);
    labels.end_label = "endfor_"+to_string(for_count);
    for_count++;

    return labels;
}

Labels Aux::getWhileLabels()
{
    Labels labels;  

    labels.statement_label = "while_"+to_string(while_count);
    labels.end_label = "endwhile_"+to_string(while_count);
    while_count++;

    return labels;
}

Labels Aux::getTernaryLabels()
{
    Labels labels;  

    labels.statement_label = "falseternary_"+to_string(ternary_count);
    labels.end_label = "endternary_"+to_string(ternary_count);
    ternary_count++;

    return labels;
}

Labels Aux::getAndLabels()
{
    Labels labels;  

    labels.statement_label = "falseand_"+to_string(and_count);
    labels.end_label = "endand_"+to_string(and_count);
    and_count++;

    return labels;
}

Labels Aux::getOrLabels()
{
    Labels labels;  

    labels.statement_label = "falseor_"+to_string(or_count);
    labels.end_label = "endor_"+to_string(or_count);
    or_count++;

    return labels;
}