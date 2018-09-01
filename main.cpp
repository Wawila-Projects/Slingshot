#include <stdio.h>
#include "ast.h"
#include "token.h"

extern Stmnt *input;

int yyparse();
extern FILE *yyin;
string codedata;

int main(int argc, char *argv[])
{

    input = 0;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s No enough arguments\n", argv[0]);
        return 1;
    }

    yyin  = fopen(argv[1], "rb");
    
    if (yyin == NULL) {
        fprintf(stderr, "Cannot open file %s\n", argv[1]);
        return 2;
    }
    
    yyparse();
    
    string prologo = "#include \"screen.h\"\n#include \"system.h\"\n.global main\n\n";

    codedata += ".data\n";
    codedata += "lo: .word 0\n";
    codedata += "hi: .word 0\n";

    string text = ".text\n\n";

    if (input != 0) {
        input->setContext("global");
        
        bool isCorrect = input->evaluate();
        if(!isCorrect)
        {
            printf("ERROR SEMANTICO\n");
            return 3;
        }
        
        string inputcode = input->generateCode().code;
        string code = prologo + codedata + text + inputcode;
        printf("%s\n", code.c_str());
    }
}
