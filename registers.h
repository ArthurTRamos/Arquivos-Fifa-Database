#ifndef REGISTERS_H
    #define REGISTERS_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    #include "list.h"
    #include "funcoes_fornecidas.h"

    typedef struct headerRegister HEADER;
    typedef struct dataRegister DATA;

    /*
        Arquivo:
            Possui as estruturas de dados para os
            registros de cabeçalho e de dados e define
            funções de manipulação desses nos arquivos binários,
            de texto e de índice
    */

    // Aloca espaço para o registro de cabeçalho
    HEADER* CreateHeader();

    // Aloca espaço para um número de registros de dados
    DATA** CreateData(int registerNumber);

    // Armazena os dados do arquivo .csv nos registros
    bool StoreCsvInfo(char** registers, int registerNumber);

    // Escreve os registros de cabeçalho e de dados no arquivo .bin
    void WriteBinHeader(FILE* outFile);
    void WriteBinData(FILE* outFile);

    // Lê uma quantidade de registros do arquivo .bin
    void GetData(FILE* inFile, int n_reg);

    // Imprime as informações do jogador de um registro
    void PrintData(DATA* data);

    // Armazena os dados do arquivo .bin nos registros
    bool StoreBinInfo(FILE* inFile, int onReg, int offReg);

    // Armazena os dados não removidos do arquivo .bin nos registros
    bool StoreOnBinInfo(FILE* inFile, int onReg, int offReg, long long** bytesOffSet);

    // Faz uma busca i nos registros de dados
    void SearchBinData(int duplas, char** campName, char** campValue, int i);

    // Insere os novos registros no arquivo binário
    void Inserir(FILE* inFile, int NInsercoes, int teste);

    // Remove os registros pedidos
    bool RemoveBinData(char* inFileName, char* indexFileName, int duplas, char** campName,
    char** campValue, int n, HEADER* headerOp);

    // Remove o registro por ID
    bool RemoveBinDataID(FILE* binFile, char* indexFileName, int duplas, char** campName,
    char** campValue, int n, int IDValue, HEADER** headerOp);

    // Busca binária de um ID no arquivo de índice
    long long binarySearchID(int* IDVector, long long* byteOffSetVector, int IDValue, int begin,
    int end);

    // Insere um novo registro removido/byteoffset no final da lista de removidos
    void insertRemovedNonSortedList(long long topo, FILE* binFile, long long byteOffSetToBeInserted, HEADER** headerOp);

    // Obtém e ordena a lista de removidos
    void removedSortedList(long long topo, FILE* binFile, long long* byteOffSetVector, int* sizeRegisterVector, int keepTrack, HEADER** headerOp);
    
    // Ordena a lista de removidos por tamanho do registro
    // e Reescreve no arquivo binário
    void SortListRemove(int **sizeRegisterVector, long long** bytesOffSet, HEADER* headerOp);
    void QuickSortRemove(int begin, int end, int **sizeRegisterVector, long long** bytesOffSet);
    int SortElementsRemove(int begin, int end, int **sizeRegisterVector, long long** bytesOffSet);
    void writeSortedList(int* sizeRegisterVector, long long* byteOffSetVector, FILE* binFile, int counter, HEADER* headerOp);

    // Ordena os registros de dados por ID
    void SortRegisters(long long** bytesOffSet);
    void QuickSort(int begin, int end, long long** bytesOffSet);
    int SortElements(int begin, int end, long long** bytesOffSet);
    void SwapRegisters(int reg1, int reg2);

    // Escreve no arquivo de índice
    void WriteRegistersIndex(FILE* indexFile, long long* bytesOffSet);

    // Libera memória dos registros de cabeçalho e de dados
    void FreeMemoryHeader(HEADER** header);
    void FreeMemoryData(DATA*** data, int registers);

#endif