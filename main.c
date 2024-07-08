/*
    Grupo:
        Arthur Trottmann Ramos - 14681052
        Henrique Drago - 14675441
        Henrique Yukio Sekido - 14614564

    Arquivo:
        Programa principal e de leitura de Entradas. Redireciona
        para as operações, funções e procedimentos específicos
*/

#include "operations.h"

int main(void) {
    // Variáveis para o número da funcionalidade (de 1 a 10),
    // o número de buscas (funcionalidade 3, 8 e 9), de remoções (funcionalidade 5) e
    // de inserções (funcionalidades 6 e 10 )
    int operation, numberOfSearches, numberOfRemotions, numberOfInsertion;
    // Variáveis para as strings dos nomes dos arquivos de
    // entrada e de saída
    char *inFileName, *outFileName, *indexFileName;

    inFileName = (char*) malloc(20 * sizeof(char));
    outFileName = (char*) malloc(20 * sizeof(char));
    indexFileName = (char*) malloc(20 * sizeof(char));

    scanf("%d %s", &operation, inFileName);

    // Escolha a funcionalidade do programa
    switch(operation) {
        case 1:
            // Funcionalidade 1
            scanf("%s", outFileName);
            Operation1(inFileName, outFileName);
            break;
        case 2:
            // Funcionalidade 2
            Operation2(inFileName);
            break;
        case 3:
            // Funcionalidade 3
            scanf("%d", &numberOfSearches);
            Operation3(inFileName, numberOfSearches);
            break;
        case 4:
            // Funcionalidade 4
            scanf("%s", indexFileName);
            Operation4(inFileName, indexFileName);
            break;
        case 5:
            // Funcionalidade 5
            scanf("%s", indexFileName);
            scanf("%d", &numberOfRemotions);
            Operation5(inFileName, indexFileName, numberOfRemotions);
            break;
        case 6:
            // Funcionalidade 6
            scanf("%s %d", indexFileName, &numberOfInsertion);
            Operation6(inFileName, indexFileName, numberOfInsertion);
            break;
        case 7:
            // Funcionalidade 7
            scanf("%s", indexFileName);
            Operation7(inFileName, indexFileName);
            break;
        case 8:
            // Funcionalidade 8
            scanf("%s %d", indexFileName, &numberOfSearches);
            Operation8(inFileName, indexFileName, numberOfSearches);
            break;
        case 9:
            // Funcionalidade 9
            scanf("%s %d", indexFileName, &numberOfSearches);
            Operation9(inFileName, indexFileName, numberOfSearches);
            break;
        case 10:
            // Funcionalidade 10
            scanf("%s %d", indexFileName, &numberOfInsertion);
            Operation10(inFileName, indexFileName, numberOfInsertion);
            break;
        default:
            printf("Falha no processamento do arquivo.\n");
    }
    
    // Libera a memória alocada
    free(inFileName);
    free(outFileName);
    free(indexFileName);
    inFileName = NULL;
    outFileName = NULL;
    indexFileName = NULL;
    
    return 0;
}