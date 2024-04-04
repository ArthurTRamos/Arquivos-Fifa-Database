#include "funcoes_fornecidas.h"

struct reg {
    char removido;
    int tamanho_reg;
    long long int prox;
    int id;
    int idade;
    int tam_jogador;
    char* jogador;
    int tam_nac;
    char* nac;
    int tam_clube;
    char* clube;
};

void op1();
void escrita_cabecalho();
int ler_dados(FILE*, FILE*, struct reg);
void escrita_dados(FILE*, struct reg);

int main() {
    int p;
    scanf("%d", &p);

    op1();
}

void op1(){
    char* inFile;
    char* outFile;

    scanf("%s", inFile);
    scanf("%s", outFile);

    FILE* in = fopen(inFile, "r");
    FILE* out = fopen(outFile, "wb");

    struct reg dados;

    dados.removido = 0;
    dados.tamanho_reg = 0;
    dados.prox = -1;

    escrita_cabecalho(out);
    int nRegArq = ler_dados(in, out, dados);

}

void escrita_cabecalho(FILE* out) {
    char status = '0';
    long long int topo = -1;
    long long int proxByteOffset = 0;
    int nRegArq = 0;
    int nRegRem = 0;

    fwrite(&status, sizeof(char), 1, out);
    fwrite(&topo, sizeof(long long int), 1, out);
    fwrite(&proxByteOffset, sizeof(long long int), 1, out);
    fwrite(&nRegArq, sizeof(int), 1, out);
    fwrite(&nRegRem, sizeof(int), 1, out);
}

int ler_dados(FILE* in, FILE* out, struct reg dados) {
    int nRegArq = 0;
    char lixo[45];
    fread(lixo, sizeof(char) * 45, 1, in);

    char linha[100];
    int i;
    char c = 'a';

    while(!feof(in)) {
        i = 0;
        while(c != '\n' && c != '\r') {
            fscanf("%c", c);
            linha[i] = c;
            i++;
        }
        linha[i] = '\0';

        char *token;
        token = strtok(linha, ',');
        dados.id = atoi(token);
        token = strtok(NULL, ',');
        dados.idade = atoi(token);
        
        token = strtok(NULL, ',');
        i = 0;
        while(token[i] != '\0') {
            dados.jogador[i] = token[i];
            i++;
        }
        dados.tam_jogador = i;

        token = strtok(NULL, ',');
        i = 0;
        while(token[i] != '\0') {
            dados.nac[i] = token[i];
            i++;
        }
        dados.tam_nac = i;

        token = strtok(NULL, ',');
        i = 0;
        while(token[i] != '\0') {
            dados.clube[i] = token[i];
            i++;
        }
        dados.tam_clube = i;

        escrita_dados(out, dados);
        nRegArq++;
    }
}

void escrita_dados(FILE* out, struct reg dados) {
    fwrite(dados.removido, sizeof(char), 1, out);
    fwrite(dados.tamanho_reg, sizeof(int), 1, out);
    fwrite(dados.prox, sizeof(long long int), 1, out);
    fwrite(dados.id, sizeof(int), 1, out);
    fwrite(dados.idade, sizeof(int), 1, out);
    fwrite(dados.tam_jogador, sizeof(int), 1, out);
    fwrite(dados.jogador, sizeof(char) * dados.tam_jogador, 1, out);
    fwrite(dados.tam_nac, sizeof(int), 1, out);
    fwrite(dados.nac, sizeof(char) * dados.tam_nac, 1, out);
    fwrite(dados.tam_clube, sizeof(int), 1, out);
    fwrite(dados.clube, sizeof(char) * dados.tam_clube, 1, out);
}