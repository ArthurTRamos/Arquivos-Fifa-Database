#include "registers.h"

// Registro de Cabeçalho
struct headerRegister {
    char status; // Consistência do Arquivo ('0'/'1' para instável/estável)
    long long top; // Byte offset de um registro removido
    long long nextByteOffset; // Próximo byte disponível para escrita
    int onRegistersNumber; // Registros não removidos
    int offRegistersNumber; // Registros removidos
};

// Registro de Dados
struct dataRegister {
    char removed; // Registro removido ou não
    int registerSize; // Número de Bytes do registro
    long long nextRegister; // Byte offsete do próximo registro removido
    int playerId; // ID do jogador
    int playerAge; // Idade do jogador
    int playerNameSize;
    char* playerName; // Nome do jogador
    int playerNatSize;
    char* playerNat; // Nacionalidade do jogador
    int playerClubSize;
    char* playerClub; // Clube do jogador
};

HEADER* header;
DATA** data;

// Aloca memória para o registro de cabeçalho
HEADER* CreateHeader() {
    HEADER* newHeader;

    if((newHeader = (HEADER*) malloc(sizeof(HEADER))) == NULL)
        return NULL;

    // Inicializa os campos
    (newHeader->status) = '0';
    newHeader->top = -1;
    newHeader->nextByteOffset = 0;
    newHeader->onRegistersNumber = 0;
    newHeader->offRegistersNumber = 0;

    return newHeader;
}

// Aloca memória para os registros de dados
DATA** CreateData(int registerNumber) {
    DATA** newData;

    if((newData = (DATA**) malloc(registerNumber * sizeof(DATA*))) == NULL)
        return NULL;

    for(int i = 0; i < registerNumber; i++) {
        if((newData[i] = (DATA*) malloc(sizeof(DATA))) == NULL)
            return NULL;

        // Reserva espaço para nome, nacionalidade e
        // clube do jogador do registro
        newData[i]->playerName = (char*) malloc(40 * sizeof(char));
        newData[i]->playerNat = (char*) malloc(40 * sizeof(char));
        newData[i]->playerClub = (char*) malloc(40 * sizeof(char));
    }

    return newData;
}


// Armazena os registros lidos de registers nas structs
// de cabeçalho e de dados
bool StoreCsvInfo(char** registers, int registerNumber) {
    header = CreateHeader();
    data = CreateData(registerNumber);

    if((header == NULL) || (data == NULL))
        return false;

    char field[40]; // Armazenamento temporário de cada campo
    char control; // Contole de término do campo (',')

    // Tamanho dos campos variáveis e 
    // contole do byte offset do próximo registro
    int nameSize, natSize, clubSize, begin;

    // índices das strings de field e da linha lida
    // do arquivo .csv
    int j, stringPerc;

    begin = 25; // Byte Offset do primeiro registro

    // Loop para cada registro de dados
    for(int i = 0; i < registerNumber; i++) {
        j = stringPerc = 0;
        (data[i]->removed) = '0';

        // Armazena o 1º caractere da string
        control = registers[i][stringPerc];

        // Leitura do campo ID
        while(control != ',') {
            field[j] = control;
            j++;
            stringPerc++;
            control = registers[i][stringPerc];
        }

        field[j] = '\0';
        stringPerc++;

        // Converte ID para inteiro e armazena em seu campo
        data[i]->playerId = atoi(field);


        j = 0;
        control = registers[i][stringPerc];

        // Leitura do campo Idade
        while(control != ',') {
            field[j] = control;
            j++;
            stringPerc++;
            control = registers[i][stringPerc];
        }
        
        field[j] = '\0';
        stringPerc++;

        // Verifica se o campo é nulo ou não
        if(j != 0)
            data[i]->playerAge = atoi(field);
        else
            data[i]->playerAge = -1;

        
        j = 0;
        control = registers[i][stringPerc];

        // Leitura do campo nome do jogador
        while(control != ',') {
            field[j] = control;
            j++;
            stringPerc++;
            control = registers[i][stringPerc];
        }

        field[j] = '\0';
        stringPerc++;

        // Verifica se o campo é nulo ou não
        // Se não, atualiza o tamanho do nome e
        // o próprio nome
        if(j != 0) {
            nameSize = strlen(field);
            strcpy(data[i]->playerName, field);
        }
        else {
            nameSize = 0;
            data[i]->playerName = "";
        }

        
        j = 0;
        control = registers[i][stringPerc];

        // Leitura do campo nacionalidade do jogador
        while(control != ',') {
            field[j] = control;
            j++;
            stringPerc++;
            control = registers[i][stringPerc];
        }

        field[j] = '\0';
        stringPerc++;

        // Verifica se o campo é nulo ou não
        // Se não, atualiza o tamanho da nacionalidade e
        // a própria nacionalidade
        if(j != 0) {
            natSize = strlen(field);
            strcpy(data[i]->playerNat, field);
        }
        else {
            natSize = 0;
            data[i]->playerNat = "";
        }

        
        j = 0;
        control = registers[i][stringPerc];

        // Leitura do campo clube do jogador
        while(control != '\0' && control != 13) {
            field[j] = control;
            j++;
            stringPerc++;
            control = registers[i][stringPerc];
        }

        field[j] = '\0';

        // Verifica se o campo é nulo ou não
        // Se não, atualiza o tamanho do clube e
        // o próprio clube
        if(j != 0) {
            clubSize = strlen(field);
            strcpy(data[i]->playerClub, field);
        }
        else {
            clubSize = 0;
            data[i]->playerClub = "";
        }

        // Atualiza os tamanhos dos campos variáveis
        data[i]->playerNameSize = nameSize;
        data[i]->playerNatSize = natSize;
        data[i]->playerClubSize = clubSize;

        // Atualiza o tamanho do registro e o próximo registro removido
        data[i]->registerSize = 33 + nameSize + natSize + clubSize;
        data[i]->nextRegister = -1;

        // Atualiza o byte offset atual
        begin = begin + data[i]->registerSize;

        // Atualiza o cabeçalho com o próximo byte livre e
        // número de registros
        header->nextByteOffset = begin;
        if(data[i]->removed == '0')
            header->onRegistersNumber += 1;
        else
            header->offRegistersNumber += 1;
    }

    // Atualiza o status e o último byte do arquivo
    header->status = '1';
    header->nextByteOffset = begin;
    
    return true;
}

// Escreve o registro de cabeçalho no arquivo .bin
void WriteBinHeader(FILE* outFile) {
    fwrite(&(header->status), sizeof(char), 1, outFile);
    fwrite(&(header->top), sizeof(long long), 1, outFile);
    fwrite(&(header->nextByteOffset), sizeof(long long), 1, outFile);
    fwrite(&(header->onRegistersNumber), sizeof(int), 1, outFile);
    fwrite(&(header->offRegistersNumber), sizeof(int), 1, outFile);
}

// Escreve o registro de dados no arquivo .bin
void WriteBinData(FILE* outFile) {
    for(int i = 0; i < header->onRegistersNumber; i++) {
        fwrite(&(data[i]->removed), sizeof(char), 1, outFile);
        fwrite(&(data[i]->registerSize), sizeof(int), 1, outFile);
        fwrite(&(data[i]->nextRegister), sizeof(long long), 1, outFile);
        fwrite(&(data[i]->playerId), sizeof(int), 1, outFile);
        fwrite(&(data[i]->playerAge), sizeof(int), 1, outFile);
        fwrite(&(data[i]->playerNameSize), sizeof(int), 1, outFile);
        fwrite(data[i]->playerName, data[i]->playerNameSize * sizeof(char), 1, outFile);
        fwrite(&(data[i]->playerNatSize), sizeof(int), 1, outFile);
        fwrite(data[i]->playerNat, data[i]->playerNatSize * sizeof(char), 1, outFile);
        fwrite(&(data[i]->playerClubSize), sizeof(int), 1, outFile);
        fwrite(data[i]->playerClub, data[i]->playerClubSize * sizeof(char), 1, outFile); 
    }

    // Libera memória de data e de header
    FreeMemoryData(&data, header->onRegistersNumber);
    FreeMemoryHeader(&header);
}

// Lê os registros do arquivo .bin e os imprime
void GetData(FILE* inFile, int n_reg) {
    // Usa um registro de dados auxiliar
    DATA* data = (DATA*) malloc(sizeof(DATA));
    // vetor auxiliar para pular um registro removido
    char lixo[200];

    data->playerName = (char*) malloc(40 * sizeof(char));
    data->playerNat = (char*) malloc(40 * sizeof(char));
    data->playerClub = (char*) malloc(40 * sizeof(char));

    // Percorre todos os registros do arquivo
    for(int i = 0; i < n_reg; i++) {
        fread(&(data->removed), 1, 1, inFile);
        fread(&(data->registerSize), 4, 1, inFile);

        // Se for um registro removido, pula para o próximo
        if(data->removed == '1') {
            fread(lixo, data->registerSize - 5, 1, inFile);
            continue;
        }

        // Lê e armazena todas as outras informações do registro de
        // dados
        fread(&(data->nextRegister), sizeof(long long), 1, inFile);
        fread(&(data->playerId), sizeof(int), 1, inFile);
        fread(&(data->playerAge), sizeof(int), 1, inFile);
        fread(&(data->playerNameSize), sizeof(int), 1, inFile);
        fread(data->playerName, data->playerNameSize, 1, inFile);
        (data->playerName)[data->playerNameSize] = '\0';
        fread(&(data->playerNatSize), sizeof(int), 1, inFile);
        fread(data->playerNat, data->playerNatSize, 1, inFile);
        (data->playerNat)[data->playerNatSize] = '\0';
        fread(&(data->playerClubSize), sizeof(int), 1, inFile);
        fread(data->playerClub, data->playerClubSize, 1, inFile);
        (data->playerClub)[data->playerClubSize] = '\0';

        // Imprime as informações pedidas do registro
        PrintData(data);
    }
}

// Imprime nome, nacionalidade e clube do jogador de um
// registro de dados
void PrintData(DATA* data) {
    // Nome
    printf("Nome do Jogador: ");
    if(data->playerNameSize > 0)
        printf("%s\n", data->playerName);
    else
        printf("SEM DADO\n");

    // Nacionalidade
    printf("Nacionalidade do Jogador: ");
    if(data->playerNatSize > 0)
        printf("%s\n", data->playerNat);
    else
        printf("SEM DADO\n");

    // Clube
    printf("Clube do Jogador: ");
    if(data->playerClubSize > 0)
        printf("%s\n\n", data->playerClub);
    else
        printf("SEM DADO\n\n");
}

// Armazena as informações do arquivo .bin em registros
bool StoreBinInfo(FILE* inFile, int onReg, int offReg) {
    // Aloca memória para o cabeçalho e para os dados
    header = CreateHeader();
    data = CreateData(onReg + offReg);

    char status; // Verifica se chegou ao final do arquivo
    int i = 0; // Contola o índice do registro atual

    if(header == NULL || data == NULL)
        return false;

    header->onRegistersNumber = onReg;
    header->offRegistersNumber = offReg;

    // Lê cada registro do arquivo binário até o fim
    while(fread(&status, sizeof(char), 1, inFile) != 0) {
        // Atualiza o campo "removido"
        data[i]->removed = status;
        
        // Atualiza o tamanho do registro e o próximo registro removido
        fread(&(data[i]->registerSize), sizeof(int), 1, inFile);
        fread(&(data[i]->nextRegister), sizeof(long long), 1, inFile);

        // Atualiza o ID e a idade do jogador
        fread(&(data[i]->playerId), sizeof(int), 1, inFile);
        fread(&(data[i]->playerAge), sizeof(int), 1, inFile);

        // Atualiza os campos variáveis e os tamanhos dos campos

        fread(&(data[i]->playerNameSize), sizeof(int), 1, inFile);
        fread(data[i]->playerName, data[i]->playerNameSize, 1, inFile);
        (data[i]->playerName)[data[i]->playerNameSize] = '\0';

        fread(&(data[i]->playerNatSize), sizeof(int), 1, inFile);
        fread(data[i]->playerNat, data[i]->playerNatSize, 1, inFile);
        (data[i]->playerNat)[data[i]->playerNatSize] = '\0';

        fread(&(data[i]->playerClubSize), sizeof(int), 1, inFile);
        fread(data[i]->playerClub, data[i]->playerClubSize, 1, inFile);
        (data[i]->playerClub)[data[i]->playerClubSize] = '\0';

        i++;
    }

    // Fecha o arquivo binário
    fclose(inFile);
    return true;
}

// Armazena somente os registros não removidos do arquivo binário
bool StoreOnBinInfo(FILE* inFile, int onReg, int offReg, long long** bytesOffSet) {
    // Aloca memória para o cabeçalho e para os dados
    header = CreateHeader();
    data = CreateData(onReg);

    char status; // Verifica se chegou ao final do arquivo
    char lixo[200];
    int i = 0; // Contola o índice do registro atual
    int begin = 25; // Byteoffset do primeiro registro
    int remaining; // Controla o número de cifrões no final de um registro

    if(header == NULL || data == NULL)
        return false;

    header->onRegistersNumber = onReg;
    header->offRegistersNumber = offReg;

    // Lê cada registro do arquivo binário até o fim ou até encontrar todos os registros
    // não removidos

    while(fread(&status, sizeof(char), 1, inFile) != 0 && i != onReg) {
        data[i]->removed = status;
        fread(&(data[i]->registerSize), sizeof(int), 1, inFile);

        if(status == '1') {
            fread(lixo, data[i]->registerSize - 5, 1, inFile);
            begin += data[i]->registerSize;
            continue;
        }

        // Atualiza o Byteoffset do registro
        (*bytesOffSet)[i] = begin;

        fread(&(data[i]->nextRegister), sizeof(long long), 1, inFile);

        // Atualiza o ID e a idade do jogador
        fread(&(data[i]->playerId), sizeof(int), 1, inFile);
        fread(&(data[i]->playerAge), sizeof(int), 1, inFile);

        // Atualiza os campos variáveis e os tamanhos dos campos

        fread(&(data[i]->playerNameSize), sizeof(int), 1, inFile);
        fread(data[i]->playerName, data[i]->playerNameSize, 1, inFile);
        (data[i]->playerName)[data[i]->playerNameSize] = '\0';

        fread(&(data[i]->playerNatSize), sizeof(int), 1, inFile);
        fread(data[i]->playerNat, data[i]->playerNatSize, 1, inFile);
        (data[i]->playerNat)[data[i]->playerNatSize] = '\0';

        fread(&(data[i]->playerClubSize), sizeof(int), 1, inFile);
        fread(data[i]->playerClub, data[i]->playerClubSize, 1, inFile);
        (data[i]->playerClub)[data[i]->playerClubSize] = '\0';

        remaining = data[i]->registerSize - 33 - data[i]->playerNameSize - data[i]->playerNatSize - data[i]->playerClubSize;
        fread(&lixo, sizeof(char), remaining, inFile);

        begin += data[i]->registerSize;

        i++;
    }

    // Fecha o arquivo binário
    fclose(inFile);
    return true;
}

// Realiza uma busca de campos e seus valores nos registros de dados 
void SearchBinData(int duplas, char** campName, char** campValue, int i) {
    // Variáveis:
    // Contole do número de campos que correspondem à busca
    // Número de registros que correspondem à busca
    // Verifica se ID foi encontrado, caso seja um campo de busca
    int campCounter, regFinded, verifyID;

    int registers = header->onRegistersNumber;
    int totalRegisters = registers + header->offRegistersNumber;

    regFinded = 0;

    // Itera sobre todos os registros
    for(int i = 0; i < totalRegisters; i++) {
        // Verifica se o registro está removido
        if(data[i]->removed == '1')
            continue;

        campCounter = verifyID = 0;
        
        // Itera o registro sobre todos campos pedidos na busca
        for(int j = 0; j < duplas; j++) {
            // Campo de busca ID
            if(strcmp(campName[j], "id") == 0) {
                // Verifica se os dados batem
                if(data[i]->playerId == (atoi(campValue[j]))) {
                    campCounter++;
                    verifyID = 1;
                }
                continue;
            }

            // Campo de busca Idade
            if(strcmp(campName[j], "idade") == 0) {
                // Verifica se os dados batem
                if(data[i]->playerAge == (atoi(campValue[j])))
                    campCounter++;
                continue;
            }

            // Campo de busca nomeJogador
            if(strcmp(campName[j], "nomeJogador") == 0) {
                if(data[i]->playerNameSize == 0)
                    continue;

                // Verifica se os dados batem
                if(strcmp(data[i]->playerName, campValue[j]) == 0)
                    campCounter++;
                continue;
            }

            // Campo de busca nacionalidade
            if(strcmp(campName[j], "nacionalidade") == 0) {
                if(data[i]->playerNatSize == 0)
                    continue;

                // Verifica se os dados batem
                if(strcmp(data[i]->playerNat, campValue[j]) == 0)
                    campCounter++;
                continue;
            }

            // Campo de busca nomeClube
            if(strcmp(campName[j], "nomeClube") == 0) {
                if(data[i]->playerClubSize == 0)
                    continue;

                // Verifica se os dados batem
                if(strcmp(data[i]->playerClub, campValue[j]) == 0)
                    campCounter++;
                continue;
            }
        }

        // Se todos os campos de busca forem encontrados, imprime o registro
        if(campCounter == duplas) {
            if(data[i]->playerNameSize == 0)
                printf("Nome do Jogador: SEM DADO\n");
            else
                printf("Nome do Jogador: %s\n", data[i]->playerName);
                
            if(data[i]->playerNatSize == 0)
                printf("Nacionalidade do Jogador: SEM DADO\n");
            else
                printf("Nacionalidade do Jogador: %s\n", data[i]->playerNat);

            if(data[i]->playerClubSize == 0)
                printf("Clube do Jogador: SEM DADO\n\n");
            else
                printf("Clube do Jogador: %s\n\n", data[i]->playerClub);
                
            regFinded++;
        }

        // Sai do loop se a chave primária (ID) for
        // Encontrada
        if(verifyID == 1)
            break;
    }

    // Nenhum registro bate com as buscas
    if(regFinded == 0)
        printf("Registro inexistente.\n\n");

    // Se a última busca for realizada, libera memória dos registros
    if(i == duplas) {
        FreeMemoryHeader(&header);
        FreeMemoryData(&data, registers);
    }
}

// Insere os registros de entrada no arquivo binário
void Inserir(FILE* inFile, int NInsercoes, int nextByteOffset, FILE* treeFile, bool bTree) {
    T_HEADER* T_header;
    PROMOVE* promove;
    char removed = '0';
    long int nextRegister = -1;
    char c = '$';
    char stats = '1';
    long long lixo;

    DATA* dataVetor = (DATA*) malloc(sizeof(DATA));

    int n, tam, byteOffSet, cifrao, nReg, tFinal, writeTam, nRem, nRemAux;
    long int PBO; // Próximo byteoffset

    nRemAux = 0; // Número de registros inseridos
    tFinal = 0;

    dataVetor->playerNat = (char*) malloc(40 * sizeof(char));
    dataVetor->playerName = (char*) malloc(40 * sizeof(char));
    dataVetor->playerClub = (char*) malloc(40 * sizeof(char));

    if(bTree) {
        insertPreparations(&T_header, &promove, treeFile);
    }
    
    for(int i = 0; i < NInsercoes; ++i) {
        cifrao = 0;

        dataVetor->playerNameSize = 0;
        dataVetor->playerNatSize = 0;
        dataVetor->playerClubSize = 0;

        // Lê as informações do registro
        scan_quote_string(dataVetor->playerClub);
        dataVetor->playerId = atoi(dataVetor->playerClub);

        scan_quote_string(dataVetor->playerClub);
        dataVetor->playerAge = atoi(dataVetor->playerClub);

        if(dataVetor->playerAge == 0)
            dataVetor->playerAge = -1;

        scan_quote_string(dataVetor->playerClub);
        strcpy(dataVetor->playerName, dataVetor->playerClub);
        dataVetor->playerNameSize = strlen(dataVetor->playerClub); 

        scan_quote_string(dataVetor->playerClub);
        strcpy(dataVetor->playerNat, dataVetor->playerClub);
        dataVetor->playerNatSize = strlen(dataVetor->playerClub);

        scan_quote_string(dataVetor->playerClub);
        dataVetor->playerClubSize = strlen(dataVetor->playerClub);     

        tam = dataVetor->playerNameSize + dataVetor->playerNatSize + dataVetor->playerClubSize + 33;

        // Busca pelo registro capaz de corportar o tamanho tam
        byteOffSet = FindNo(tam, inFile);

        // Insere o registro no final do arquivo binário
        if(byteOffSet == -1) {
            fseek(inFile, 0, SEEK_END);
            byteOffSet = ftell(inFile);
            cifrao = 0;
            writeTam = tam;
            tFinal = tFinal + tam;
        // Insere no registro de byteoffset encontrado
        } else {
            fseek(inFile, byteOffSet + 1, SEEK_SET);
            fread(&writeTam, sizeof(int), 1, inFile);
            cifrao = writeTam - tam;
            fseek(inFile, byteOffSet, SEEK_SET);
            nRemAux++;
        }

        // Insere todas as informações do novo registro no arquivo binário
        fwrite(&(removed), sizeof(char), 1, inFile);
        fwrite(&(writeTam), sizeof(int), 1, inFile);
        fwrite(&(nextRegister), sizeof(long int), 1, inFile);
        fwrite(&(dataVetor->playerId), sizeof(int), 1, inFile);
        fwrite(&(dataVetor->playerAge), sizeof(int), 1, inFile);
        fwrite(&(dataVetor->playerNameSize), sizeof(int), 1, inFile);
        fwrite(dataVetor->playerName, dataVetor->playerNameSize * sizeof(char), 1, inFile);
        fwrite(&(dataVetor->playerNatSize), sizeof(int), 1, inFile);
        fwrite(dataVetor->playerNat, dataVetor->playerNatSize * sizeof(char), 1, inFile);
        fwrite(&(dataVetor->playerClubSize), sizeof(int), 1, inFile);
        fwrite(dataVetor->playerClub, dataVetor->playerClubSize * sizeof(char), 1, inFile); 
        
        // Escreve os cifrões necessários no fim do registro
        for(int i = 0; i < cifrao; ++i) {
            fwrite(&c, sizeof(char), 1, inFile);
        }

        if(bTree) {
            insertReg(&T_header, &promove, inFile, treeFile, dataVetor->playerId, byteOffSet);
        }
    }

    if(bTree) {
        insertFinalizations(&T_header, &promove, treeFile);
        FreeTreeHeader(&T_header);
        free(promove);
    }

    // Atualiza os dados do cabeçalho
    fseek(inFile, 17, SEEK_SET);
    PBO = nextByteOffset + tFinal;
    fread(&nReg, 4, 1, inFile);
    nReg = nReg + NInsercoes; // Registros não removidos
    fread(&nRem, 4, 1, inFile);
    nRem = nRem - nRemAux; // Registros removidos
    fseek(inFile, 0, SEEK_SET);
    fwrite(&stats, sizeof(char), 1, inFile);
    fread(&lixo, sizeof(long long), 1, inFile);
    fwrite(&PBO, sizeof(long int), 1, inFile);
    fwrite(&nReg, sizeof(int), 1, inFile);
    fwrite(&nRem, sizeof(int), 1, inFile);

    // Libera Memória
    free(dataVetor->playerName);
    free(dataVetor->playerNat);
    free(dataVetor->playerClub);
    free(dataVetor);
}

// Faz a remoção de registros conforme os campos especificiados
bool RemoveBinData(char* inFileName, char* indexFileName, int duplas, char** campName, char** campValue, int n, HEADER* headerOp) {
    // Variáveis:
    // Contole do número de campos que correspondem à busca
    // Verifica se passou pelo RemoveDataID
    int campCounter, flagID;

    FILE* binFile;
    FILE* indexFile;

    char lixo[200];
    
    char statusCab; // Status do arquivo binário (cabeçalho)
    int onReg, offReg; // Registros não removidos e removidos
    char statusConsist = '0'; //para setar quando for manipular o arquivo

    int begin = 25; // byteoffset inicial de registros
    long long removedByteOffSet;
    long long topoCab; // topo do cabeçalho

    flagID = 0;

    if((binFile = fopen(inFileName, "rb+")) == NULL)
        return false;

    // Itera sobre os campos de busca e verifica se há o id
    for(int j = 0; j < duplas; j++){
        if(strcmp(campName[j], "id") == 0) {
            // REMOVE COM BASE NO ID
            if(RemoveBinDataID(binFile, indexFileName, duplas, campName, campValue, n, atoi(campValue[j]), &headerOp)) {
                flagID = 1;
            }else{
                fclose(binFile);
                return false;
            }
        }
    }

    // SE NÃO HOUVER CAMPO ID, BUSCA SEQUENCIAL NO BINARIO
    if(flagID == 0) {
        data = CreateData(1);

        char status; // Verifica se chegou ao final do arquivo

        if(headerOp == NULL || data == NULL) {
            fclose(binFile);
            return false;
        }

        fread(&statusCab, 1, 1, binFile);

        // Se o arquivo estiver inconsistente, retorna falso
        if(statusCab == '0') {
            fclose(binFile);
            return false;
        }

        fseek(binFile, 0, SEEK_SET);
        fwrite(&statusConsist, 1, 1, binFile); //status vira 0, pois irá manipular o arquivo

        // Lê o resto do cabeçalho
        fread(&topoCab, 8, 1, binFile);
        fread(lixo, 8, 1, binFile);
        fread(&onReg, 4, 1, binFile);
        fread(&offReg, 4, 1, binFile);

        headerOp->top = topoCab;
        headerOp->onRegistersNumber = onReg;
        headerOp->offRegistersNumber = offReg;

        // Lê o arquivo
        while(fread(&status, sizeof(char), 1, binFile) != 0) {
            data[0]->removed = status;
            fread(&(data[0]->registerSize), sizeof(int), 1, binFile);

            // SE O REGISTRO ESTIVER REMOVIDO, PULA
            if(status == '1') {
                fread(lixo, data[0]->registerSize - 5, 1, binFile);
                begin += data[0]->registerSize;
            }
            else {

            // LE AS INFORMAÇOES DO REGISTRO
                removedByteOffSet = begin;

                fread(&(data[0]->nextRegister), sizeof(long long), 1, binFile);

                // Lê o ID e a idade do jogador
                fread(&(data[0]->playerId), sizeof(int), 1, binFile);
                fread(&(data[0]->playerAge), sizeof(int), 1, binFile);

                // Lê os campos variáveis e os tamanhos dos campos
                fread(&(data[0]->playerNameSize), sizeof(int), 1, binFile);
                fread(data[0]->playerName, data[0]->playerNameSize, 1, binFile);
                (data[0]->playerName)[data[0]->playerNameSize] = '\0';

                fread(&(data[0]->playerNatSize), sizeof(int), 1, binFile);
                fread(data[0]->playerNat, data[0]->playerNatSize, 1, binFile);
                (data[0]->playerNat)[data[0]->playerNatSize] = '\0';

                fread(&(data[0]->playerClubSize), sizeof(int), 1, binFile);
                fread(data[0]->playerClub, data[0]->playerClubSize, 1, binFile);
                (data[0]->playerClub)[data[0]->playerClubSize] = '\0';

                begin += data[0]->registerSize;

                campCounter = 0;
                
                // Itera o registro sobre todos campos pedidos na busca
                // COMPARA OS CAMPOS DE BUSCA COM O REGISTRO
                for(int j = 0; j < duplas; j++) {
                    
                    // Campo de busca Idade
                    if(strcmp(campName[j], "idade") == 0) {
                        if(data[0]->playerAge == (atoi(campValue[j]))){
                            campCounter++;
                        }
                    }

                    // Campo de busca nomeJogador
                    if(strcmp(campName[j], "nomeJogador") == 0) {
                        if(data[0]->playerNameSize != 0) {
                            if(strcmp(data[0]->playerName, campValue[j]) == 0) {
                                campCounter++;
                            }
                        }
                    }
                    
                    // Campo de busca nacionalidade
                    if(strcmp(campName[j], "nacionalidade") == 0) {
                        if(data[0]->playerNatSize != 0) {
                            if(strcmp(data[0]->playerNat, campValue[j]) == 0) {
                                campCounter++;
                            }
                        }
                    }

                    // Campo de busca nomeClube
                    if(strcmp(campName[j], "nomeClube") == 0) {
                        if(data[0]->playerClubSize != 0) {
                            if(strcmp(data[0]->playerClub, campValue[j]) == 0) {
                                campCounter++;
                            }
                        }
                    }
                }

                // SE TODOS OS CAMPOS DE BUSCA FOREM ENCONTRADOS, REMOVE
                if(campCounter == duplas) {
                    data[0]->removed = '1';

                    fseek(binFile, -data[0]->registerSize, SEEK_CUR);
                    fwrite(&data[0]->removed, 1, 1, binFile); //seta como removido
                    fseek(binFile, (data[0]->registerSize) - 1, SEEK_CUR);

                    headerOp->offRegistersNumber++;
                    headerOp->onRegistersNumber--;


                    long long previousByteoffset = -99; //seta essa variavel para -99
                    //se entrar e for logo inserir no começo, previous sera -99
                    //desse modo, é possível tratar desse caso

                    //depois de setar como removido, insere na lista encadeada
                    sortedInsertion(binFile, headerOp->top, previousByteoffset, data[0]->registerSize, removedByteOffSet, &headerOp);

                    fseek(binFile, begin, SEEK_SET);
                }
            }
        }

        // Atualiza os números de registros removidos e não removidos
        fseek(binFile, 17, SEEK_SET);
        fwrite(&headerOp->onRegistersNumber, sizeof(int), 1, binFile);
        fwrite(&headerOp->offRegistersNumber, sizeof(int), 1, binFile);

        // Se a última busca for realizada, libera memória dos registros
        FreeMemoryData(&data, 1);
    }

    statusConsist = '1';

    fseek(binFile, 0, SEEK_SET);
    fwrite(&statusConsist, 1, 1, binFile); //terminou, entao seta para consistente novamente

    fclose(binFile);
    return true;
}

// Remove o registro com base no valor do id
bool RemoveBinDataID(FILE* binFile, char* indexFileName, int duplas, char** campName, char** campValue, int n, int IDValue, HEADER** headerOp) {
    FILE* indexFile;
    DATA** dataToBeRemoved = CreateData(1);
    char status; // pega o status
    long long byteOffSetToBeRemoved;
    char lixo[200];
    int onReg, offReg; // Registros não removidos e removidos
    long long topoCab;
    char statusCab;
    char statusConsist = '0';
    int indice = 0;

    if(*headerOp == NULL || dataToBeRemoved == NULL) {
        return false;
    }

    fread(&statusCab, 1, 1, binFile);

    // Se o arquivo estiver inconsistente, retorna falso
    if(statusCab == '0') {
        return false;
    }

    fseek(binFile, 0, SEEK_SET);
	fwrite(&statusConsist, 1, 1, binFile); //status vira 0, pois irá manipular o arquivo

    // Lê o resto do cabeçalho
    fread(&topoCab, 8, 1, binFile);
    fread(lixo, 8, 1, binFile);
    fread(&onReg, 4, 1, binFile);
    fread(&offReg, 4, 1, binFile);

    (*headerOp)->top = topoCab;
    (*headerOp)->onRegistersNumber = onReg;
    (*headerOp)->offRegistersNumber = offReg;

    // VETORES PARA LEITURA DO REGISTRO DE INDICES
    int IDVector[2000];
    long long IDbyteOffSetVector[2000];

    if((indexFile = fopen(indexFileName, "rb")) == NULL)
        return false;

    fread(&status, 1, 1, indexFile);

    // Se o arquivo estiver inconsistente, imprime
    // o erro e sai da função
    if(status == '0') {
        fclose(indexFile);
        printf("Falha no processamento do arquivo.\n");
        return false;
    }

    // Lê todo o índice
    while(fread(&IDVector[indice], 4, 1, indexFile) != 0){
        fread(&IDbyteOffSetVector[indice], 8, 1, indexFile);
        indice++;
    }

    fclose(indexFile);

    // Busca binária pelo byteoffset do id
    byteOffSetToBeRemoved = binarySearchID(IDVector, IDbyteOffSetVector, IDValue, 0, indice);

    // Se não achar o id
    if(byteOffSetToBeRemoved == 0) {
        return false;
    }

    fseek(binFile, byteOffSetToBeRemoved, SEEK_SET);

    fread(&status, sizeof(char), 1, binFile);

    // Atualiza o campo "removido"
    dataToBeRemoved[0]->removed = status;
        
    // Atualiza o tamanho do registro e o próximo registro removido
    fread(&(dataToBeRemoved[0]->registerSize), sizeof(int), 1, binFile);
    fread(&(dataToBeRemoved[0]->nextRegister), sizeof(long long), 1, binFile);

    // Atualiza o ID e a idade do jogador
    fread(&(dataToBeRemoved[0]->playerId), sizeof(int), 1, binFile);
    fread(&(dataToBeRemoved[0]->playerAge), sizeof(int), 1, binFile);

    // Atualiza os campos variáveis e os tamanhos dos campos
    fread(&(dataToBeRemoved[0]->playerNameSize), sizeof(int), 1, binFile);
    fread(dataToBeRemoved[0]->playerName, dataToBeRemoved[0]->playerNameSize, 1, binFile);
    (dataToBeRemoved[0]->playerName)[dataToBeRemoved[0]->playerNameSize] = '\0';

    fread(&(dataToBeRemoved[0]->playerNatSize), sizeof(int), 1, binFile);
    fread(dataToBeRemoved[0]->playerNat, dataToBeRemoved[0]->playerNatSize, 1, binFile);
    (dataToBeRemoved[0]->playerNat)[dataToBeRemoved[0]->playerNatSize] = '\0';

    fread(&(dataToBeRemoved[0]->playerClubSize), sizeof(int), 1, binFile);
    fread(dataToBeRemoved[0]->playerClub, dataToBeRemoved[0]->playerClubSize, 1, binFile);
    (dataToBeRemoved[0]->playerClub)[dataToBeRemoved[0]->playerClubSize] = '\0';

    int regFinded = 0;
    int campCounter = 0;

    // Verifica os campos de busca com os campos do registro
    for(int j = 0; j < duplas; j++) {
        // Campo de busca ID
        if(strcmp(campName[j], "id") == 0) {
            // Verifica se os dados batem
            if(dataToBeRemoved[0]->playerId == (atoi(campValue[j]))) {
                campCounter++;
            }
            continue;
        }

        // Campo de busca Idade
        if(strcmp(campName[j], "idade") == 0) {
            if(dataToBeRemoved[0]->playerAge == (atoi(campValue[j])))
                campCounter++;
            continue;
        }

        // Campo de busca nomeJogador
        if(strcmp(campName[j], "nomeJogador") == 0) {
            if(dataToBeRemoved[0]->playerNameSize == 0)
                continue;
            if(strcmp(dataToBeRemoved[0]->playerName, campValue[j]) == 0)
                campCounter++;
            continue;
        }

        // Campo de busca nacionalidade
        if(strcmp(campName[j], "nacionalidade") == 0) {
            if(dataToBeRemoved[0]->playerNatSize == 0)
                continue;
            if(strcmp(dataToBeRemoved[0]->playerNat, campValue[j]) == 0)
                campCounter++;
            continue;
        }

        // Campo de busca nomeClube
        if(strcmp(campName[j], "nomeClube") == 0) {
            if(dataToBeRemoved[0]->playerClubSize == 0)
                continue;
            if(strcmp(dataToBeRemoved[0]->playerClub, campValue[j]) == 0)
                campCounter++;
            continue;
        }
    }

    // Se todos os campos de busca forem encontrados, remove o registro
    if(campCounter == duplas) {
        dataToBeRemoved[0]->removed = '1';

        fseek(binFile, -dataToBeRemoved[0]->registerSize, SEEK_CUR);
        fwrite(&dataToBeRemoved[0]->removed, 1, 1, binFile);
        fseek(binFile, (dataToBeRemoved[0]->registerSize) - 1, SEEK_CUR);
            
        (*headerOp)->offRegistersNumber++;
        (*headerOp)->onRegistersNumber--;

        long long previousByteoffset = -99;//seta essa variavel para -99
        //se entrar e for logo inserir no começo, previous sera -99
        //desse modo, é possível tratar desse caso

        //depois de setar como removido, insere na lista encadeada
        sortedInsertion(binFile, (*headerOp)->top, previousByteoffset, dataToBeRemoved[0]->registerSize, byteOffSetToBeRemoved, headerOp);

        regFinded++;
    }

    // Atualiza o cabeçalho
    fseek(binFile, 17, SEEK_SET);
    fwrite(&(*headerOp)->onRegistersNumber, sizeof(int), 1, binFile);
    fwrite(&(*headerOp)->offRegistersNumber, sizeof(int), 1, binFile);

    // Nenhum registro bate com as buscas
    if(regFinded == 0) {
        return false;
    }

    FreeMemoryData(&dataToBeRemoved, 1);

    return true;
}

// Busca binária pelo id com infos do arquivo de índice
long long binarySearchID(int* IDVector, long long* byteOffSetVector, int IDValue, int begin, int end) {
    int i = (begin + end)/2;

    if(begin > end)  /* Ponto de parada. Item não está no vetor */
        return 0;

    if (IDVector[i] == IDValue) /* Item encontrado */
        return byteOffSetVector[i];

    if (IDVector[i] < IDValue)  /* Item está no sub-vetor à direita */
        return binarySearchID(IDVector, byteOffSetVector, IDValue, i + 1, end);
    else  /* vector[i] > item. Item está no sub-vetor à esquerda */
        return binarySearchID(IDVector, byteOffSetVector, IDValue, begin, i - 1);
}

void sortedInsertion(FILE* binFile, long long currentByteoffset, long long previousByteoffset, int newRegisterSize, long long newByteoffset, HEADER** headerOp) {

    int currentRegisterSize; //tamanho do registro atual
    long long nextByteoffset; //prox byteoffset
    long long menosUm = -1; //caso insira no final ou na lista anteriormente vazia

    if(currentByteoffset != -1) { //header->topo != -1 ou byteoffset da recursao anterior != -1

        fseek(binFile, currentByteoffset + 1, SEEK_SET); //vai ate o regsitro a ser comparado
        fread(&currentRegisterSize, sizeof(int), 1, binFile);
        fread(&nextByteoffset, sizeof(long long), 1, binFile); //armazena infos para serem comparadas
        if(newRegisterSize < currentRegisterSize) {
            if(previousByteoffset != -99) {//nao acabou de entrar na função sorted insertion
                //INSERE NO MEIO DA LISTA

                fseek(binFile, previousByteoffset + 5, SEEK_SET);
                fwrite(&newByteoffset, sizeof(long long), 1, binFile);
                fseek(binFile, newByteoffset + 5, SEEK_SET);//seta previous, new e current bytesoffset
                fwrite(&currentByteoffset, sizeof(long long), 1, binFile);
                return;

            }else{ //INSERE NO COMEÇO DA LISTA
                fseek(binFile, newByteoffset + 5, SEEK_SET);
                fwrite(&((*headerOp)->top), sizeof(long long), 1, binFile);
                fseek(binFile, 1, SEEK_SET);//seta header->top e newbyteoffset
                (*headerOp)->top = newByteoffset;
                fwrite(&newByteoffset, sizeof(long long), 1, binFile);
                return;
            }

        }else{
            sortedInsertion(binFile, nextByteoffset, currentByteoffset, newRegisterSize, newByteoffset, headerOp);
        }

    }else{ //header->topo == -1 ou proxbyteoffset anterior == -1

        if((*headerOp)->offRegistersNumber == 1) {//ENTRA AQUI SOMENTE SE O TOPO DO CABECALHO SEJA -1
        //lista anteriormente vazia

            fseek(binFile, 1, SEEK_SET);
            (*headerOp)->top = newByteoffset;
            fwrite(&newByteoffset, sizeof(long long), 1, binFile);//seta o header->top e o newbyteoffset
            fseek(binFile, newByteoffset + 5, SEEK_SET);
            fwrite(&menosUm, sizeof(long long), 1, binFile);
            return;
        }else{// INSERIR FINAL DA LISTA

            fseek(binFile, previousByteoffset + 5, SEEK_SET);
            fwrite(&newByteoffset, sizeof(long long), 1, binFile);
            fseek(binFile, newByteoffset + 5, SEEK_SET);//manipula pra inserir no final da lista
            fwrite(&menosUm, sizeof(long long), 1, binFile);
            return;
        }
    }
}

// Ordena os ids e byteoffsets por QuickSort
void SortRegisters(long long** bytesOffSet) {
    QuickSort(0, header->onRegistersNumber - 1, bytesOffSet);
}

void QuickSort(int begin, int end, long long** bytesOffSet) {
    if(begin < end) {
        int index = SortElements(begin, end, bytesOffSet);
        QuickSort(begin, index - 1, bytesOffSet);
        QuickSort(index, end, bytesOffSet);
    }
}

int SortElements(int begin, int end, long long** bytesOffSet) {
    int pivot = data[(begin + end)/2]->playerId;
    long long aux;

    while(begin <= end) {
        while(data[begin]->playerId < pivot)
            begin++;
        while(data[end]->playerId > pivot)
            end--;
        if(begin <= end) {
            SwapRegisters(begin, end);
            aux = (*bytesOffSet)[begin];
            (*bytesOffSet)[begin] = (*bytesOffSet)[end];
            (*bytesOffSet)[end] = aux;
            begin++;
            end--;
        }
    }
    return begin;
}

// Troca os ids dos registros reg1 e reg2
void SwapRegisters(int reg1, int reg2) {
    DATA* dataAux = (DATA*) malloc(sizeof(DATA));

    dataAux->playerId = data[reg1]->playerId;
    data[reg1]->playerId = data[reg2]->playerId;
    data[reg2]->playerId = dataAux->playerId;

    free(dataAux);
    dataAux = NULL;
}

// Escreve no arquivo de índices
void WriteRegistersIndex(FILE* indexFile, long long* bytesOffSet) {
    char stats = '0';
    fwrite(&stats, sizeof(char), 1, indexFile);

    for(int i = 0; i < header->onRegistersNumber; i++) {
        fwrite(&(data[i]->playerId), sizeof(int), 1, indexFile);
        fwrite(&(bytesOffSet[i]), sizeof(long long), 1, indexFile);
    }

    fseek(indexFile, 0, SEEK_SET);
    stats = '1';
    fwrite(&stats, sizeof(char), 1, indexFile);

    FreeMemoryData(&data, header->onRegistersNumber);
    FreeMemoryHeader(&header);
}

// Libera memória alocada para o registro de cabeçalho
void FreeMemoryHeader(HEADER** header) {
    free(*header);
    *header = NULL;
}

// Libera memória alocada para os registros de dados
void FreeMemoryData(DATA*** data, int registers) {
    for(int i = 0; i < registers; i++) {
        if((*data)[i] != NULL) {
            if(((*data)[i]->playerNameSize) != 0)
                free((*data)[i]->playerName);
            if(((*data)[i]->playerNatSize) != 0)
                free((*data)[i]->playerNat);
            if(((*data)[i]->playerClubSize) != 0)
                free((*data)[i]->playerClub);
            free((*data)[i]);
            (*data)[i] = NULL;
        }
    }

    free(*data);
    *data = NULL;
}

// Faz uma busca na árvore b sem o uso do id
void searchWithoutId(FILE* inFile, int buscaAtual, char** slots, int nSlots) {
    char status;
    char lixo[20];
    int onReg, offReg, total;

    DATA* dados = (DATA*) malloc(sizeof(DATA)); // Dados requisitados pela busca (slots)
    DATA* scan = (DATA*) malloc(sizeof(DATA)); // Dados do registro atual da leitura

    scan->playerClub = (char*) malloc(40);
    scan->playerName = (char*) malloc(40);
    scan->playerNat = (char*) malloc(40);

    dados->playerClub = (char*) malloc(40);
    dados->playerName = (char*) malloc(40);
    dados->playerNat = (char*) malloc(40);

    dados->playerAge = -1;
    dados->playerClubSize = -1;
    dados->playerNameSize = -1;
    dados->playerNatSize = -1;
    
    // Copia os dados da busca para o vetor slots
    for(int i = 0; i < nSlots * 2; i = i + 2) {
        if(strcmp(slots[i], "idade") == 0) {
            dados->playerAge = atoi(slots[i+1]);
            continue;
        }

        if(strcmp(slots[i], "nomeJogador") == 0) {
            dados->playerNameSize = strlen(slots[i+1]);
            strcpy(dados->playerName, slots[i+1]);
        }

        if(strcmp(slots[i], "nacionalidade") == 0) {
            dados->playerNatSize = strlen(slots[i+1]);
            strcpy(dados->playerNat, slots[i+1]);
        }

        if(strcmp(slots[i], "nomeClube") == 0) {
            dados->playerClubSize = strlen(slots[i+1]);
            strcpy(dados->playerClub, slots[i+1]);
        }
    }

    fseek(inFile, 1, SEEK_SET);
    fread(lixo, 16, 1, inFile);
    fread(&onReg, 4, 1, inFile);
    fread(&offReg, 4, 1, inFile);

    total = onReg + offReg;

    printf("Busca %d\n\n", buscaAtual + 1);

    // Itera sobre todos os registros do arquivo
    for(int i = 0; i < total; ++i) {
        fread(&(scan->removed), 1, 1, inFile);
        fread(&(scan->registerSize), sizeof(int), 1, inFile);

        if(scan->removed == '1') {
            fseek(inFile, scan->registerSize - 5, SEEK_CUR);
            continue;
        }

        fread(lixo, 12, 1, inFile);

        fread(&(scan->playerAge), sizeof(int), 1, inFile);

        fread(&(scan->playerNameSize), sizeof(int), 1, inFile);
        fread(scan->playerName, scan->playerNameSize, 1, inFile);
        (scan->playerName)[scan->playerNameSize] = '\0';

        fread(&(scan->playerNatSize), sizeof(int), 1, inFile);
        fread(scan->playerNat, scan->playerNatSize, 1, inFile);
        (scan->playerNat)[scan->playerNatSize] = '\0';

        fread(&(scan->playerClubSize), sizeof(int), 1, inFile);
        fread(scan->playerClub, scan->playerClubSize, 1, inFile);
        (scan->playerClub)[scan->playerClubSize] = '\0';

        // Verifica e compara os campos lidos com aqueles da busca

        if((dados->playerAge != -1) && (dados->playerAge != scan->playerAge)) {
            continue;
        }

        if((dados->playerNameSize != -1) && (dados->playerNameSize != scan->playerNameSize)) {
            continue;
        } else if(strcmp(dados->playerName, scan->playerName) != 0 && dados->playerNameSize != -1) {
            continue;
        }

        if(dados->playerNatSize != -1 && dados->playerNatSize != scan->playerNatSize) {
            continue;
        } else if(strcmp(dados->playerNat, scan->playerNat) != 0 && dados->playerNatSize != -1) {
            continue;
        }

        if(dados->playerClubSize != -1 && dados->playerClubSize != scan->playerClubSize) {
            continue;
        } else if(strcmp(dados->playerClub, scan->playerClub) != 0 && dados->playerClubSize != -1) {
            continue;
        }

        // Imprime as informações caso o registros seja válido para a busca
        if(scan->playerNameSize == -1)
            printf("Nome do Jogador: SEM DADO\n");
        else
            printf("Nome do Jogador: %s\n", scan->playerName);
            
        if(scan->playerNatSize == -1)
            printf("Nacionalidade do Jogador: SEM DADO\n");
        else
            printf("Nacionalidade do Jogador: %s\n", scan->playerNat);

        if(scan->playerClubSize == -1)
            printf("Clube do Jogador: SEM DADO\n\n");
        else
            printf("Clube do Jogador: %s\n\n", scan->playerClub);
        
    }

    free(scan->playerName);
    free(scan->playerNat);
    free(scan->playerClub);
    free(dados->playerName);
    free(dados->playerNat);
    free(dados->playerClub);
    free(scan);
    free(dados);
}