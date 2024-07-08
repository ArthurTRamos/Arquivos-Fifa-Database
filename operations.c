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
    // FUNCIONALIDADE 4
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

    // ITERA UMA VEZ PRA CADA REMOÇÃO
    for(int i = 0; i < numberOfRemotions; i++) {
        // Erro na remoção de número i
        if(!RemotionBinFile(inFileName, indexFileName, i, headerOp)) {
            printf("Falha no processamento do arquivo.\n");
            return;
        }
    }

    FreeMemoryHeader(&headerOp);

    // Cria o segundo arquivo de índice
    //FUNCIONALIDADE 4
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
void Operation6(char* inFileName, char* IndexFileName, int insertions) {
    // Inserção dos registros
    FILE* inFile;

    inFile = fopen(inFileName, "rb+");
    if(inFile == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    if(!Insert(inFile, insertions, NULL, false)) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    binarioNaTela(inFileName);
    Operation4(inFileName, IndexFileName); // Criação do último índice
}

// Controle da operação 7
void Operation7(char* inFileName, char* bTreeFileName) {
    FILE* inFile, *treeFile;

    // Abre o arquivo binário para leitura
    if((inFile = fopen(inFileName, "rb")) == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Abre o arquivo da árvore b para leitura/escrita
    if((treeFile = fopen(bTreeFileName, "wb+")) == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(inFile);
        return;
    }

    // Cria a árvore b
    if(!CreateBTree(inFile, treeFile)) {
        printf("Falha no processamento do arquivo.\n");
        fclose(inFile);
        fclose(treeFile);
    }
    else {
        fclose(inFile);
        fclose(treeFile);
        binarioNaTela(bTreeFileName);
    }
}

// Controle da operação 8
void Operation8(char* inFileName, char* bTreeFileName, int searches) {
    FILE* inFile, *treeFile;
    char status;

    // Abre o arquivo binário para leitura
    if((inFile = fopen(inFileName, "rb")) == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Verifica a consistência do arquivo binário
    fread(&status, sizeof(char), 1, inFile);
    if(status == '0') {
        fclose(inFile);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Abre o arquivo da árvore b
    if((treeFile = fopen(bTreeFileName, "rb+")) == NULL) {
        fclose(inFile);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Verifica a consistência do arquivo árvore b
    fread(&status, sizeof(char), 1, treeFile);
    if(status == '0') {
        fclose(inFile);
        fclose(treeFile);
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    
    char stringID[3]; // Leitura da string "id"
    int IDtoBeFound; // Leitura do valor do id

    // Faz as buscas necessárias
    for(int i = 0; i < searches; i++) {
        scanf("%s %d", stringID, &IDtoBeFound);
        SearchWithIdAbTree(treeFile, inFile, i, IDtoBeFound, true);
    }

    fclose(inFile);
    fclose(treeFile);
}

// Controle da operação 9
void Operation9(char* inFileName, char* bTreeFileName, int searches) {
    searchControl09(inFileName, bTreeFileName, searches);
}

// Controle da operação 10
void Operation10(char* inFileName, char* bTreeFileName, int insertions) {
    FILE* inFile;
    FILE* bTreeFile;
    char status;

    inFile = fopen(inFileName, "rb+");
    if(inFile == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    fread(&status, sizeof(char), 1, inFile);
    if(status == '0') {
        fclose(inFile);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    bTreeFile = fopen(bTreeFileName, "rb+");
    if(bTreeFile == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(inFile);
        return;
    }

    if(status == '0') {
        fclose(inFile);
        fclose(bTreeFile);
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    
    if(!Insert(inFile, insertions, bTreeFile, true)) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    binarioNaTela(inFileName);
    binarioNaTela(bTreeFileName);
}