#ifndef LISTA_H
    #define LISTA_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>

    typedef struct no NO;

    /*
        Arquivo:
            Contém as funções para manipulação da lista
            de registros removidos do arquivo binário
    */
    
    // Cria um nó
    NO* CreateNo();

    // Limpa/Desaloca a lista/seus nós
    void ClearList(FILE* inFile);

    // Cria a lista dado um arquivo binário
    int CreateList(FILE* inFile);

    // Procura por um nó com tamanho maior ou igual a tam
    long int FindNo(int tam, FILE* inFile);

    // Insere um nó/registro de tamanho tam
    void InserirNo(int tam, long int ByteOffSet, long int currentByteOffSet);

#endif