#ifndef OPERATIONS_H
    #define OPERATIONS_H

    /*
        Arquivo:
            Direciona cada funcionalidade para
            suas respectivas funções específicas
    */

    #include "fileFunctions.h"
    
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
    void Operation6(char* inFileName, char* IndexFileName, int insertions);

    // Funcionalidade 7
    void Operation7(char* inFileName, char* bTreeFileName);

    // Funcionalidade 8
    void Operation8(char* inFileName, char* bTreeFileName, int searches);

    // Funcionalidade 9
    void Operation9(char* inFileName, char* bTreeFileName, int searches);

    // Funcionalidade 10
    void Operation10(char* inFileName, char* bTreeFileName, int insertions);

#endif