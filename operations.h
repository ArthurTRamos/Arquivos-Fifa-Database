#ifndef OPERATIONS_H
    #define OPERATIONS_H

    /*
        Arquivo:
            Direciona cada funcionalidade para
            suas respectivas funções específicas
    */

    #include "fileFunctions.h"
    #include "funcoes_fornecidas.h"
    
    // Funcionalidade 1
    void Operation1(char* inFileName, char* outFileName);
    
    // Funcionalidade 2
    void Operation2(char* inFileName);
    
    // Funcionalidade 3
    void Operation3(char* inFileName, int numberOfSearches);

    // Funcionalidade 4
    void Operation4(char* inFileName, char* indexFileName);

    // Funcionalidade 5
    void Operation5(char* inFileName, char* indexFileName, int numberOfRemotions);

    // Funcionalidade 6
    void Operation6(char* inFileName, char* IndexFileName);

#endif