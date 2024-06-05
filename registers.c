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
void Inserir(FILE* inFile, int NInsercoes, int teste) {
    char removed = '0';
    long int nextRegister = -1;
    char c = '$';

    int Id; // ID do jogador
    int Age; // Idade do jogador
    int NameSize;
    char* Name; // Nome do jogador
    int NatSize;
    char* Nat; // Nacionalidade do jogador
    int ClubSize;
    char* Club; // Clube do jogador

    int n, tam, lixo, byteOffSet, cifrao, nReg, tFinal, writeTam, nRem, nRemAux;
    long int PBO; // Próximo byteoffset
    char* vetor;

    nRemAux = 0; // Número de registros inseridos
    tFinal = 0;

    vetor = (char*) malloc(40 * sizeof(char));
    Nat = (char*) malloc(40 * sizeof(char));
    Name = (char*) malloc(40 * sizeof(char));
    Club = (char*) malloc(40 * sizeof(char));
    
    for(int i = 0; i < NInsercoes; ++i) {
        cifrao = 0;

        ClubSize = 0;
        NameSize = 0;
        NatSize = 0;

        // Lê as informações do registro
        scan_quote_string(vetor);
        Id = atoi(vetor);

        scan_quote_string(vetor);
        Age = atoi(vetor);

        if(Age == 0)
            Age = -1;

        scan_quote_string(vetor);
        strcpy(Name, vetor);
        NameSize = strlen(vetor); 

        scan_quote_string(vetor);
        strcpy(Nat, vetor);
        NatSize = strlen(vetor);

        scan_quote_string(vetor);
        strcpy(Club, vetor);
        ClubSize = strlen(vetor);     

        tam = ClubSize + NameSize + NatSize + 33;

        // Busca pelo registro capaz de corportar o tamanho tam
        byteOffSet = FindNo(tam, inFile);

        // Insere o registro no final do arquivo binário
        if(byteOffSet == -1) {
            fseek(inFile, 0, SEEK_END);
            cifrao = 0;
            writeTam = tam;
            tFinal = tFinal + tam;
        // Insere no registro de byteoffset encontrado
        } else {
            fseek(inFile, byteOffSet, SEEK_SET);
            fread(&lixo, 1, 1, inFile);
            fread(&writeTam, sizeof(int), 1, inFile);
            cifrao = writeTam - tam;
            fseek(inFile, byteOffSet, SEEK_SET);
            nRemAux++;
        }

        // Insere todas as informações do novo registro no arquivo binário
        fwrite(&(removed), sizeof(char), 1, inFile);
        fwrite(&(writeTam), sizeof(int), 1, inFile);
        fwrite(&(nextRegister), sizeof(long int), 1, inFile);
        fwrite(&(Id), sizeof(int), 1, inFile);
        fwrite(&(Age), sizeof(int), 1, inFile);
        fwrite(&(NameSize), sizeof(int), 1, inFile);
        fwrite(Name, NameSize * sizeof(char), 1, inFile);
        fwrite(&(NatSize), sizeof(int), 1, inFile);
        fwrite(Nat, NatSize * sizeof(char), 1, inFile);
        fwrite(&(ClubSize), sizeof(int), 1, inFile);
        fwrite(Club, ClubSize * sizeof(char), 1, inFile); 
        
        // Escreve os cifrões necessários no fim do registro
        for(int i = 0; i < cifrao; ++i)
            fwrite(&c, sizeof(char), 1, inFile);
    }

    // Atualiza os dados do cabeçalho
    fseek(inFile, 17, SEEK_SET);
    PBO = teste + tFinal;
    fread(&nReg, 4, 1, inFile);
    nReg = nReg + NInsercoes;
    fread(&nRem, 4, 1, inFile);
    nRem = nRem - nRemAux;
    fseek(inFile, 9, SEEK_SET);
    fwrite(&PBO, sizeof(long int), 1, inFile);
    fwrite(&nReg, sizeof(int), 1, inFile);
    fwrite(&nRem, sizeof(int), 1, inFile);

    free(Nat);
    free(Club);
    free(Name);
    free(vetor);
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

    int begin = 25; // byteoffset inicial de registros
    long long removedByteOffSet;
    long long topoCab; // topo do cabeçalho

    flagID = 0;

    if((binFile = fopen(inFileName, "rb+")) == NULL)
        return false;

    // Itera sobre os campos de busca e verifica se há o id
    for(int j = 0; j < duplas; j++){
        if(strcmp(campName[j], "id") == 0) {
            // Remove com base no id
            if(RemoveBinDataID(binFile, indexFileName, duplas, campName, campValue, n, atoi(campValue[j]), &headerOp)) {
                flagID = 1;
            }else{
                return false;
            }
        }
    }

    // Se não houver o id
    if(flagID == 0) {
        data = CreateData(1);

        char status; // Verifica se chegou ao final do arquivo

        if(headerOp == NULL || data == NULL)
            return false;

        fread(&statusCab, 1, 1, binFile);

        // Se o arquivo estiver inconsistente, retorna falso
        if(statusCab == '0') {
            fclose(binFile);
            return false;
        }

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

            // Se o registro estiver removido, pula
            if(status == '1') {
                fread(lixo, data[0]->registerSize - 5, 1, binFile);
                begin += data[0]->registerSize;
            }
            else {
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
                // Compara os campos com o registro lido anteriormente
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

                // Se todos os campos de busca forem encontrados, remove
                if(campCounter == duplas) {
                    data[0]->removed = '1';

                    fseek(binFile, -data[0]->registerSize, SEEK_CUR);
                    fwrite(&data[0]->removed, 1, 1, binFile);
                    fseek(binFile, (data[0]->registerSize) - 1, SEEK_CUR);

                    headerOp->offRegistersNumber++;
                    headerOp->onRegistersNumber--;
                    // Insere o registro na lista de registros removidos
                    insertRemovedNonSortedList(headerOp->top, binFile, removedByteOffSet, &headerOp);

                    fseek(binFile, begin, SEEK_SET);
                }
            }
        }

        // Atualiza os números de registros removidos e não removidos
        fseek(binFile, 17, SEEK_SET);
        fwrite(&headerOp->onRegistersNumber, sizeof(int), 1, binFile);
        fwrite(&headerOp->offRegistersNumber, sizeof(int), 1, binFile);

        long long* byteOffSetVector = (long long*) malloc(headerOp->offRegistersNumber * sizeof(long long));
        int* sizeRegisterVector = (int*) malloc(headerOp->offRegistersNumber * sizeof(int));

        byteOffSetVector[0] = headerOp->top;

        removedSortedList(headerOp->top, binFile, byteOffSetVector, sizeRegisterVector, 0, &headerOp);

        // Se a última busca for realizada, libera memória dos registros
        FreeMemoryData(&data, 1);
    }

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
    int indice = 0;

    if(*headerOp == NULL || dataToBeRemoved == NULL)
        return false;

    fread(&statusCab, 1, 1, binFile);

    // Se o arquivo estiver inconsistente, retorna falso
    if(statusCab == '0') {
        fclose(binFile);
        return false;
    }

    // Lê o resto do cabeçalho
    fread(&topoCab, 8, 1, binFile);
    fread(lixo, 8, 1, binFile);
    fread(&onReg, 4, 1, binFile);
    fread(&offReg, 4, 1, binFile);

    (*headerOp)->top = topoCab;
    (*headerOp)->onRegistersNumber = onReg;
    (*headerOp)->offRegistersNumber = offReg;

    // Vetores para manipulação do índice
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
        fclose(binFile);
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
        insertRemovedNonSortedList((*headerOp)->top, binFile, byteOffSetToBeRemoved, headerOp);

        regFinded++;
    }

    // Atualiza o cabeçalho
    fseek(binFile, 17, SEEK_SET);
    fwrite(&(*headerOp)->onRegistersNumber, sizeof(int), 1, binFile);
    fwrite(&(*headerOp)->offRegistersNumber, sizeof(int), 1, binFile);

    // Nenhum registro bate com as buscas
    if(regFinded == 0) {
        fclose(binFile);
        return false;
    }
    
    long long* byteOffSetVector = (long long*) malloc((*headerOp)->offRegistersNumber * sizeof(long long));
    int* sizeRegisterVector = (int*) malloc((*headerOp)->offRegistersNumber * sizeof(int));

    byteOffSetVector[0] = (*headerOp)->top;

    removedSortedList((*headerOp)->top, binFile, byteOffSetVector, sizeRegisterVector, 0, headerOp);

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

// Insere o novo registro removido no final da lista de byteoffsets removidos
void insertRemovedNonSortedList(long long topo, FILE* binFile, long long byteOffSetToBeInserted, HEADER** headerOp) {
    long long auxTopo;
    long long menosUm = -1;
    
    if(topo != -1) {
        fseek(binFile, topo + 5, SEEK_SET);
        fread(&auxTopo, sizeof(long long), 1, binFile);
        if(auxTopo == -1) {
            fseek(binFile, -8, SEEK_CUR);
            fwrite(&byteOffSetToBeInserted, sizeof(long long), 1, binFile);
            fseek(binFile, byteOffSetToBeInserted + 5, SEEK_SET);
            fwrite(&menosUm, sizeof(long long), 1, binFile);
            return;
        }
        insertRemovedNonSortedList(auxTopo, binFile, byteOffSetToBeInserted, headerOp);
    }else {
        fseek(binFile, 1, SEEK_SET);
        (*headerOp)->top = byteOffSetToBeInserted;
        fwrite(&byteOffSetToBeInserted, sizeof(long long), 1, binFile);
        fseek(binFile, byteOffSetToBeInserted + 5, SEEK_SET);
        fwrite(&menosUm, sizeof(long long), 1, binFile);
        return;
    }

}

// Obtém a lista de removidos com tamanho dos registros e byteoffsets
// Ordena a lista
void removedSortedList(long long topo, FILE* binFile, long long* byteOffSetVector, int* sizeRegisterVector, int keepTrack, HEADER** headerOp) {
    if((*headerOp)->offRegistersNumber == 0)
        return;

    // Obteve todos os registros removidos
    if(keepTrack == (*headerOp)->offRegistersNumber) {
        SortListRemove(&sizeRegisterVector, &byteOffSetVector, *headerOp);
        (*headerOp)->top = byteOffSetVector[0];
        fseek(binFile, 1, SEEK_SET);
        fwrite(&(*headerOp)->top, 8, 1, binFile);
        // Reescreve a lista de removidos no arquivo binário
        writeSortedList(sizeRegisterVector, byteOffSetVector, binFile, 0, *headerOp);
        return;
    }
    
    fseek(binFile, topo + 1, SEEK_SET); // pula removido
    fread(&sizeRegisterVector[keepTrack], sizeof(int), 1, binFile); // lê o tamanho do atual
    fread(&byteOffSetVector[keepTrack + 1], sizeof(long long), 1, binFile); // lê o próximo
    removedSortedList(byteOffSetVector[keepTrack + 1], binFile, byteOffSetVector, sizeRegisterVector,
    keepTrack + 1, headerOp);
}

// Ordena a lista de registros removidos pelo tamanho do registro
void SortListRemove(int **sizeRegisterVector, long long** bytesOffSetVector, HEADER* headerOp) {
    QuickSortRemove(0, headerOp->offRegistersNumber - 1, sizeRegisterVector, bytesOffSetVector);
}

void QuickSortRemove(int begin, int end, int **sizeRegisterVector, long long** bytesOffSetVector) {
    if(begin < end) {
        int index = SortElementsRemove(begin, end, sizeRegisterVector, bytesOffSetVector);
        QuickSortRemove(begin, index - 1, sizeRegisterVector, bytesOffSetVector);
        QuickSortRemove(index, end, sizeRegisterVector, bytesOffSetVector);
    }
}

int SortElementsRemove(int begin, int end, int **sizeRegisterVector, long long** bytesOffSet) {
    int pivot = (*sizeRegisterVector)[(begin + end)/2];
    long long aux;

    while(begin <= end) {
        while((*sizeRegisterVector)[begin] < pivot)
            begin++;
        while((*sizeRegisterVector)[end] > pivot)
            end--;
        if(begin <= end) {
            aux = (*bytesOffSet)[begin];
            (*bytesOffSet)[begin] = (*bytesOffSet)[end];
            (*bytesOffSet)[end] = aux;

            aux = (*sizeRegisterVector)[begin];
            (*sizeRegisterVector)[begin] = (*sizeRegisterVector)[end];
            (*sizeRegisterVector)[end] = aux;
            begin++;
            end--;
        }
    }
    return begin;
}

// Reescrita da lista de removidos no arquivo binário
void writeSortedList(int* sizeRegisterVector, long long* byteOffSetVector, FILE* binFile, int counter, HEADER* headerOp) {
    long long menosUm = -1;

    // Se chegar no último registro
    if(counter == (headerOp->offRegistersNumber) - 1) {
        fseek(binFile, byteOffSetVector[counter] + 5, SEEK_SET);
        fwrite(&menosUm, 8, 1, binFile);
        return;
    }
    
    fseek(binFile, byteOffSetVector[counter] + 5, SEEK_SET);
    fwrite(&byteOffSetVector[counter + 1], 8, 1, binFile);

    writeSortedList(sizeRegisterVector, byteOffSetVector, binFile, counter + 1, headerOp);
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

// Escreve no registro de índices
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