#include "bTree.h"

// Cabeçalho da Árvore
struct headerTree {
    char status; // Status do arquivo da árvore b (0/1 - inconsistente/consistente)
    int noRaiz; // RRN do nó raiz (-1 == inicial)
    int proxRRN; // Próximo RRN a ser criado (0 == inicial)
    int nroChaves; // Número de nós da árvore (0 == inicial)
};

// Item Promovido no Split
struct promoveSplit {
    char querry; // 0/1 - Não houve/Houve o Split
    int id; // Id splitado
    long long byteOffset; // ByteOffset splitado
    int leftRRN; // Nó origem do item splitado
    int rightRRN; // Nó criado anteriormente no split
    int alturaNo; // Altura do nó em que será inserido o item splitado
};

// Cria uma struct de cabeçalho
T_HEADER* CreateBHeader(char status, int noRaiz, int proxRRN, int nroChaves) {
    T_HEADER* header = (T_HEADER*) malloc(sizeof(T_HEADER));

    if(header != NULL) {
        header->status = status;
        header->noRaiz = noRaiz;
        header->proxRRN = proxRRN;
        header->nroChaves = nroChaves;
    }

    return  header;
}

// Cria uma Árvore B a partir de um arquivo binário
bool CreateBTree(FILE* inFile, FILE* treeFile) {
    T_HEADER* header;
    char status, dollarSign;
    char trash[200];
    int onReg, registerCounter, registerSize, id;
    long long byteOffset = 25;

    dollarSign = '$';
    registerCounter = 0;

    // Verifica a consistência do arquivo binário
    fread(&status, 1, 1, inFile);
    if(status == '0')
        return false;

    // Lê o restante do cabeçalho
    fread(trash, sizeof(long long), 2, inFile);
    fread(&onReg, sizeof(int), 1, inFile);
    fread(trash, sizeof(int), 1, inFile);

    status = '0';

    // Configura o arquivo árvore b como inconsistente
    fwrite(&status, sizeof(char), 1, treeFile);
    
    header = CreateBHeader(0, -1, 0, 0);

    if(header == NULL)
        return false;

    PROMOVE* promove = (PROMOVE*) malloc(sizeof(PROMOVE));
    if(promove == NULL)
        return false;

    // Lê todos os registros não removidos e adiciona na árvore
    while(fread(&status, 1, 1, inFile) != 0 && registerCounter != onReg) {
        promove->querry = '0';
        fread(&registerSize, sizeof(int), 1, inFile);

        // Registro removido
        if(status == '1') {
            fread(trash, registerSize - 5, 1, inFile);
            byteOffset += registerSize;
            continue;
        }

        fread(trash, sizeof(long long), 1, inFile);
        fread(&id, sizeof(int), 1, inFile);
        fread(trash, registerSize - 17, 1, inFile);

        // Insere na árvore b
        InsertNode(treeFile, &header, &promove, id, byteOffset);
        
        byteOffset = byteOffset + registerSize;
        registerCounter++;
    }

    header->status = '1';

    // Atualiza o cabeçalho do arquivo da árvore b
    fseek(treeFile, 1, SEEK_SET);
    fwrite(&(header->noRaiz), sizeof(int), 1, treeFile);
    fwrite(&(header->proxRRN), sizeof(int), 1, treeFile);
    fwrite(&(header->nroChaves), sizeof(int), 1, treeFile);

    for(int i = 0; i < 47; i++)
        fwrite(&dollarSign, 1, 1, treeFile);
    
    // Atualiza o status do arquivo
    fseek(treeFile, 0, SEEK_SET);
    fwrite(&(header->status), 1, 1, treeFile);

    FreeTreeHeader(&header);
    free(promove);
    promove = NULL;
    return true;
}

// Insere um elemento de id/byteoffset na árvore b
void InsertNode(FILE* treeFile, T_HEADER** header, PROMOVE** promove, int id, long long byteOffset) {
    // Árvore vazia
    if((*header)->nroChaves == 0)
        InsertFirstNode(treeFile, header, 0, 0, id, byteOffset, -1, -1, 1);

    // Árvore não vazia
    else {
        InsertNodeAux(treeFile, header, promove, (*header)->noRaiz, id, byteOffset);
        // Criação de um novo nó raiz
        if((*promove)->querry == '1')
            InsertFirstNode(treeFile, header, (*header)->proxRRN, (*promove)->alturaNo, (*promove)->id,     (*promove)->byteOffset, (*promove)->leftRRN, (*promove)->rightRRN, 1);
    }

    // Atualiza o contador de nós da árvore
    (*header)->nroChaves++;
}

// Insere o primeiro elemento num nó de RRN especificado
// Option => 0/1 - não muda/muda o rootNode
void InsertFirstNode(FILE* treeFile, T_HEADER** header, int RRN, int alturaNo, int id, long long byteOffset, int leftRRN, int rightRRN, int option) {
    // Jump para o byteoffset do nó
    fseek(treeFile, 60 * (RRN + 1), SEEK_SET);

    // Variáveis Auxiliares
    int nroChaves = 1;
    int nulo1 = -1;
    long long nulo2 = -1;

    // Escreve os dados
    fwrite(&alturaNo, sizeof(int), 1, treeFile);
    fwrite(&nroChaves, sizeof(int), 1, treeFile);
    fwrite(&id, sizeof(int), 1, treeFile);
    fwrite(&byteOffset, sizeof(long long), 1, treeFile);

    // Atualiza os itens restantes
    for(int i = 0; i < 2; i++) {
        fwrite(&nulo1, sizeof(int), 1, treeFile);
        fwrite(&nulo2, sizeof(long long), 1, treeFile);
    }

    // Atualiza os dois primeiros filhos (RRNs)
    fwrite(&leftRRN, sizeof(int), 1, treeFile);
    fwrite(&rightRRN, sizeof(int), 1, treeFile);

    // Atualiza os nós filhos restantes
    for(int j = 0; j < 2; j++)
        fwrite(&nulo1, sizeof(int), 1, treeFile);

    // Atualiza o cabeçalho com a troca (ou não) do nó raiz
    if(option == 1)
        (*header)->noRaiz = RRN;
    (*header)->proxRRN++;
}

// Insere um novo item numa árvore não vazia
void InsertNodeAux(FILE* treeFile, T_HEADER** header, PROMOVE** promove, int RRN, int id, long long byteOffset) {
    // Vai para o nó do estado
    fseek(treeFile, 60 * (RRN + 1), SEEK_SET);

    // Parâmetros do nó
    int alturaNo;
    int nroChaves;

    fread(&alturaNo, sizeof(int), 1, treeFile);
    fread(&nroChaves, sizeof(int), 1, treeFile);

    // Chegou no nó folha (para fazer a inserção)
    if(alturaNo == 0) {
        // Nó cheio (precisa de split)
        if(nroChaves == ORDER - 1)
            Split(treeFile, header, promove, alturaNo, nroChaves, RRN, id, byteOffset, -1);

        // Nó com espaço
        else {
            // Insere num nó não cheio
            InsertNonFullNode(treeFile, header, RRN, alturaNo, nroChaves, id, byteOffset, -1, -1);
            (*promove)->querry = '0';
        }
    }
    // Procura pelo próximo RRN para continuar a inserção
    else {
        int RRNtoGo = SearchNextRRN(treeFile, nroChaves, id, byteOffset);
        InsertNodeAux(treeFile, header, promove, RRNtoGo, id, byteOffset);

        // Se não for retorno de split
        if((*promove)->querry == '0')
            return;
        // Se for retorno de split
        else {
            fseek(treeFile, 60 * (RRN + 1), SEEK_SET);
            fread(&alturaNo, sizeof(int), 1, treeFile);
            fread(&nroChaves, sizeof(int), 1, treeFile);
            // Precisará de Split
            if(nroChaves == ORDER -1)
                Split(treeFile, header, promove, alturaNo, nroChaves, RRN, (*promove)->id, (*promove)->byteOffset, (*promove)->rightRRN);
            // Não Precisará de Split
            else {
                InsertNonFullNode(treeFile, header, RRN, alturaNo, nroChaves, (*promove)->id, (*promove)->byteOffset, (*promove)->leftRRN, (*promove)->rightRRN);
                (*promove)->querry = '0';
            }
        }
    }
}

// Faz o processo de split no nó com determinado RRN
// Promove o 2º elemento após a inserção ordenada
void Split(FILE* treeFile, T_HEADER** header, PROMOVE** promove, int alturaNo, int nroChaves, int RRN, int id, long long byteOffset, int rightRRN) {
    int keys[ORDER]; // Chaves (Ids)
    long long bytes[ORDER]; // Byte Offsets
    int rrn[ORDER + 1]; // RRNs
    char trash[10];
    // counter - controla quantidade de elementos já lidos do nó
    // save - índice de inserção ordenada da nova chave
    // testKey/Byte - Id e Byte Offset lidos e comparados
    int nulo1, i, counter, save, testKey;
    long long nulo2, testByte;

    nulo1 = -1;
    nulo2 = -1;
    i = 0;
    counter = ORDER-1;

    // Itera sobre os elementos do nó
    for(; i < nroChaves; i++) {
        fread(&testKey, sizeof(int), 1, treeFile);
        fread(&testByte, sizeof(long long), 1, treeFile);
        counter--;

        // Verifica se encontrou o local correto de inserção da nova chave
        if(id < testKey) {
            keys[i] = id;
            keys[i + 1] = testKey;
            bytes[i] = byteOffset;
            bytes[i + 1] = testByte;
            save = i;
            i = i + 2;
            break;
        // Senão encontrou, adiciona o item lido nos arrays
        }else{
            keys[i] = testKey;
            bytes[i] = testByte;
        }
    }

    // Se o novo item for inserido no final do array
    if(id > keys[i - 1]) {
        keys[i] = id;
        bytes[i] = byteOffset;
        save = i;
    }

    // Adiciona os itens restantes do nó, se necessário
    for(int j = 0; j < counter; j++) {
        fread(&(keys[i]), sizeof(int), 1, treeFile);
        fread(&(bytes[i]), sizeof(long long), 1, treeFile);
        i++;
    }

    // Insere os RRNs na ordem correta
    for(int j = 0; j < ORDER + 1; j++) {
        if(j == save + 1)
            rrn[j] = rightRRN;
        else 
            fread(&(rrn[j]), sizeof(int), 1, treeFile);
    }

    // Volta para antes do primeiro item
    fseek(treeFile, -52, SEEK_CUR);

    // Escreve o elemento remanescente do nó
    fwrite(&(keys[0]), sizeof(int), 1, treeFile);
    fwrite(&(bytes[0]), sizeof(long long), 1, treeFile);
    
    for(int i = 0; i < 2; i++) {
        fwrite(&nulo1, sizeof(int), 1, treeFile);
        fwrite(&nulo2, sizeof(long long), 1, treeFile);
    }

    // Escreve os rrns remanescentes
    fwrite(&rrn[0], sizeof(int), 1, treeFile);
    fwrite(&rrn[1], sizeof(int), 1, treeFile);
    fwrite(&nulo1, sizeof(int), 1, treeFile);
    fwrite(&nulo1, sizeof(int), 1, treeFile);

    // Retorna para o início do nó
    fseek(treeFile, -60, SEEK_CUR);
    fread(&nulo1, 4, 1, treeFile);
    nroChaves = 1;
    // Atualiza a quantidade de chaves
    fwrite(&nroChaves, 4, 1, treeFile);
    
    // Insere o primeiro elemento no novo nó
    InsertFirstNode(treeFile, header, (*header)->proxRRN, alturaNo, keys[2], bytes[2], rrn[2], rrn[3], 0);
    fseek(treeFile, 60 * ((*header)->proxRRN), SEEK_SET);
    fread(&trash, sizeof(int), 2, treeFile);
    // Insere o elemento restante no novo nó criado
    InsertNonFullNode(treeFile, header, (*header)->proxRRN - 1, alturaNo, 1, keys[3], bytes[3], rrn[3], rrn[4]);

    // Seta o elemento para promoção
    (*promove)->querry = '1';
    (*promove)->id = keys[1];
    (*promove)->byteOffset = bytes[1];
    (*promove)->leftRRN = RRN;
    (*promove)->rightRRN = (*header)->proxRRN - 1;
    (*promove)->alturaNo = alturaNo + 1;
}

// Insere um novo item num nó não cheio
void InsertNonFullNode(FILE* treeFile, T_HEADER** header, int RRN, int alturaNo, int nroChaves, int id, long long byteOffset, int leftRRN, int rightRRN) {
    int testId, i, rrnAux; 
    int trash[100];
    long long testByte;

    i = 0;

    // Procura pelo lugar de inserção
    for(; i < nroChaves; i++) {
        // Lê o item do nó
        fread(&testId, sizeof(int), 1, treeFile);
        fread(&testByte, sizeof(long long), 1, treeFile);

        // Achou o local de inserção do nó
        if(id < testId) {
            // Shift para a direita os itens necessários
            ShiftItens(treeFile, nroChaves - i, testId, testByte);
            // Volta para a posição de inserção
            fseek(treeFile, -(12 * (nroChaves - i + 1)), SEEK_CUR);

            // Escreve o novo id
            fwrite(&id, sizeof(int), 1, treeFile);
            fwrite(&byteOffset, sizeof(long long), 1, treeFile);

            for(int j = 0; j < 2 - i; j++)
                fread(trash, 12, 1, treeFile);
            break;
        }
    }

    // Se for inserir no final
    if(id > testId) {
        fwrite(&id, sizeof(int), 1, treeFile);
        fwrite(&byteOffset, sizeof(long long), 1, treeFile);
        for(int j = 0; j < 2 - i; j++)
            fread(trash, 12, 1, treeFile);
    }

    for(int j = 0; j < i+1; j++)
        fread(&rrnAux, sizeof(int), 1, treeFile);
    
    fread(&rrnAux, sizeof(int), 1, treeFile);

    // Shift para a direita dos RRNs necessários
    ShiftRRN(treeFile, 2 - i, rrnAux);

    int deslocamento = 4 * (3 - i);

    // Volta para local de inserção do novo RRN
    fseek(treeFile, -deslocamento, SEEK_CUR);
    fwrite(&rightRRN, sizeof(int), 1, treeFile);

    // Atualiza cabeçalho do nó
    nroChaves++;
    fseek(treeFile, 60 * (RRN + 1), SEEK_SET);
    fread(&alturaNo, sizeof(int), 1, treeFile);
    fwrite(&nroChaves, sizeof(int), 1, treeFile);
}

// Procura pelo próximo RRN para inserir id/byteOffset
int SearchNextRRN(FILE* treeFile, int nroChaves, int id, long long byteOffset) {
    int testId, i, RRNtoGo, counter;
    long long testByte;

    counter = ORDER - 1;

    i = 0;
    
    // Procura pela posição em que Id ficaria
    for(; i < nroChaves; i++) {
        fread(&testId, sizeof(int), 1, treeFile);
        fread(&testByte, sizeof(long long), 1, treeFile);
        counter--;

        if(id < testId)
            break;
    }

    // Pula para a parte dos RRNs
    for(int j = 0; j < counter; j++) {
        fread(&testId, sizeof(int), 1, treeFile);
        fread(&testByte, sizeof(long long), 1, treeFile);
    }

    // Lê o RRN correto
    for(int k = 0; k <= i; k++)
        fread(&RRNtoGo, sizeof(int), 1, treeFile);

    return RRNtoGo;
}

// Shifta os itens necessários para a direita do nó
void ShiftItens(FILE* treeFile, int shiftRequired, int id, long long byteOffSet) {
    if(shiftRequired == 0)
        return;
    
    int saveId = id;
    long long saveByteOffset = byteOffSet;

    fread(&id, sizeof(int), 1, treeFile);
    fread(&byteOffSet, sizeof(long long), 1, treeFile);
    fseek(treeFile, -12, SEEK_CUR);
    // Escreve os novos valores
    fwrite(&saveId, sizeof(int), 1, treeFile);
    fwrite(&saveByteOffset, sizeof(long long), 1, treeFile);
    ShiftItens(treeFile, shiftRequired - 1, id, byteOffSet);
}

// Shifta os RRNs necessários para a direita do nó
void ShiftRRN(FILE* treeFile, int shiftRequired, int rrnAux) {
    if(shiftRequired == 0)
        return;
    
    int saveRRN = rrnAux;

    fread(&rrnAux, sizeof(int), 1, treeFile);
    fseek(treeFile, -4, SEEK_CUR);
    // Escreve o novo valor
    fwrite(&saveRRN, sizeof(int), 1, treeFile);
    ShiftRRN(treeFile, shiftRequired - 1, rrnAux);
}

// Retorna o Byte Offset do item com chave específica por IDtoBeFound
long long SearchRegister(FILE* treeFile, int RRN, int IDtoBeFound) {
    // Se não achar o item com a chave, retorna -1
    if(RRN == -1)
        return -1;

    // Vai para o nó do estado
    fseek(treeFile, 60 * (RRN + 1), SEEK_SET);

    // Parâmetros do nó
    int alturaNo;
    int nroChaves;

    fread(&alturaNo, sizeof(int), 1, treeFile);
    fread(&nroChaves, sizeof(int), 1, treeFile);

    int testId, i, RRNtoGo, counter;
    long long testByte;

    counter = ORDER - 1;

    i = 0;

    // Itera sobre os elementos do nó
    for(; i < nroChaves; i++) {
        fread(&testId, sizeof(int), 1, treeFile);
        fread(&testByte, sizeof(long long), 1, treeFile);
        counter--;

        // Se encontrou com item com id especificado
        if(IDtoBeFound == testId)
            return testByte;

        // Se o item não está no nó
        if(IDtoBeFound < testId)
            break;
    }

    // Pula para a parte dos RRNs
    for(int j = 0; j < counter; j++) {
        fread(&testId, sizeof(int), 1, treeFile);
        fread(&testByte, sizeof(long long), 1, treeFile);
    }

    // Lê o RRN correto
    for(int k = 0; k <= i; k++)
        fread(&RRNtoGo, sizeof(int), 1, treeFile);

    return(SearchRegister(treeFile, RRNtoGo, IDtoBeFound));
}

// Libera memória alocado para o header
void FreeTreeHeader(T_HEADER** header) {
    free(*header);
    *header = NULL;
}

// Insere um novo item na árvore
void insertReg(T_HEADER** header, PROMOVE** promove, FILE* inFile, FILE* treeFile, int id, long int byteOffSet) {
    (*promove)->querry = '0';
    // Insere na árvore b
    InsertNode(treeFile, header, promove, id, byteOffSet);
}

// Configura o header e o promove para a inserção
bool insertPreparations(T_HEADER** header, PROMOVE** promove, FILE* treeFile) {
    char status = '0';
    char dollarSign = '$';

    fseek(treeFile, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, treeFile);
    
    *header = CreateBHeader(0, -1, 0, 0);
    
    if(*header == NULL)
        return false;

    fread(&((*header)->noRaiz), sizeof(int), 1, treeFile);
    fread(&((*header)->proxRRN), sizeof(int), 1, treeFile);
    fread(&((*header)->nroChaves), sizeof(int), 1, treeFile);

    *promove = (PROMOVE*) malloc(sizeof(PROMOVE));
    if(*promove == NULL)
        return false;
}

// Finaliza as inserções ao reescrever o header
void insertFinalizations(T_HEADER** header, PROMOVE** promove, FILE* treeFile) {
    char dollarSign = '$';

    // Atualiza o cabeçalho do arquivo da árvore b
    fseek(treeFile, 1, SEEK_SET);
    fwrite(&((*header)->noRaiz), sizeof(int), 1, treeFile);
    fwrite(&((*header)->proxRRN), sizeof(int), 1, treeFile);
    fwrite(&((*header)->nroChaves), sizeof(int), 1, treeFile);
}