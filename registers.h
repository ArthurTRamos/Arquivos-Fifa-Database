#ifndef REGISTERS_H
    #define REGISTERS_H

    #include "bTree.h"

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
    void Inserir(FILE* inFile, int NInsercoes, int nextByteOffset, FILE* treeFile, bool bTree);

    // Remove os registros pedidos
    bool RemoveBinData(char* inFileName, char* indexFileName, int duplas, char** campName,
    char** campValue, int n, HEADER* headerOp);

    // Remove o registro por ID
    bool RemoveBinDataID(FILE* binFile, char* indexFileName, int duplas, char** campName,
    char** campValue, int n, int IDValue, HEADER** headerOp);

    // Busca binária de um ID no arquivo de índice
    long long binarySearchID(int* IDVector, long long* byteOffSetVector, int IDValue, int begin,
    int end);

    // Inserção do removido na lista encadeada ordenada
    void sortedInsertion(FILE* binFile, long long currentByteoffset, long long previousByteoffset, int newRegisterSize, long long newByteoffset, HEADER** headerOp);
    
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

    // Faz uma busca na árvore b sem o id
    void searchWithoutId(FILE* inFile, int buscaAtual, char** slots, int nSlots);

#endif