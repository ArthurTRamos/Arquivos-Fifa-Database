#include "operations.h"

// Cntrole da operação 1
void Operation1(char* inFileName, char* outFileName) {
    // Lê o arquivo .csv e escreve no arquivo .bin
    // Imprime o valor do binário na tela
    if(ReadCsvFile(inFileName) && WriteBinFile(outFileName))
        binarioNaTela(outFileName);
    else
        printf("Falha no processamento do arquivo.\n");
}

// Controle da operação 2
void Operation2(char* inFileName) {
    // Imprime as informações do arquivo binário
    PrintFileInfo(inFileName);
}

// Contole da operação 3
void Operation3(char* inFileName, int numberOfSearches) {
    // Lê arquivo .bin e armazena nos registros
    // Chama "SearchBinFile" para cada busca pedida
    if(ReadBinFile(inFileName)) {
        for(int i = 0; i < numberOfSearches; i++) {
            printf("Busca %d\n\n", i + 1);
            SearchBinFile(i);
        }
    }
    else
        printf("Falha no processamento do arquivo.\n");
}

// Controle da operação 4
void Operation4(char* inFileName, char* indexFileName) {
    long long* bytesOffSet; // Vetor auxiliar de bytesOffSets
    // Lê e Armazena Registros Não removidos do arquivo binário
    if(ReadOnBinFile(inFileName, &bytesOffSet)) {
        SortRegisters(&bytesOffSet); // Ordena Registros
        WriteBinIndex(indexFileName, bytesOffSet); // Escreve no arq. índices
        binarioNaTela(indexFileName);
        free(bytesOffSet);
        bytesOffSet = NULL;
    }
    else
        printf("Falha no processamento do arquivo.\n");
}

// Controle da operação 5
void Operation5(char* inFileName, char* indexFileName, int numberOfRemotions) {
    HEADER* headerOp; // header auxiliar

    long long* bytesOffSetFirst; // Usado na criação do primeiro índice
    long long* bytesOffSetSecond; // Usado na criação do último índice

    // Cria o primeiro arquivo de índice
    if(ReadOnBinFile(inFileName, &bytesOffSetFirst)) {
        SortRegisters(&bytesOffSetFirst);
        WriteBinIndex(indexFileName, bytesOffSetFirst);
        free(bytesOffSetFirst);
        bytesOffSetFirst = NULL;
    }
    else {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    headerOp = CreateHeader();
    // Efetua as remoções
    for(int i = 0; i < numberOfRemotions; i++) {
        // Erro na remoção de número i
        if(!RemotionBinFile(inFileName, indexFileName, i, headerOp)) {
            printf("Falha no processamento do arquivo.\n");
            return;
        }
    }

    FreeMemoryHeader(&headerOp);

    // Cria o segundo arquivo de índice
    if(ReadOnBinFile(inFileName, &bytesOffSetSecond)) {
        SortRegisters(&bytesOffSetSecond);
        WriteBinIndex(indexFileName, bytesOffSetSecond);
        free(bytesOffSetSecond);
        bytesOffSetSecond = NULL;
    }
    else {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    binarioNaTela(inFileName);
    binarioNaTela(indexFileName);
}

// Controle da operação 6
void Operation6(char* inFileName, char* IndexFileName) {
    Insert(inFileName); // Inserção dos registros
    binarioNaTela(inFileName);
    Operation4(inFileName, IndexFileName); // Criação do último índice
}