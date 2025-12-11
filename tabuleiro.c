#include "tabuleiro.h"

int abs(int x) {
    return x < 0 ? -x : x;
}
Peca *criar_peca(char sigla, char cor) {
    Peca *p = malloc(sizeof(Peca));
    p->sigla = sigla;
    p->cor = cor;
    return p;
}
void inicializar_tabuleiro()
{
    // Inicializa todas as casas como vazias
    for (int i = 0; i < TAMANHO_TABULEIRO; i++)
    {
        for (int j = 0; j < TAMANHO_TABULEIRO; j++)
        {
            tabuleiro[i][j] = malloc(sizeof(Casa));
            tabuleiro[i][j]->peca = NULL;
        }
    }

    // Coloca as peças brancas na base (linhas 6 e 7)
    tabuleiro[7][0]->peca = criar_peca('T', 'B'); // Torre
    tabuleiro[7][1]->peca = criar_peca('C', 'B'); // Cavalo
    tabuleiro[7][2]->peca = criar_peca('B', 'B'); // Bispo
    tabuleiro[7][3]->peca = criar_peca('R', 'B'); // Rainha
    tabuleiro[7][4]->peca = criar_peca('K', 'B'); // Rei
    tabuleiro[7][5]->peca = criar_peca('B', 'B'); // Bispo
    tabuleiro[7][6]->peca = criar_peca('C', 'B'); // Cavalo
    tabuleiro[7][7]->peca = criar_peca('T', 'B'); // Torre
    for (int j = 0; j < TAMANHO_TABULEIRO; j++)
    {
        tabuleiro[6][j]->peca = criar_peca('P', 'B'); // Peões
    }

    // Coloca as peças pretas no topo (linhas 0 e 1)
    tabuleiro[0][0]->peca = criar_peca('T', 'P'); // Torre
    tabuleiro[0][1]->peca = criar_peca('C', 'P'); // Cavalo
    tabuleiro[0][2]->peca = criar_peca('B', 'P'); // Bispo
    tabuleiro[0][3]->peca = criar_peca('R', 'P'); // Rainha
    tabuleiro[0][4]->peca = criar_peca('K', 'P'); // Rei
    tabuleiro[0][5]->peca = criar_peca('B', 'P'); // Bispo
    tabuleiro[0][6]->peca = criar_peca('C', 'P'); // Cavalo
    tabuleiro[0][7]->peca = criar_peca('T', 'P'); // Torre
    for (int j = 0; j < TAMANHO_TABULEIRO; j++)
    {
        tabuleiro[1][j]->peca = criar_peca('P', 'P'); // Peões
    }
}
void imprimir_tabuleiro()
{
    printf("=== VEZ: %s ===\n", (vez == 'B') ? "BRANCAS (B)" : "PRETAS (P)");
    printf("\n    ");
    for (char c = 'A'; c <= 'H'; c++)
        printf(" %c  ", c);
    printf("\n");

    for (int i = 0; i < TAMANHO_TABULEIRO; i++)
    {
        printf("  +---+---+---+---+---+---+---+---+\n");
        printf("%d ", TAMANHO_TABULEIRO - i);
        for (int j = 0; j < TAMANHO_TABULEIRO; j++)
        {
            Casa *casa = tabuleiro[i][j];
            printf("|");
            if (casa->peca)
            {
                const char *cor = (casa->peca->cor == 'B') ? ANSI_YELLOW : ANSI_RED;
                printf(" %s%c" ANSI_RESET " ", cor, casa->peca->sigla);
            }
            else
            {
                printf("   ");
            }
        }
        printf("| %d\n", TAMANHO_TABULEIRO - i);
    }

    printf("  +---+---+---+---+---+---+---+---+\n");
    printf("    ");
    for (char c = 'A'; c <= 'H'; c++)
        printf(" %c  ", c);
    printf("\n\n");
}
int converter_letra_para_indice(char c)
{
    if (c >= 'A' && c <= 'H')
        return c - 'A';
    if (c >= 'a' && c <= 'h')
        return c - 'a';
    return -1;
}
