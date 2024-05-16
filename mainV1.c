#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funcoes_fornecidas.h"

void operation1() {
    char inFile [10];
    char outFile [13];

    scanf("%s %s", inFile, outFile);

    FILE* in = fopen(inFile, "r");
    FILE* out = fopen(outFile, "wb");

    char status = 0; // Inicia com 0 e, se tudo correr bem, termina com 1
    long long int topo = -1; // Byteoffset de um registro logicamente removido (-1 standart)
    long long int proxByteOffset = 0; // Próximo byteoffset disponível
    int nroRegArq = 0; // Número de registros que não foram logicamente removidos
    int nroRegRem = 0; // Número de Registros que foram removidos

    fwrite(&status, sizeof(char), 1, out);
    fwrite(&topo, sizeof(long long int), 1, out);
    fwrite(&proxByteOffset, sizeof(long long int), 1, out);
    fwrite(&nroRegArq, sizeof(int), 1, out);
    fwrite(&nroRegRem, sizeof(int), 1, out);

    char removido = 0; // Se o registro está removido ou não
    int tamReg = 0; // Número de Bytes do Registro
    long long int Prox = -1; // Byteoffset do próximo registro logicamente removido
    int id;
    int idade;
    int tamNomeJogador;
    char nomeJogador[100];
    int tamNacionalidade;
    char nacionalidadeJogador[100];
    int tamClube;
    char clubeJogador[100];
    char buffer;
    int counter = 0;

    fseek(in, 45, SEEK_SET); // Pula o ponteiro para o byteoffset do primeiro registro
    
    while(fread(&buffer, sizeof(char), 1, in) != 0) {
        fseek(in, -sizeof(char), SEEK_CUR); // Volta 1 byte

        fscanf(in, "%d", &id);
        fseek(in, sizeof(char), SEEK_CUR);
        fscanf(in, "%d", &idade);
        fseek(in, sizeof(char), SEEK_CUR);

        do {
            fscanf(in, "%c", &(nomeJogador[counter]));
        } while(nomeJogador[counter++] != ',');

        tamNomeJogador = counter - 1;
        counter = 0;

        do {
            fscanf(in, "%c", &(nacionalidadeJogador[counter]));
        } while(nacionalidadeJogador[counter++] != ',');

        tamNacionalidade = counter - 1;
        counter = 0;

        do {
            fscanf(in, "%c", &(clubeJogador[counter]));
        } while(clubeJogador[counter++] != '\n' && clubeJogador[counter++] != '\r');

        tamClube = counter - 1;

        tamReg = tamReg + 33 + tamNomeJogador + tamNacionalidade + tamClube;

        fwrite(&removido, sizeof(char), 1, out);
        fwrite(&tamReg, sizeof(int), 1, out);
        fwrite(&Prox, sizeof(long long int), 1, out);
        fwrite(&id, sizeof(int), 1, out);
        fwrite(&idade, sizeof(int), 1, out);
        fwrite(&tamNomeJogador, sizeof(int), 1, out);
        fwrite(nomeJogador, sizeof(char), tamNomeJogador, out);
        fwrite(&tamClube, sizeof(int), 1, out);
        fwrite(&clubeJogador, sizeof(char), tamClube, out);

        nroRegArq++;
    }

    status = '1';
    proxByteOffset = ftell(out);

    fseek(out, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, out);

    fseek(out, 9, SEEK_SET);
    fwrite(&proxByteOffset, sizeof(long long int), 1, out);
    fwrite(&nroRegArq, sizeof(int), 1, out);
    
    fclose(in);
    fclose(out);

    binarioNaTela(outFile);
}

int main(void) {
    int operation;

    scanf("%d", &operation);

    switch (operation) {
        case 1:
            operation1();
            /*char inFile [10];
            char outFile [13];

            scanf("%s %s", inFile, outFile);

            FILE* in = fopen(inFile, "r");
            FILE* out = fopen(outFile, "wb");*/

            //operation1(in, out);

            /*fclose(in);
            fclose(out);*/
            break;
    }

    return 0;
}
