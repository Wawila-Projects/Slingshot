#include "ast.h"
#include "aux.h"

Aux * aux = new Aux();

void addToData(string id, string type, string value) {
    codedata += id + ": " + type + " " + value + "\n";
}

Code ExprList::generateCode(){
    Code output;
    output.code = "";
    for(auto& e: content) {
        Code E = e->generateCode();
        aux->releaseTemp(E.place);
        output.place = aux->nextTemp();
        output.code += E.code;
    }
    return output;
}

Code StmntList::generateCode(){
    Code output;
    output.code = "";
    for(auto& e: content) {
        Code E = e->generateCode();
        output.code += E.code;
    }
    return output;
}

Code TernaryExpr::generateCode(){
    Code output;
    Code cond = condition->generateCode();
    Code E1 = truevalue->generateCode();
    Code E2 = falsevalue->generateCode();
    Labels L = aux->getTernaryLabels();;

    aux->releaseTemp(cond.place);
    aux->releaseTemp(E1.place);
    aux->releaseTemp(E2.place);

    output.place = aux->nextTemp();

    output.code = cond.code;
    output.code += "\tbeqz " + cond.place + ", " + L.statement_label + "\n";
    output.code += E1.code;
    output.code += "\tmove " + output.place + ", " + E1.place + "\n";
    output.code += "\tj " + L.end_label + "\n";
    output.code += L.statement_label+":\n";
    output.code += E2.code;
    output.code += "\tmove " + output.place + ", " + E2.place + "\n";
    output.code += L.end_label+":\n";


    return output;
}

Code IdExpr::generateCode(){
    Code output;
    
    Variable var;
    aux->findVariable(context, id, var);

    output.isNumeric = false;
    output.place = aux->nextTemp();

    if(var.type == 2) {
        output.code = "\tlb " + output.place + ", " + var.id + " \n";
    } 
    else if (var.type == 3) {
        output.code = "\tlw " + output.place + ", " + var.id + " \n";
    }

    return output;
}

Code IntExpr::generateCode(){
    Code output;
    
    output.place = aux->nextTemp();
    output.code = "\tli " + output.place + ", " + to_string(value) + " \n";
    
    output.isNumeric = true;
    output.value = value;

    return output;
}

Code CharExpr::generateCode(){
    Code output;
    output.place = aux->nextTemp();

    output.isNumeric = false;
    output.code = "\tli " + output.place + ", '" + value + "' \n";

    return output;
} 

Code ArrayCallExpr::generateCode(){
    Code output;
    Code I = index->generateCode();
    output.place = aux->nextTemp();
    aux->releaseTemp(I.place);
    
    
    output.code = I.code;
    output.isNumeric = false;
    
    Variable var;
    aux->findVariable(context, name, var);
    

    if(var.type == 3) {
        output.code += "\tsll " + I.place + ", " + I.place + ", 2\n"; 
        output.code += "\tla " + output.place + ", " + var.id + "\n";
    } else {
        output.code += "\tla " + output.place + ", " + var.id + "\n";
    }
    
    output.code += "\tadd " + output.place + ", " + output.place + ", " + I.place + "\n";
    output.code += "\tlw " + output.place + ", (" + output.place + ") \n";
 
    return output;
}

Code FunctionCallExpr::generateCode(){
    Code output;

    int args_count = 0;
    string carry = "";
    for(auto& a : args->content) {

        if(IdExpr * var = dynamic_cast<IdExpr *> (a)) {
            Code E = a->generateCode();
            carry += E.code;
            aux->releaseTemp(E.place);
            carry += "\tmove $a" + to_string(args_count) + ", " + E.place + "\n";
            args_count++;    
        }
        else
        {
            Code E = a->generateCode();
            output.code += E.code;
            aux->releaseTemp(E.place);
            output.code += "\tmove $a" + to_string(args_count) + ", " + E.place + "\n";
            args_count++;
        }
    }

    output.code += carry;
    output.code += "\tjal " + name + "\n";
    output.place = "$v0";

    return output;
}

Code PlusExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();
    
    aux->releaseTemp(E1.place);
    aux->releaseTemp(E2.place);

    if(E1.isNumeric && E2.isNumeric)
    {
        output.isNumeric = true;
        output.value = E1.value + E2.value; 
        output.place = aux->nextTemp();
        output.code = "\tli " + output.place + " ," + to_string(output.value) + "\n";
    } 
    else if(E1.isNumeric && !E2.isNumeric)
    {
        output.code = E2.code;
        output.isNumeric = false;
        output.place = aux->nextTemp();
        output.code += "\taddi " + output.place +  ", " + E2.place + ", " + to_string(E1.value) + "\n";
    }
    else if(!E1.isNumeric && E2.isNumeric)
    {
        output.code = E1.code;
        output.isNumeric = false;
        output.place = aux->nextTemp();
        output.code += "\taddi " + output.place +  ", " + E1.place + ", " + to_string(E2.value) + "\n";
    }
    else
    {
        output.code = E1.code + E2.code;
        output.isNumeric = false;
        output.place = aux->nextTemp();
        output.code += "\tadd " + output.place + ", " + E1.place + ", " + E2.place + "\n";
    }

    return output;
}

Code SubExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();
    aux->releaseTemp(E1.place);
    aux->releaseTemp(E2.place);

    if(E1.isNumeric && E2.isNumeric)
    {
        
        output.isNumeric = true;
        output.value = E1.value - E2.value; 
        output.place = aux->nextTemp();
        output.code += "\tli " + output.place + " ," + to_string(output.value) + "\n";
    } 
    else if(!E1.isNumeric && E2.isNumeric)
    {
        output.code = E1.code;
        output.isNumeric = false;
        output.place = aux->nextTemp();
        output.code += "\taddi " + output.place +  ", " + E1.place + ", -" + to_string(E2.value) + "\n";
    }
    else
    {
        output.code = E1.code + E2.code;
        output.isNumeric = false;
        output.place = aux->nextTemp();
        output.code += "\tsub " + output.place + ", " + E1.place + ", " + E2.place + "\n";
    }

    return output;
}

Code MulExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();
    output.code = E1.code + E2.code;
    aux->releaseTemp(E1.place);
    aux->releaseTemp(E2.place);
    
    if(E1.isNumeric && E2.isNumeric)
    {
        output.isNumeric = true;
        output.value = E1.value * E2.value;
        output.place = aux->nextTemp();
        output.code = "\tli " + output.place + " ," + to_string(output.value) + "\n";

        return output;
    }

    output.code = E1.code + E2.code;
    output.place = aux->nextTemp();
    output.code += "\tmove $a0, " + E1.place + "\n";
    output.code += "\tmove $a1, " + E2.place + "\n";
    output.code += "\tjal mult\n";
    output.code += "\tmove " + output.place + ", $v0\n";

    return output;
}

Code DivExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();
    output.code = E1.code + E2.code;
    aux->releaseTemp(E1.place);
    aux->releaseTemp(E2.place);
    
    if(E1.isNumeric && E2.isNumeric)
    {
        output.isNumeric = true;
        output.value = E1.value / E2.value;
        output.place = aux->nextTemp();
        output.code = "\tli " + output.place + " ," + to_string(output.value) + "\n";

        return output;
    }
    
    output.code = E1.code + E2.code;
    output.place = aux->nextTemp();

    output.code += "\tmove $a0, " + E1.place + "\n";
    output.code += "\tmove $a1, " + E2.place + "\n";
    output.code += "\tla $a2, lo\n";
    output.code += "\tla $a3, hi\n";
    output.code += "\tjal divide\n";
    output.code += "\tlw " + output.place + ", lo\n";


    return output;
}

Code ModExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();
    output.code = E1.code + E2.code;
    aux->releaseTemp(E1.place);
    aux->releaseTemp(E2.place);
    
    if(E1.isNumeric && E2.isNumeric)
    {
        output.isNumeric = true;
        output.value = E1.value % E2.value;
        output.place = aux->nextTemp();
        output.code = "\tli " + output.place + " ," + to_string(output.value) + "\n";

        return output;
    }

    output.isNumeric = false;
    //output.code = E1.code + E2.code;
    output.place = aux->nextTemp();

    output.code += "\tmove $a0, " + E1.place + "\n";
    output.code += "\tmove $a1, " + E2.place + "\n";
    output.code += "\tla $a2, lo\n";
    output.code += "\tla $a3, hi\n";
    output.code += "\tjal divide\n";
    output.code += "\tlw " + output.place + ", hi\n";
    
    return output;
}

Code GreaterThanExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();
    output.code = E1.code + E2.code;
    aux->releaseTemp(E1.place);
    aux->releaseTemp(E2.place);

    output.place = aux->nextTemp();
    output.code += "\tslt " + output.place + ", " + E2.place + ", " + E1.place + "\n";
    
    return output;
}

Code LesserThanExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();
    output.code = E1.code + E2.code;
    aux->releaseTemp(E1.place);
    aux->releaseTemp(E2.place);

    output.place = aux->nextTemp();
    output.code += "\tslt " + output.place + ", " + E1.place + ", " + E2.place + "\n";
    
    return output;
}

Code LesserEqualExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();
    output.code = E1.code + E2.code;
    aux->releaseTemp(E1.place);
    aux->releaseTemp(E2.place);

    output.place = aux->nextTemp();
    output.code += "\tsle " + output.place + ", " + E1.place + ", " + E2.place + "\n";
    
    return output;
}

Code GreaterEqualExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();
    output.code = E1.code + E2.code;
    aux->releaseTemp(E1.place);
    aux->releaseTemp(E2.place);

    output.place = aux->nextTemp();
    output.code += "\tsge " + output.place + ", " + E1.place + ", " + E2.place + "\n";
    
    return output;
}

Code EqualExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();
    output.code = E1.code + E2.code;
    aux->releaseTemp(E1.place);
    aux->releaseTemp(E2.place);

    output.place = aux->nextTemp();
    output.code += "\tseq " + output.place + ", " + E1.place + ", " + E2.place + "\n";
    
    return output;
}

Code NotEqualExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();
    output.code = E1.code + E2.code;
    aux->releaseTemp(E1.place);
    aux->releaseTemp(E2.place);

    output.place = aux->nextTemp();
    output.code += "\tsne " + output.place + ", " + E1.place + ", " + E2.place + "\n";
    
    return output;
}

Code AssignExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode(); 
    output.isNumeric = false;

    if(IdExpr * id = dynamic_cast<IdExpr *>(expr1)) {
        aux->releaseTemp(E1.place);
        aux->releaseTemp(E2.place);
        output.place = aux->nextTemp();

        output.code = E1.code + E2.code;
        Variable V;
        aux->findVariable(context, id->id, V);
        output.code += "\tsw " + E2.place + ", " + V.id + "\n";
        return output;
    }

    if(ArrayCallExpr * arr = dynamic_cast<ArrayCallExpr *>(expr1)) {
        output.place = aux->nextTemp();
        aux->releaseTemp(E1.place);
        
        Code I = arr->index->generateCode();
        aux->releaseTemp(I.place);
        
        output.code = E2.code;
        output.code += I.code;
        output.isNumeric = false;
        
        Variable var;
        aux->findVariable(context, arr->name, var);
        
            
        if(var.type == 3) {
            output.code += "\tsll " + I.place + ", " + I.place + ", 2\n"; 
            output.code += "\tla " + output.place + ", " + var.id + "\n";
        } else {
            output.code += "\tla " + output.place + ", " + var.id + "\n";
        }

    
        
        output.code += "\tadd " + output.place + ", " + output.place + ", " + I.place + "\n";
        output.code += "\tsw " + E2.place + ", (" + output.place + ")\n"; 
        
        aux->releaseTemp(E2.place);
    }
    
    return output;
}

Code PlusAssignExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();
    
    aux->releaseTemp(E2.place);

    output.code = E1.code + E2.code;
    
    if(IdExpr * id = dynamic_cast<IdExpr *>(expr1)) {
        Variable V;
        aux->findVariable(context, id->id, V);
        output.code += "\tadd " + E1.place + ", " + E1.place + ", " + E2.place + "\n";
        output.code += "\tsw " + E1.place + ", " + V.id + "\n";
    }

    output.place = E1.place;

    return output;
}

Code SubAssignExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();
    
    aux->releaseTemp(E2.place);

    output.code = E1.code + E2.code;

    if(IdExpr * id = dynamic_cast<IdExpr *>(expr1)) {
        Variable V;
        aux->findVariable(context, id->id, V);
        output.code += "\tsub " + E1.place + ", " + E1.place + ", " + E2.place + "\n";
        output.code += "\tsw " + E1.place + ", " + V.id + "\n";
    }

    output.place = E1.place;

    return output;
}

Code MulAssignExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();
    output.code = E1.code + E2.code;
    aux->releaseTemp(E2.place);

    output.code = E1.code + E2.code;
    output.place = E1.place;
    output.code += "\tmove $a0, " + E1.place + "\n";
    output.code += "\tmove $a1, " + E2.place + "\n";
    output.code += "\tjal mult\n";

    if(IdExpr * id = dynamic_cast<IdExpr *>(expr1)) {
        Variable V;
        aux->findVariable(context, id->id, V);
        output.code += "\tsw  $v0, " + V.id + "\n";
    }

    return output;
}

Code DivAssignExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();
    output.code = E1.code + E2.code;
    aux->releaseTemp(E2.place);

    output.code = E1.code + E2.code;
    output.place = E1.place;

    output.code += "\tmove $a0, " + E1.place + "\n";
    output.code += "\tmove $a1, " + E2.place + "\n";
    output.code += "\tla $a2, lo\n";
    output.code += "\tla $a3, hi\n";
    output.code += "\tjal divide\n";
    output.code += "\tlw " + output.place + ", lo\n";

    if(IdExpr * id = dynamic_cast<IdExpr *>(expr1)) {
        Variable V;
        aux->findVariable(context, id->id, V);
        output.code += "\tsw  " + output.place + ", "+ V.id + "\n";
    }

    return output;
    
}

Code ModAssignExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();
    output.code = E1.code + E2.code;
    aux->releaseTemp(E2.place);

    output.code = E1.code + E2.code;
    output.place = E1.place;

    output.code += "\tmove $a0, " + E1.place + "\n";
    output.code += "\tmove $a1, " + E2.place + "\n";
    output.code += "\tla $a2, lo\n";
    output.code += "\tla $a3, hi\n";
    output.code += "\tjal divide\n";
    output.code += "\tlw " + output.place + ", hi\n";

    if(IdExpr * id = dynamic_cast<IdExpr *>(expr1)) {
        Variable V;
        aux->findVariable(context, id->id, V);
        output.code += "\tsw  " + output.place + ", "+ V.id + "\n";
    }

    return output;
}

Code RShiftAssignExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();
    
    aux->releaseTemp(E2.place);

    output.code = E1.code + E2.code;

    if(IdExpr * id = dynamic_cast<IdExpr *>(expr1)) {
        Variable V;
        aux->findVariable(context, id->id, V);
        output.code += "\tsrl " + E1.place + ", " + E1.place + ", " + E2.place + "\n";
        output.code += "\tsw " + E1.place + ", " + V.id + "\n";
    }

    output.place = E1.place;

    return output;
}

Code LShiftAssignExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();
    
    aux->releaseTemp(E2.place);

    output.code = E1.code + E2.code;
    
    if(IdExpr * id = dynamic_cast<IdExpr *>(expr1)) {
        Variable V;
        aux->findVariable(context, id->id, V);
        output.code += "\tsll " + E1.place + ", " + E1.place + ", " + E2.place + "\n";
        output.code += "\tsw " + E1.place + ", " + V.id + "\n";
    }

    output.place = E1.place;

    return output;
}

Code XorAssignExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();
    
    aux->releaseTemp(E2.place);

    output.code = E1.code + E2.code;

    if(IdExpr * id = dynamic_cast<IdExpr *>(expr1)) {
        Variable V;
        aux->findVariable(context, id->id, V);
        output.code += "\txor " + E1.place + ", " + E1.place + ", " + E2.place + "\n";
        output.code += "\tsw " + E1.place + ", " + V.id + "\n";
    }

    output.place = E1.place;

    return output;
}

Code OrAssignExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();
    
    aux->releaseTemp(E2.place);

    output.code = E1.code + E2.code;
    
    if(IdExpr * id = dynamic_cast<IdExpr *>(expr1)) {
        Variable V;
        aux->findVariable(context, id->id, V);
        output.code += "\tor " + E1.place + ", " + E1.place + ", " + E2.place + "\n";
        output.code += "\tsw " + E1.place + ", " + V.id + "\n";
    }

    output.place = E1.place;

    return output;
}

Code AndAssignExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();
    
    aux->releaseTemp(E2.place);

    output.code = E1.code + E2.code;

    if(IdExpr * id = dynamic_cast<IdExpr *>(expr1)) {
        Variable V;
        aux->findVariable(context, id->id, V);
        output.code += "\tand " + E1.place + ", " + E1.place + ", " + E2.place + "\n";
        output.code += "\tsw " + E1.place + ", " + V.id + "\n";
    }

    output.place = E1.place;

    return output;
}

Code LogicalAndExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();

    aux->releaseTemp(E1.place);
    aux->releaseTemp(E2.place);

    output.place = aux->nextTemp();
    output.code = E1.code + E2.code;
    output.code += "\tsne " + E1.place + ", " + E1.place + ", $zero\n";
    output.code += "\tsne " + E2.place + ", " + E2.place + ", $zero\n";
    output.code += "\tand " + output.place + ", " + E1.place + ", " + E2.place + "\n";


    return output;
}

Code LogicalOrExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();

    aux->releaseTemp(E1.place);
    aux->releaseTemp(E2.place);

    output.place = aux->nextTemp();
    output.code = E1.code + E2.code;
    output.code += "\tsne " + E1.place + ", " + E1.place + ", $zero\n";
    output.code += "\tsne " + E2.place + ", " + E2.place + ", $zero\n";
    output.code += "\tor " + output.place + ", " + E1.place + ", " + E2.place + "\n";


    return output;
}

Code IncOrExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();

    aux->releaseTemp(E1.place);
    aux->releaseTemp(E2.place);

    output.place = aux->nextTemp();
    output.code = E1.code + E2.code;
    
    output.code += "\tOr " + output.place + ", " + E1.place + ", " + E2.place + "\n"; 
    
    return output;
}

Code XorExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();

    aux->releaseTemp(E1.place);
    aux->releaseTemp(E2.place);

    output.place = aux->nextTemp();
    output.code = E1.code + E2.code;

    output.code += "\txor " + output.place + ", " + E1.place + ", " + E2.place + "\n"; 
    return output;
}

Code AndExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();

    aux->releaseTemp(E1.place);
    aux->releaseTemp(E2.place);

    output.place = aux->nextTemp();
    output.code = E1.code + E2.code;

    output.code += "\tand " + output.place + ", " + E1.place + ", " + E2.place + "\n"; 
    return output;
}

Code LeftShiftExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();

    aux->releaseTemp(E1.place);
    aux->releaseTemp(E2.place);

    output.place = aux->nextTemp();
    output.code = E1.code + E2.code;

    if(IdExpr * id = dynamic_cast<IdExpr *>(expr1)) {
        Variable V;
        aux->findVariable(context, id->id, V);
        output.code += "\tsll " + output.place + ", " + E1.place + ", " + E2.place + "\n"; 
        output.code += "\tsw " + E1.place + ", " + V.id + "\n";
    }

    return output;
}

Code RightShiftExpr::generateCode(){
    Code output;
    Code E1 = expr1->generateCode();
    Code E2 = expr2->generateCode();

    aux->releaseTemp(E1.place);
    aux->releaseTemp(E2.place);

    output.place = aux->nextTemp();
    output.code = E1.code + E2.code;

    if(IdExpr * id = dynamic_cast<IdExpr *>(expr1)) {
        Variable V;
        aux->findVariable(context, id->id, V);
        output.code += "\tsrl " + output.place + ", " + E1.place + ", " + E2.place + "\n"; 
        output.code += "\tsw " + E1.place + ", " + V.id + "\n";
    }

    return output;
}

Code PreIncUnExpr::generateCode(){
    Code output;
    Code E = expr->generateCode();

    aux->releaseTemp(E.place);

    output.place = aux->nextTemp();
    output.code = E.code;

    if(IdExpr * id = dynamic_cast<IdExpr *>(expr)) {
        Variable V;
        aux->findVariable(context, id->id, V);
        output.code += "\taddi " + output.place + ", " + E.place + ", 1\n"; 
        output.code += "\tsw " + E.place + ", " + V.id + "\n";
    }    

    return output;
}

Code PreDecUnExpr::generateCode(){
    Code output;
    Code E = expr->generateCode();

    aux->releaseTemp(E.place);

    output.place = aux->nextTemp();
    output.code = E.code;
    
    if(IdExpr * id = dynamic_cast<IdExpr *>(expr)) {
        Variable V;
        aux->findVariable(context, id->id, V);
        output.code += "\taddi " + output.place + ", " + E.place + ", -1\n"; 
        output.code += "\tsw " + E.place + ", " + V.id + "\n";
    }

    
    return output;
}

Code NegativeUnExpr::generateCode(){
    Code output;
    Code E = expr->generateCode();

    aux->releaseTemp(E.place);

    output.place = aux->nextTemp();
    output.code = E.code;

    if(IdExpr * id = dynamic_cast<IdExpr *>(expr)) {
        Variable V;
        aux->findVariable(context, id->id, V);
        output.code += "\tneg " + output.place + ", " + E.place + "\n"; 
        output.code += "\tsw " + E.place + ", " + V.id + "\n";
    }

    return output;
}

Code BitwiseNotUnExpr::generateCode(){
    Code output;
    Code E = expr->generateCode();

    aux->releaseTemp(E.place);

    output.place = aux->nextTemp();
    output.code = E.code;

    if(IdExpr * id = dynamic_cast<IdExpr *>(expr)) {
        Variable V;
        aux->findVariable(context, id->id, V);
        output.code += "\tnot " + output.place + ", " + E.place + "\n"; 
        output.code += "\tsw " + E.place + ", " + V.id + "\n";
    }

    return output;
}

Code NotUnExpr::generateCode(){
    Code output;
    Code E = expr->generateCode();
    aux->releaseTemp(E.place);
    
    output.place = aux->nextTemp();
    output.code = E.code;

    if(IdExpr * id = dynamic_cast<IdExpr *>(expr)) {
        Variable V;
        aux->findVariable(context, id->id, V);
        output.code += "\tsne " + output.place + ", " + E.place + ", $zero\n";
        output.code += "\tsw " + E.place + ", " + V.id + "\n";
    }

    return output;
}

Code PostIncUnExpr::generateCode(){
    Code output;
    Code E = expr->generateCode();

    aux->releaseTemp(E.place);

    output.place = aux->nextTemp();
    output.code = E.code;

    if(IdExpr * id = dynamic_cast<IdExpr *>(expr)) {
        Variable V;
        aux->findVariable(context, id->id, V);
        output.code += "\taddi " + output.place + ", " + E.place + ", 1\n";
        output.code += "\tsw " + E.place + ", " + V.id + "\n";
    }


    return output;
}

Code PostDecUnExpr::generateCode(){
    Code output;
    Code E = expr->generateCode();

    aux->releaseTemp(E.place);

    output.place = aux->nextTemp();
    output.code = E.code;

    if(IdExpr * id = dynamic_cast<IdExpr *>(expr)) {
        Variable V;
        aux->findVariable(context, id->id, V);
        output.code += "\t " + output.place + ", " + E.place + ", -1\n"; 
        output.code += "\tsw " + E.place + ", " + V.id + "\n";
    }

    return output;
}

Code SizeOfUnExpr::generateCode(){
    Code output;
    
    output.place = aux->nextTemp();

    CharExpr* c = dynamic_cast<CharExpr*>(expr);
    if(c != NULL  || type == 2)
    {
        output.code = "\tli " + output.place + ", 1 \n";
        output.value = 1;
    
        output.isNumeric = true;
        return output;
    }
    
    IntExpr* d = dynamic_cast<IntExpr*>(expr);
    if(d != NULL || type == 3)
    {
        output.code = "\tli " + output.place + ", 4 \n";
        output.value = 4;
    
        output.isNumeric = true;
        return output;
    }

    if(IdExpr * id = dynamic_cast<IdExpr*>(expr)) {
        Variable var;
        if(!aux->findVariable(context, id->id, var))
            return output;

        output.isNumeric = true;
        output.code = "\tli " + output.place + ", " + to_string(var.size) + "\n";
        output.value = var.size;
    }

    return output;
}

/* STATEMENTS */

Code ReturnStmnt::generateCode(){
    Code output;
    
    if(value == NULL) {
        return output;
    }
    
    Code E = value->generateCode(); 
    aux->releaseTemp(E.place);
    output.code = E.code;    

    if(IdExpr * id = dynamic_cast<IdExpr *>(value)) {
        Variable V;
        aux->findVariable(context, id->id, V);
        output.code += "\tlw $v0, " + V.id + "\n";
    }
    else
    if(IntExpr * id = dynamic_cast<IntExpr *>(value)) {
        output.code += "\tli $v0, " + to_string(id->value) + "\n";
    }
    else {
    }
    
    output.code += "\tmove $v0, " + E.place + "\n";
    output.place = "$v0";

    return output;
}

Code IfStmnt::generateCode(){
    Code output;
    Code E = condition->generateCode();
    Code S1 = body->generateCode();

    Code S2 ;
    
    if(optelse != NULL)
        S2 = optelse->generateCode();
    
        Labels L;

    aux->releaseTemp(E.place);

    if(S2.code != "")
        L = aux->getIfLabels(true);
    else
        L = aux->getIfLabels(false);

    output.code = E.code;
    output.code += "\tbeqz " + E.place + ", " + L.statement_label + "\n";
    output.code += S1.code;
    
    if(S2.code == "")
    {
        output.code += L.end_label+":\n";
        return output;
    }
    
    output.code += "\tj " + L.end_label + "\n";
    output.code += L.statement_label+":\n";
    output.code += S2.code;
    output.code += L.end_label+":\n";
    return output;
}

Code WhileStmnt::generateCode(){
    Code output;
    Code E = condition->generateCode();
    Code S = body->generateCode();
    Labels L = aux->getWhileLabels();

    aux->releaseTemp(E.place);

    output.code = L.statement_label+":\n";
    output.code += E.code;
    output.code += "\tbeqz " + E.place + ", " + L.end_label + "\n";
    output.code += S.code;
    output.code += "\tj " + L.statement_label + "\n";
    output.code += L.end_label+":\n";

    return output;
}

Code ForStmnt::generateCode(){
    Code output;
    Code I = id->generateCode();
    Code E = condition->generateCode();
    Code C = change->generateCode();
    Code S = body->generateCode();
    Labels L = aux->getForLabels();

    aux->releaseTemp(I.place);
    aux->releaseTemp(E.place);
    aux->releaseTemp(C.place);

    output.code = I.code;
    output.code += L.statement_label+":\n";
    output.code += E.code;
    output.code += "\tbeqz " + E.place + ", " + L.end_label + "\n";
    output.code += S.code;
    output.code += C.code;
    output.code += "\tj " + L.statement_label + "\n";
    output.code += L.end_label+":\n";

    return output;
}

Code BlockStmnt::generateCode(){
    Code output;
    output.code = "";
    output.isNumeric = false;

    if(declarations != NULL)
        for (auto const& s : declarations->content) {
            Code S = s->generateCode();
            output.code += S.code + "\n";
        }

    if(statements != NULL)
        for (auto const& s : statements->content) {
            Code S = s->generateCode();
            output.code += S.code + "\n";
        }

    return output;
}

Code ExpressionStmnt::generateCode(){
    Code output;
    Code E = expr->generateCode(); 
    
    aux->releaseTemp(E.place);
    output.code = E.code;

    return output;
}

Code VarDeclaration::generateCode(){
    Code output;
    Code E;
    Variable V;

    aux->findVariable(context, id, V);

    if(value != NULL) {
        E = value->generateCode();
        aux->releaseTemp(E.place);
    }

    if(context == "global") {
        string datavalue;

        switch(type){
            case 2: 
                    datavalue = value == NULL ? "0" : to_string(((CharExpr*)value)->value);
                    addToData(id, ".byte", datavalue);
            break;
            case 3: 
                    datavalue = value == NULL ? "0" : to_string(((IntExpr*)value)->value);
                    addToData(id, ".word", datavalue);
            break;
        }
    }
    else {
        string datavalue;
        switch(type){
            case 2: 
                output.code = E.code;
                datavalue = value == NULL ? "$zero" : E.place;
                output.code += "\tsb " + datavalue + ", " + V.id + "\n";
            break;
            case 3: 
                output.code = E.code;
                datavalue = value == NULL ? "$zero" : E.place;
                output.code += "\tsw " + datavalue + ", " + V.id + "\n";
            break;
        }
        
    }
    
    return output;
}

Code ArrayDeclaration::generateCode(){
    Code output;
    string datavalue;

    if(context == "global") {
        if(value != NULL) {  
            switch(type){
                case 2: 
                    datavalue += to_string(((CharExpr*)value->content[0])->value);
                    for(int i = 1; i < size; i++)
                        datavalue += ", " + to_string(((CharExpr*)value->content[i])->value);
                break;
                case 3: 
                    datavalue += to_string(((IntExpr*)value->content[0])->value);
                    for(int i = 1; i < size; i++)
                        datavalue += ", " + to_string(((IntExpr*)value->content[i])->value);
                break;
            }
        } else {
            datavalue += "0";
            for(int i = 1; i < size; i++)
                datavalue += ", 0";
        }

        switch(type){
            case 2: 
                    addToData(id, ".byte", datavalue);
            break;
            case 3: 
                    addToData(id, ".word", datavalue);
            break;
        }
    }
    else {
       /*
            STACK
       */
        
    }

    return output;
}

Code FuncDeclaration::generateCode(){
    Code output;
    return output;
}

Code FuncDefinition::generateCode(){
    Code output;
    Code B = body->generateCode();

    output.code = id + ":\n";
    if(context == "main") {
        output.code += "\tli $a0, BRIGHT_WHITE\n";
        output.code += "\tli $a1, BLACK\n";
        output.code += "\tjal set_color\n";
        output.code += "\tjal clear_screen\n";
    }

    output.code += "\taddi $sp, $sp, -" + to_string(aux->stack[id]) + "\n";
    output.code += "\tsw $ra, ($sp)\n";

    
    for(int i = 0; i < 3; i++) {
        Variable var;
        string name = "$a" + to_string(i);
        if(aux->findVariable(context, name, var)) {
            output.code += "\tsw " + name + ", " + var.id + "\n"; 
        }
    }

    output.code += B.code;

    if(context == "main") {
        output.code += "\taddi $sp, $sp, " + to_string(aux->stack[context]) + "\n";
        output.code += "\tjr $ra\n";
        return output;
    }

    output.code += "\tlw $ra, ($sp)\n";
    output.code += "\taddi $sp, $sp, " + to_string(aux->stack[context]) + "\n";
    output.code += "\tjr $ra\n";
    
   
    return output;
}   

Code ParamDeclaration::generateCode(){
    Code output;
    return output;
}

Code Declarations::generateCode(){
    Code output;
    /*
        List de vardeclaration(int, string);
    */

    output.code = "";
    for(auto& d : decls->content) {
        Code D = d->generateCode();
        output.code += D.code;
    }


    return output;
}

Code PrintfFunc::generateCode(){
    Code output;
    
        output.code = "";
    
        int param_count = 0;
    
        for(int i =1;  i < (int)strlen(text)-1; i++) {
            
            if(text[i] == '\\' && text[i+1] == 'n') {
                output.code += "\tli $a0, '\\n' \n";
                output.code += "\tjal put_char\n";
                i += 2;
                continue;
            }
            
            if(text[i] == '%') {
                i++;
                
                Code E = params->content[param_count++]->generateCode();
                aux->releaseTemp(E.place);
    
                output.code += E.code;
                if(text[i] == 'd') {
                    output.code += "\tmove $a0, " + E.place + "\n";
                    output.code += "\tjal put_udecimal\n";
    
                }
                else if(text[i] == 'c') {
                    output.code += "\tmove $a0, " + E.place + "\n";
                    output.code += "\tjal put_char\n";
                }
    
                continue;
            }
            
    
            output.code += "\tli $a0, '";
            output.code += text[i];
            output.code += "' \n";
            output.code += "\tjal put_char\n";
        }
        
        return output;
}

Code TimeFunc::generateCode(){
    Code output;

    output.place = aux->nextTemp();
    output.code = "\tlw "+ output.place +", MS_COUNTER_REG_ADDR\n";

    return output;

}

Code SrandFunc::generateCode(){
    Code output;
    Code E = param->generateCode();

    aux->releaseTemp(E.place);

    output.code = E.code;
    output.code += "\tadd $a0, " + E.place + ", $zero\n";    
    output.code += "\tjal rand_seed\n";

    return output;
}

Code RandFunc::generateCode(){
    Code output;

    output.isNumeric = false;
    output.place = "$v0";  
    output.code = "\tjal rand\n";

    return output;
}

/* EVALUATES */ 

bool IdExpr::evaluate() {
    bool found = aux->variableExists(context, id);
    bool global = aux->variableExists("global", id);

    return  found || global;
}

bool ArrayCallExpr::evaluate() {
    Variable var;
    bool found = aux->findVariable(context, name, var);

    if(!found)
        return false;

    bool isarray = var.isArray;
    //bool notnegative = index >= 0;
    //bool validrange = index <= var.size;
    
    return isarray;// && notnegative && validrange;
}

bool FunctionCallExpr::evaluate() {
    Variable var;
    bool found = aux->findVariable("global", name, var);

    if(!found)
        return false;

    return (unsigned)var.paramCount == args->content.size();
}

bool BlockStmnt::evaluate() {
    if(aux->stack.count(context) == 0)
    aux->stack[context] = 4;

    if(declarations != NULL && !declarations->evaluate())
        return false;

    if(statements != NULL && !statements->evaluate())
        return false;

    return true;
}

bool VarDeclaration::evaluate() {
    if(aux->variableExists(context, id) || aux->variableExists("global", id))
        return false;

    aux->addVariable(context, id, type);    

    return true;
}

bool ArrayDeclaration::evaluate() {
    if(aux->variableExists(context, id) || aux->variableExists("global", id))
        return false;

    if(size <= 0 && value == NULL)
        return false;

    if(size <= 0) 
        size =  value->content.size();

    aux->addArray(context, id, type, size);    

    return true;
}

bool FuncDeclaration::evaluate() {
    if(aux->variableExists("global", id))
        return false;

    aux->addFunction(id, type, params->content.size());

    if(aux->stack.count(id) == 0)
        aux->stack[id] = 4;
    
    int count;
    for(auto& p : params->content) {
        if(VarDeclaration * var = dynamic_cast<VarDeclaration *>(((ParamDeclaration*)p)->decl))
        {
            aux->addVariable(id, var->id, ((ParamDeclaration*)p)->type);
            aux->contexts[id][var->id].id = "$a" + to_string(count++);
            aux->contexts[id][var->id].stackPos = -1;
            
            if(((ParamDeclaration*)p)->type == 2 ) {
                aux->addVariable(id, aux->contexts[id][var->id].id, 2);   
                aux->contexts[id][aux->contexts[id][var->id].id].id = to_string(aux->stack[id]) + "($sp)";
                aux->contexts[id][aux->contexts[id][var->id].id].stackPos = aux->stack[id];
                aux->stack[id]++;
            }
             else if (((ParamDeclaration*)p)->type == 3) {
                aux->addVariable(id, aux->contexts[id][var->id].id, 3);   
                aux->contexts[id][aux->contexts[id][var->id].id].id = to_string(aux->stack[id]) + "($sp)";
                aux->contexts[id][aux->contexts[id][var->id].id].stackPos = aux->stack[id];
                aux->stack[id] += 4;
            }

            continue;
        }

        if(ArrayDeclaration * var = dynamic_cast<ArrayDeclaration *>(((ParamDeclaration*)p)->decl))
        {
            aux->addArray(id, var->id, ((ParamDeclaration*)p)->type, var->size);
            aux->contexts[id][var->id].id = "$a" + to_string(count++);
            aux->contexts[id][var->id].stackPos = -1;

            if(((ParamDeclaration*)p)->type == 2 ) {
                aux->addArray(id, aux->contexts[id][var->id].id, 2, var->size);   
                aux->contexts[id][aux->contexts[id][var->id].id].id = to_string(aux->stack[id]) + "($sp)";
                aux->contexts[id][var->id].stackPos = aux->stack[id];
                aux->stack[id] += 4;
            }
             else if (((ParamDeclaration*)p)->type == 3) {
                aux->addArray(id, aux->contexts[id][var->id].id, 3, var->size);   
                aux->contexts[id][aux->contexts[id][var->id].id].id = to_string(aux->stack[id]) + "($sp)";
                aux->contexts[id][var->id].stackPos = aux->stack[id];
                aux->stack[id] += 4;
            }

            continue;
        }
    }

    return true;
}

bool Declarations::evaluate() {

    if(aux->stack.count(context) == 0)
        aux->stack[context] = 4;

    int stack = aux->stack[context];

    for(auto& dec : this->decls->content) {
        if(!dec->evaluate())
            return false;
        
        if(VarDeclaration * var = dynamic_cast<VarDeclaration *>(dec))
        {
            Variable v;
            aux->findVariable(context, var->id, v);

            if(v.isGlobal)
                v.id = var->id;
            else
                v.id = to_string(stack) + "($sp)";
                
            v.stackPos = stack;

            if(var->type == 2)
                stack += 1;
            if(var->type == 3)
                stack += 4;

            aux->contexts[context][var->id] = v;

            continue;
        }

        if(ArrayDeclaration * var = dynamic_cast<ArrayDeclaration *>(dec))
        {
            Variable v;
            aux->findVariable(context, var->id, v);
            
            if(v.isGlobal)
                v.id = var->id;
            else
                v.id = to_string(stack) + "($sp)";

            v.stackPos = stack;

            if(var->type == 2)
                stack += v.size;
            if(var->type == 3)
                stack += v.size;

            aux->contexts[context][var->id] = v;
            
            continue;
        }
    }

    aux->stack[context] = stack;
    return true;
}

bool PrintfFunc::evaluate() {

    if(params != NULL && !params->evaluate())
        return false;

    unsigned count = 0;

    for(unsigned i = 0; i < strlen(text); i++) {
        if(text[i] == '%') {
            if(i >= strlen(text))
                return false;
            i++;
            if(text[i] != 'd' && text[i] != 'c')
                return false;

            count++;
        }
    }

    if(params == NULL && count == 0)
        return true;

    return params->content.size() == count;
}