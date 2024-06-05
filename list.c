#include "list.h"

struct no {
    long int nextByteOffSet; // Byteoffset do próximo removido
    long int currentByteOffSet; // Byteoffset do atual removido
    int tam; // Tamanho do registro
    NO *next;
};

NO* pai;

// Remove um nó com base em seu nó anterior
void RemoveNo(NO* anterior, FILE* inFile) {
    NO* aux;
    
    // Verifica se o nó anteriror é o pai
    // Se não for, só altera o registro de anterior
    if(pai->currentByteOffSet != anterior->currentByteOffSet) {
        aux = anterior->next;
        anterior->next = aux->next;
        anterior->nextByteOffSet = anterior->next->currentByteOffSet;

        fseek(inFile, anterior->currentByteOffSet + 5, SEEK_SET);
        fwrite(&(anterior->nextByteOffSet), sizeof(long long), 1, inFile);
    // Se for o pai, altera o topo do arquivo binário
    } else {
        aux = pai->next;
        pai->next = aux->next;

        fseek(inFile, 1, SEEK_SET);
        fwrite(&(aux->nextByteOffSet), sizeof(long long), 1, inFile);
    }
    free(aux);
}

// Cria a lista de removidos do arquivo
int CreateList(FILE* inFile) {
    pai = CreateNo();

    char lixo;
    long int topo, ByteOffSet;
    int tam;

    fseek(inFile, 0, SEEK_SET);
    
    fread(&lixo, 1, 1, inFile);
    fread(&topo, 8, 1, inFile);
    
    long int teste; // byteoffset de fim do arquivo
    fread(&teste, 8, 1, inFile);

    // Se não houver removidos, só retorna o último byte do arquivo
    if(topo <= 1)
        return teste;

    fseek(inFile, topo, SEEK_SET);
    fread(&lixo, sizeof(char), 1, inFile);

    fread(&tam, sizeof(int), 1, inFile);

    fread(&ByteOffSet, sizeof(long), 1, inFile);

    // Insere o primeiro registro removido na lista
    InserirNo(tam, ByteOffSet, topo);
    
    NO *lista = pai->next;

    // Adiciona os outros registros removidos
    while(ByteOffSet != -1) {
        fseek(inFile, ByteOffSet, SEEK_SET);
        fread(&lixo, 1, 1, inFile);

        fread(&tam, 4, 1, inFile);
        fread(&ByteOffSet, 8, 1, inFile);

        InserirNo(tam, ByteOffSet, lista->nextByteOffSet);

        lista = lista->next;
    }
    return teste;
}

// Insere um nó de tamanho de registro tam, próximo byteoffset removido e
// byteoffset atual (de forma ordenada)
void InserirNo(int tam, long int nextByteOffSet, long int currentByteOffSet) {
    NO *ponteiro = pai;
    NO *prox = CreateNo();

    prox->nextByteOffSet = nextByteOffSet;
    prox->next = NULL;
    prox->tam = tam;
    prox->currentByteOffSet = currentByteOffSet;

    if(pai->next == NULL) {
        pai->next = prox;
        return;
    }

    // Procura pelo lugar correto de inserção de prox
    while(ponteiro->next != NULL) {
        if(ponteiro->next->tam > prox->tam)
            break;
        ponteiro = ponteiro->next;
    }

    prox->next = ponteiro->next;
    ponteiro->next = prox;
}

// Aloca memória para um novo nó
NO* CreateNo() {
    NO* new = (NO*) malloc(sizeof(NO));

    if(new == NULL)
        return NULL;

    new->next = NULL;
    new->nextByteOffSet = -1;
    new->currentByteOffSet = -1;
    new->tam = 0;

    return new;
}

// Procura por um nó de tamanho >= a tam
long int FindNo(int tam, FILE* inFile) {
    if(pai->next == NULL)
        return -1;

    long int ByteOffSet;
    NO *lista = pai;

    while(lista->next != NULL) {
        ByteOffSet = lista->next->currentByteOffSet;

        if(lista->next->tam >= tam) {
            RemoveNo(lista, inFile);
            return ByteOffSet;
        } 
        else
            lista = lista->next;
    }
    return -1;
}

// Limpa/Desaloca a lista
void ClearList(FILE* inFile) {
    NO* aux;
    NO* base = pai->next;

    free(pai);
    
    while(base != NULL) {
        aux = base;
        base = base->next;
        free(aux);
    }
}