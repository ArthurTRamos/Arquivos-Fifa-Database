#include "fileFunctions.h"

// Lê as informações dos jogadores do .csv
// Armazena cada linha do arquivo num array de strings
bool ReadCsvFile(char* inFileName) {
    // Ponteiro para o arquivo .csv
    FILE* inFile;
    // Variáveis para o número total de registros/strings,
    // tamanho da string de cada linha do registro e número de strings
    int registerNumber, stringSize, matrizSpace;
    
    // Array de strings para cada linha do .csv (matriz)
    char** registers;
    // Vetor auxiliar
    char trash[50];

    registerNumber = 0;
    matrizSpace = 1000;

    // Abre arquivo .csv para leitura
    if((inFile = fopen(inFileName, "r")) == NULL)
        return false;

    // Aloca espaço para 1000 strings num array
    if(((registers = (char**) malloc(matrizSpace * sizeof(char*))) == NULL))
        return false;
    
    // Aloca 85 bytes para cada string
    for(int i = 0; i < matrizSpace; i++) {
        if((registers[i] = (char*) malloc(85 * sizeof(char))) == NULL) {
            free(registers);
            registers = NULL;
            return false;
        }
    }
    
    // Lê/pula a primeira linha do arquivo .csv
    fgets(trash, 50, inFile);

    // Lê cada linha do arquivo .csv e armazena na posição
    // registerNumber do array de strings
    while(fgets(registers[registerNumber], 85, inFile) != NULL) {
        // Se atingir o número máximo de strings, realoca memória
        if(registerNumber == matrizSpace - 1) {
            matrizSpace *= 2;
            registers = realloc(registers, matrizSpace * sizeof(char*));

            for(int i = matrizSpace/2; i < matrizSpace; i++) {
                if((registers[i] = (char*) malloc(85 * sizeof(char))) == NULL) {
                    free(registers);
                    registers = NULL;
                    return false;
                }
            }
        }

        // Armazena em stringSize o tamanho da linha lida
        stringSize = strlen(registers[registerNumber]);

        // Insere '\0' no lugar do '\n' para ajustar a string lida
        registers[registerNumber][stringSize - 1] = '\0';

        // Soma 1 ao número de registros
        registerNumber++;
    }

    // Armazena as informações em registros
    if(!StoreCsvInfo(registers, registerNumber))
        return false;

    fclose(inFile);
    
    // Libera toda a memória alocada
    for(int i = 0; i < registerNumber + 1; i++) {
        free(registers[i]);
        registers[i] = NULL;
    }

    free(registers);
    registers = NULL;

    return true;
}

// Escreve os registros e campos no arquivo.bin
bool WriteBinFile(char *outFileName) {
    // Ponteiro para o arquivo .bin
    FILE* outFile;

    // Abre o arquivo binário para escrita
    if((outFile = fopen(outFileName, "wb")) == NULL)
        return false;

    // Escreve o registro de cabeçalho
    WriteBinHeader(outFile);
    // Escreve os registros de dados
    WriteBinData(outFile);

    fclose(outFile);
    
    return true;
}

// Lê arquivo .bin e imprime as informações pedidas
void PrintFileInfo(char *inFileName) {
    FILE* inFile; // Ponteiro para o arquivo binário
    char status; // Status do arquivo
    char trash[16]; // leitura/pulo de top e proxByteOffset
    int on, off; // Registros não removidos e removidos

    // Abre o arquivo binário para leitura
    if((inFile = fopen(inFileName, "rb")) == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Lê o primeiro byte do arquivo
    fread(&status, 1, 1, inFile);

    // Se o arquivo estiver inconsistente, imprime
    // o erro e sai da função
    if(status == '0') {
        fclose(inFile);
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Lê o resto do cabeçalho
    fread(trash, 16, 1, inFile);
    fread(&on, 4, 1, inFile);
    fread(&off, 4, 1, inFile);

    // Se houver registros não removidos, chama GetData
    if(on > 0)
        GetData(inFile, on + off);
    else
        printf("Registro inexistente.\n\n");

    fclose(inFile);
}

// Lê o arquivo .bin e armazena em registros
bool ReadBinFile(char* inFileName) {
    FILE* inFile; // Ponteiro para o arquivo .bin
    char status; // Status do arquivo
    char lixo[16]; // Pula top e proxByteOffset
    int onReg, offReg; // Registros não removidos e removidos

    // Abre o arquvio binário para leitura
    if((inFile = fopen(inFileName, "rb")) == NULL)
        return false;

    // Lê o status do arquivo
    fread(&status, 1, 1, inFile);

    // Se o arquivo estiver inconsistente, retorna falso
    if(status == '0') {
        fclose(inFile);
        return false;
    }

    // Lê o resto do cabeçalho
    fread(lixo, 16, 1, inFile);
    fread(&onReg, 4, 1, inFile);
    fread(&offReg, 4, 1, inFile);

    // Armazena os registros de dados
    return(StoreBinInfo(inFile, onReg, offReg));
}

// Lê e armazena apenas os registros não removidos do arquivo binário
bool ReadOnBinFile(char* inFileName, long long** bytesOffSet) {
    FILE* inFile; // Ponteiro para o arquivo .bin
    char status; // Status do arquivo
    char trash[16]; // Pula top e proxByteOffset
    int onReg, offReg; // Registros não removidos e removidos

    // Abre o arquvio binário para leitura
    if((inFile = fopen(inFileName, "rb")) == NULL)
        return false;

    // Lê o status do arquivo
    fread(&status, 1, 1, inFile);

    // Se o arquivo estiver inconsistente, retorna falso
    if(status == '0') {
        fclose(inFile);
        return false;
    }

    // Lê o resto do cabeçalho
    fread(trash, 16, 1, inFile);
    fread(&onReg, 4, 1, inFile);
    fread(&offReg, 4, 1, inFile);

    *bytesOffSet = (long long*) malloc(onReg * sizeof(long long));

    // Armazena os registros de dados
    return(StoreOnBinInfo(inFile, onReg, offReg, bytesOffSet));
}

// Faz a busca de número i nos registros de dados
void SearchBinFile(int i) {
    // Quantidade de duplas campo e valor do campo da busca
    int duplas;

    scanf("%d", &duplas);

    // Vetor de strings para os nomes dos campos a serem buscados
    char** campName = (char**) malloc(duplas * sizeof(char*));
    for(int i = 0; i < duplas; i++) {
        campName[i] = (char*) malloc(14 * sizeof(char));
    }

    // Vetor de strings para os valores dos campos a serem buscados
    char** campValue = (char**) malloc(duplas * sizeof(char*));
    for(int i = 0; i < duplas; i++) {
        campValue[i] = (char*) malloc(40 * sizeof(char));
    }
    
    // Lê os campos e seus respectivos valores
    for(int i = 0; i < duplas; i++) {
        scanf("%s", campName[i]);
        if(strcmp(campName[i], "id") == 0 || strcmp(campName[i], "idade") == 0)
            scanf("%s", campValue[i]);
        // Se for uma string, lê com a função scan_quote_string
        else
            scan_quote_string(campValue[i]);
    }

    // Chamada à função de busca
    SearchBinData(duplas, campName, campValue, i);

    // Desaloca a memória alocada para os campos
    for(int i = 0; i < duplas; i++) {
        free(campName[i]);
        free(campValue[i]);

        campName[i] = NULL;
        campValue[i] = NULL;
    }

    // Desaloca memória
    free(campName);
    free(campValue);
    
    campName = NULL;
    campValue = NULL;
}

// Faz uma remoção de número n dos arquivos binários
bool RemotionBinFile(char *inFileName, char* indexFileName, int n, HEADER* headerOp) {
    int duplas; // Quantidade de campos para a busca n
    scanf("%d", &duplas);

    // Vetor de strings para os nomes dos campos a serem buscados
    char** campName = (char**) malloc(duplas * sizeof(char*));
    for(int i = 0; i < duplas; i++) {
        campName[i] = (char*) malloc(14 * sizeof(char));
    }

    // Vetor de strings para os valores dos campos a serem buscados
    char** campValue = (char**) malloc(duplas * sizeof(char*));
    for(int i = 0; i < duplas; i++) {
        campValue[i] = (char*) malloc(40 * sizeof(char));
    }
    
    // Lê os campos e seus respectivos valores
    for(int i = 0; i < duplas; i++) {
        scanf("%s", campName[i]);
        if(strcmp(campName[i], "id") == 0 || strcmp(campName[i], "idade") == 0)
            scanf("%s", campValue[i]);
        // Se for uma string, lê com a função scan_quote_string
        else
            scan_quote_string(campValue[i]);
    }

    // REMOÇÃO DOS REGISTROS
    if(RemoveBinData(inFileName, indexFileName, duplas, campName, campValue, n, headerOp))
        return true;
    else
        return false;

    for(int i = 0; i < duplas; i++) {
        free(campName[i]);
        free(campValue[i]);
        campName[i] = NULL;
        campValue[i] = NULL;
    }

    free(campName);
    free(campValue);
    campName = NULL;
    campValue = NULL;
}

// Escreve a dupla id-byteoffset no arquivo de índice indexFileName
void WriteBinIndex(char* indexFileName, long long* bytesOffSet) {
    FILE* indexFile;
    
    if((indexFile = fopen(indexFileName, "wb")) == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    WriteRegistersIndex(indexFile, bytesOffSet);
    fclose(indexFile);
}

// Insere os registros no arquivo binário inFileName
bool Insert(FILE* inFile, int insertions, FILE* treeFile, bool bTree) {
    if(insertions > 0) {
        int nextByteOffset = CreateList(inFile); // Byte final do arquivo e Criação da lista de removidos
        if(nextByteOffset == -1) {
            fclose(inFile);
            return false;
        }

        Inserir(inFile, insertions, nextByteOffset, treeFile, bTree); // Insere os novos registros
        ClearList(inFile); // Desaloca a lista
    }

    char status = '1';
    fseek(treeFile, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, treeFile);

    fclose(treeFile);
    fclose(inFile);
    return true;
}

// Busca por um Id (IDtoBeFound) na árvore b (treeFile)
void SearchWithIdAbTree(FILE* treeFile, FILE* binFile, int numberOfSearch, int IDtoBeFound, bool option) {
    int noRaiz;

    fseek(treeFile, 1, SEEK_SET);
    fread(&noRaiz, sizeof(int), 1, treeFile);

    char status;
    int tamNome, tamNacionalidade, tamClube, tamRegistro, lixoInt;
    char nome[100], nacionalidade[100], clube[100], lixoStr[100];
    long long lixoLong;

    // Obtém o byteoffset do id procurado no arquivo binário
    long long byteOffSetToBeFound = SearchRegister(treeFile, noRaiz, IDtoBeFound);

    // Se não achou o item
    if(byteOffSetToBeFound == -1) {
        if(option)
            printf("BUSCA %d\n\n", numberOfSearch + 1);
        else
            printf("Busca %d\n\n", numberOfSearch + 1);
        printf("Registro inexistente.\n\n");
        return;
    }

    // Pula para o Byte Offset do registro
    fseek(binFile, byteOffSetToBeFound, SEEK_SET);
    fread(&status, sizeof(char), 1, binFile);
    fread(&tamRegistro, sizeof(int), 1, binFile);

    // Se o registro estiver removido
    if(status == '1') {
        printf("Registro inexistente.\n\n");
        fclose(binFile);
        fclose(treeFile);
        return;
    }

    fread(&(lixoLong), sizeof(long long), 1, binFile);

    // Atualiza o ID e a idade do jogador
    fread(&(lixoInt), sizeof(int), 1, binFile);
    fread(&(lixoInt), sizeof(int), 1, binFile);

    // Atualiza os campos variáveis e os tamanhos dos campos

    fread(&tamNome, sizeof(int), 1, binFile);
    fread(nome, tamNome, 1, binFile);
    nome[tamNome] = '\0';

    fread(&tamNacionalidade, sizeof(int), 1, binFile);
    fread(nacionalidade, tamNacionalidade, 1, binFile);
    (nacionalidade)[tamNacionalidade] = '\0';

    fread(&tamClube, sizeof(int), 1, binFile);
    fread(clube, tamClube, 1, binFile);
    clube[tamClube] = '\0';

    if(option)
        printf("BUSCA %d\n\n", numberOfSearch + 1);
    else
        printf("Busca %d\n\n", numberOfSearch + 1);

    if(tamNome == 0)
        printf("Nome do Jogador: SEM DADO\n");
    else
        printf("Nome do Jogador: %s\n", nome);
                
    if(tamNacionalidade == 0)
        printf("Nacionalidade do Jogador: SEM DADO\n");
    else
        printf("Nacionalidade do Jogador: %s\n", nacionalidade);

    if(tamClube == 0)
        printf("Clube do Jogador: SEM DADO\n\n");
    else
        printf("Clube do Jogador: %s\n\n", clube);

}

// Controle principal da busca da operação 9
void searchControl09(char* inFileName, char* BTreeFileName, int searches) {
    FILE* inFile, *treeFile;
    char lixo, status;

    // Abre o arquivo binário para leitura
    if((inFile = fopen(inFileName, "rb+")) == NULL) {
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
    if((treeFile = fopen(BTreeFileName, "rb+")) == NULL) {
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

    // Faz as buscas necessárias
    for(int i = 0; i < searches; i++) {
        if(!search09(treeFile, inFile, i)) {
            printf("Falha no processamento do arquivo.\n");
            fclose(inFile);
            fclose(treeFile);
            return;
        }
        scanf("%c", &lixo);
    }

    fclose(inFile);
    fclose(treeFile);
}
    
// Função auxiliar para buscas da operação 9
bool search09(FILE* treeFile, FILE* inFile, int buscaAtual) {
    int nSlots, idIndex, id;
    bool idTrigger = false;
    char** slots;
    scanf("%d", &nSlots); // Número de campos a serem buscados na busca atual

    // Aloca espaço para os campos de busca e seus valores
    slots = (char**) malloc(sizeof(char*) * nSlots * 2);

    if(slots == NULL) {
        return false;
    }

    for(int i = 0; i < (nSlots * 2); ++i) {
        slots[i] = (char*) malloc(40 * sizeof(char));

        if(slots[i] == NULL) {
            return false;
        }
    }

    for(int i = 0; i < (nSlots * 2); i = i + 2) {
        scanf("%s", slots[i]); // Lê o campo de busca
        scan_quote_string(slots[i + 1]); // Lê o valor do campo de busca

        // Verifica se o campo é o id
        if(strcmp(slots[i], "id") == 0) {
            idIndex = i+1;
            idTrigger = true;
        }

    }

    // Se há a busca por id
    if(idTrigger) {
        id = atoi(slots[idIndex]);
        SearchWithIdAbTree(treeFile, inFile, buscaAtual, id, false);
    // Se não há busca por id
    } else {
        searchWithoutId(inFile, buscaAtual, slots, nSlots);
    }

    for(int i = 0; i < nSlots*2; ++i) {
        free(slots[i]);
    }
    free(slots);

    return true;
}