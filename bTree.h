#ifndef BTREE_H
    #define BTREE_H

    #include "list.h"
    #define ORDER 4 // Ordem da Árvore b

    typedef struct headerTree T_HEADER;
    typedef struct promoveSplit PROMOVE;

    /*
        Arquivo:
            Possui as estruturas de cabeçalho, de nó e de split e as
            funções e procedimentos específicos para manipulação
            do índice Árvore b
    */

    // Aloca memória para o cabeçalho e o inicializa
    T_HEADER* CreateBHeader(char status, int rootNode, int nextRRN, int nodeCounter);

    // Cria uma Árvore b a partir de um arquivo binário
    bool CreateBTree(FILE* inFile, FILE* treeFile);

    // Insere um nó com id/byteOffset
    void InsertNode(FILE* treeFile, T_HEADER** header, PROMOVE** promove, int id, long long byteOffset);

    // Insere o primeiro item num novo nó
    void InsertFirstNode(FILE* treeFile, T_HEADER** header, int RRN, int nodeHeight, int id, long long byteOffset, int leftRRN, int rightRRN, int option);

    // Insere um nó de id/byteOffset numa Árvore b não vazia
    void InsertNodeAux(FILE* treeFile, T_HEADER** header, PROMOVE** promove, int RRN, int id, long long byteOffset);

    // Insere um item num nó não cheio
    void InsertNonFullNode(FILE* treeFile, T_HEADER** header, int RRN, int height, int numberOfNodes, int id, long long byteOffset, int leftRRN, int rightRRN);

    // Procura pelo próximo RRN/nó a partir de uma dada chave de busca id
    int SearchNextRRN(FILE* treeFile, int numberOfNodes, int id, long long byteOffset);

    // Realiza o split num determinado nó
    void Split(FILE* treeFile, T_HEADER** header, PROMOVE** promove, int height, int numberOfNodes, int RRN, int id, long long byteOffset, int rightRRN);

    // Desloca os itens (id/byteOffset) em algumas posições à direita do nó
    void ShiftItens(FILE* treeFile, int shiftRequired, int id, long long byteOffSet);

    // Desloca os RRNs do nó em algumas posições para a direita
    void ShiftRRN(FILE* treeFile, int shiftRequired, int rrnAux);

    // Retorna o Byte Offset do item com chave específica por IDtoBeFound
    long long SearchRegister(FILE* treeFile, int RRN, int IDtoBeFound);
    
    // Insere um novo item na árvore
    void insertReg(T_HEADER** header, PROMOVE** promove, FILE* inFile, FILE* treeFile, int id, long int byteOffSet);

    // Prepara as structs header e promove para uma inserção na árvore
    bool insertPreparations(T_HEADER** header, PROMOVE** promove, FILE* treeFile);

    // Escreve os dados finais do cabeçalho
    void insertFinalizations(T_HEADER** header, PROMOVE** promove, FILE* treeFile);

    // Libera a memória alocada para o header
    void FreeTreeHeader(T_HEADER** header);

#endif