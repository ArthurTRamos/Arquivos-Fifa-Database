#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void binarioNaTela(char *nomeArquivoBinario) { /* Você não precisa entender o código dessa função. */

	/* Use essa função para comparação no run.codes. Lembre-se de ter fechado (fclose) o arquivo anteriormente.
	*  Ela vai abrir de novo para leitura e depois fechar (você não vai perder pontos por isso se usar ela). */

	unsigned long i, cs;
	unsigned char *mb;
	size_t fl;
	FILE *fs;
	if(nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
		fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar?\n");
		return;
	}
	fseek(fs, 0, SEEK_END);
	fl = ftell(fs);
	fseek(fs, 0, SEEK_SET);
	mb = (unsigned char *) malloc(fl);
	fread(mb, 1, fl, fs);

	cs = 0;
	for(i = 0; i < fl; i++) {
		cs += (unsigned long) mb[i];
	}
	printf("%lf\n", (cs / (double) 100));
	free(mb);
	fclose(fs);
}

void operation1() {
    char inFile [10];
    char outFile [13];

    scanf("%s %s", inFile, outFile);

    FILE* in = fopen(inFile, "r");
    FILE* out = fopen(outFile, "wb");

    char status = 0; // Inicia com 0 e, se tudo correr bem, termina com 1
    double topo = -1; // Byteoffset de um registro logicamente removido (-1 standart)
    double proxByteOffset = 0; // Próximo byteoffset disponível
    int nroRegArq = 0; // Número de registros que não foram logicamente removidos
    int nroRegRem = 0; // Número de Registros que foram removidos

    fwrite(&status, sizeof(char), 1, out);
    fwrite(&topo, sizeof(double), 1, out);
    fwrite(&proxByteOffset, sizeof(double), 1, out);
    fwrite(&nroRegArq, sizeof(int), 1, out);
    fwrite(&nroRegRem, sizeof(int), 1, out);

    char removido = 0; // Se o registro está removido ou não
    int tamReg; // Número de Bytes do Registro
    double Prox = -1; // Byteoffset do próximo registro logicamente removido
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

        printf("%d %d", id, idade);

        do {
            fread(&(nomeJogador[counter]), sizeof(char), 1, in);
            printf("%c", nomeJogador[counter]);
        } while(nomeJogador[counter++] != ',');

        tamNomeJogador = counter - 1;
        printf(" %d ", tamNomeJogador);
        counter = 0;


        do {
            fread(&(nacionalidadeJogador[counter]), sizeof(char), 1, in);
            printf("%c", nacionalidadeJogador[counter]);
        } while(nacionalidadeJogador[counter++] != ',');

        tamNacionalidade = counter - 1;
        printf(" %d ", tamNacionalidade);
        counter = 0;

        do {
            fread(&(clubeJogador[counter]), sizeof(char), 1, in);
            printf("%c", clubeJogador[counter]);
        } while(clubeJogador[counter++] != '\n' && clubeJogador[counter++] != '\r');

        tamClube = counter - 1;
        printf(" %d ", tamClube);

        tamReg = 33 + tamNomeJogador + tamNacionalidade + tamClube;
        printf("%d\n", tamReg);

        fwrite(&removido, sizeof(char), 1, out);
        fwrite(&tamReg, sizeof(int), 1, out);
        fwrite(&Prox, sizeof(double), 1, out);
        fwrite(&id, sizeof(int), 1, out);
        fwrite(&idade, sizeof(int), 1, out);
        fwrite(&tamNomeJogador, sizeof(int), 1, out);
        fwrite(nomeJogador, sizeof(char), tamNomeJogador, out);
        fwrite(&tamClube, sizeof(int), 1, out);
        fwrite(&clubeJogador, sizeof(char), tamClube, out);

        nroRegArq++;
    }

    status = '1';

    fseek(out, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, out);

    proxByteOffset = ftell(out);

    status = '1';

    fseek(out, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, out);
    fseek(out, 9, SEEK_SET);
    fwrite(&proxByteOffset, sizeof(double), 1, out);
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