#ifndef FILE_FUNCTIONS_H
    #define FILE_FUNCTIONS_H

    /*
        Arquivo:
            Contém as funções específicas para
            manipulação dos arquivos .csv e .bin
    */

    #include "registers.h"

    // Lê um arquivo .csv
    bool ReadCsvFile(char* inFileName);

    // Escreve os registros num arquivo .bin
    bool WriteBinFile(char *outFileName);

    // Imprime as informações de todos os jogadores
    void PrintFileInfo(char *inFileName);

    // Lê um arquivo .bin
    bool ReadBinFile(char* inFileName);

    // Lê os registros não removidos de um arquivo .bin
    bool ReadOnBinFile(char* inFileName, long long** bytesOffSet);

    // Faz uma busca num arquivo .bin
    void SearchBinFile(int searchNamber);

    // Faz a remoção de registros do arquivo .bin
    bool RemotionBinFile(char *inFileName, char* indexFileName, int n, HEADER* headerOp);

    // Escreve id/byteoffset num arquivo de índices
    void WriteBinIndex(char* indexFileName, long long* bytesOffSet);

    // Insere os registros num arquivo .bin
    bool Insert(FILE* inFile, int insertions, FILE* treeFile, bool bTree);

    // Busca por um Id na árvore b
    void SearchWithIdAbTree(FILE* treeFile, FILE* binFile, int numberOfSearch, int IDtoBeFound, bool option);

    // Função auxiliar para buscas da operação 9
    bool search09(FILE* treeFile, FILE* inFile, int buscaAtual);

    // Controle da busca da operação 9
    void searchControl09(char* inFileName, char* BTreeFileName, int searches);
    
#endif