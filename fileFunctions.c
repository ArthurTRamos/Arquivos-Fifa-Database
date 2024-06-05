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

    // Remove os registros
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
void Insert(char* inFileName) {
    int NInsercoes; // Número de inserções
    FILE* inFile;

    scanf("%d", &NInsercoes);

    inFile = fopen(inFileName, "rb+");

    if(NInsercoes > 0) {
        int teste = CreateList(inFile); // Byte final do arquivo e Criação da lista de removidos
        Inserir(inFile, NInsercoes, teste); // Insere um novo registro
        ClearList(inFile); // Desaloca a lista
    }
    fclose(inFile);
}