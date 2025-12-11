#include "regras.h"      // O header dele mesmo
#include "tabuleiro.h"   // Para usar imprimir_tabuleiro()
#include "sistema.h"     // Para usar ExibirMenu() e atualizarEstatisticas()
#include <stdio.h>       // Para printf

int movimento_valido(Peca *peca, int li, int ci, int lf, int cf)
{
    int d_lin = lf - li; // Diferença de linha
    int d_col = cf - ci; // Diferença de coluna
    Casa *destino = tabuleiro[lf][cf];

    switch (peca->sigla)
    {
    case 'P': // Peão
        // Regras específicas para peões brancos e pretos
        if (peca->cor == 'B')
        {
            if (d_col == 0 && d_lin == -1 && destino->peca == NULL)
                return 1; // Movimento simples
            if (li == 6 && d_col == 0 && d_lin == -2 && tabuleiro[5][ci]->peca == NULL && destino->peca == NULL)
                return 1; // Movimento inicial de 2 casas
            if (abs(d_col) == 1 && d_lin == -1 && destino->peca != NULL && destino->peca->cor == 'P')
                return 1; // Captura
        }
        else
        {
            if (d_col == 0 && d_lin == 1 && destino->peca == NULL)
                return 1; // Movimento simples
            if (li == 1 && d_col == 0 && d_lin == 2 && tabuleiro[2][ci]->peca == NULL && destino->peca == NULL)
                return 1; // Movimento inicial de 2 casas
            if (abs(d_col) == 1 && d_lin == 1 && destino->peca != NULL && destino->peca->cor == 'B')
                return 1; // Captura
        }
        return 0;

    case 'T':                                                               // Torre
        return (d_lin == 0 || d_col == 0) && caminho_livre(li, ci, lf, cf); // Movimento em linha reta

    case 'B':                                                             // Bispo
        return abs(d_lin) == abs(d_col) && caminho_livre(li, ci, lf, cf); // Movimento diagonal

    case 'C':                                                                                // Cavalo
        return (abs(d_lin) == 2 && abs(d_col) == 1) || (abs(d_lin) == 1 && abs(d_col) == 2); // Movimento em "L"

    case 'R':                                                                                           // Rainha
        return (abs(d_lin) == abs(d_col) || d_lin == 0 || d_col == 0) && caminho_livre(li, ci, lf, cf); // Combinação de torre e bispo

    case 'K':                                      // Rei
        return abs(d_lin) <= 1 && abs(d_col) <= 1; // Movimento de uma casa em qualquer direção

    default:
        return 0; // Movimento inválido
    }
}
void mover_peca(int linha_origem, int coluna_origem, int linha_destino, int coluna_destino, char *usuariobranco, char *usuariopreto, FILE *arquivo)
{
    // Verifica se as coordenadas estao dentro do tabuleiro
    if (linha_origem < 0 || linha_origem >= TAMANHO_TABULEIRO || coluna_origem < 0 || coluna_origem >= TAMANHO_TABULEIRO ||
        linha_destino < 0 || linha_destino >= TAMANHO_TABULEIRO || coluna_destino < 0 || coluna_destino >= TAMANHO_TABULEIRO)
    {
        printf("Movimento invalido: coordenadas fora do tabuleiro.\n");
        return;
    }

    Casa *origem = tabuleiro[linha_origem][coluna_origem];
    Casa *destino = tabuleiro[linha_destino][coluna_destino];

    // Verifica se ha uma peca na origem
    if (origem->peca == NULL)
    {
        printf("Movimento invalido: nao ha peca na origem.\n");
        return;
    }
    if (origem->peca->cor != vez)
    {
        printf("Movimento invalido: Agora e a vez das pecas %s (%c)\n",
               (vez == 'B') ? "BRANCAS" : "PRETAS", vez);
        return;
    }
    // Verifica se eh a vez da cor correta
    if (origem->peca->cor != vez)
    {
        printf("Movimento invalido: nao eh a vez das pecas %c.\n", origem->peca->cor);
        return;
    }

    // Verifica se o movimento eh valido
    if (!movimento_valido(origem->peca, linha_origem, coluna_origem, linha_destino, coluna_destino))
    {
        printf("Movimento invalido para a peca %c.\n", origem->peca->sigla);
        return;
    }

    // Verifica se o destino nao contehm uma peca da mesma cor
    if (destino->peca != NULL && destino->peca->cor == origem->peca->cor)
    {
        printf("Movimento invalido: nao pode capturar peca da mesma cor.\n");
        return;
    }

    // Simula o movimento para verificar se o rei ficara em cheque
    Peca *peca_original = destino->peca;
    destino->peca = origem->peca;
    origem->peca = NULL;

    if (esta_em_cheque(vez))
    {
        // Desfaz o movimento se o rei ficar em cheque
        origem->peca = destino->peca;
        destino->peca = peca_original;
        printf("Movimento invalido: o rei nao pode ficar em cheque!\n");
        return;
    }

    // Movimento valido, atualiza o tabuleiro
    if (peca_original != NULL)
    {
        free(peca_original); // Libera a memória da peca capturada
    }

    // Verifica xeque-mate
    char adversario = (vez == 'B') ? 'P' : 'B';

    if (esta_em_cheque_mate(adversario))
    {
        imprimir_tabuleiro();
        printf("XEQUE-MATE! %s venceu!\n", (vez == 'B') ? "B" : "P");
        if (vez == 'B')
        {
            atualizarEstatisticas(usuariobranco, "vitoria");
            atualizarEstatisticas(usuariopreto, "derrota");
        }
        else
        {
            atualizarEstatisticas(usuariopreto, "vitoria");
            atualizarEstatisticas(usuariobranco, "derrota");
        }
        printf("Pressione qualquer tecla para continuar...");
        getch();
        ExibirMenu(arquivo, usuariobranco, usuariopreto);
    }
    else if (esta_em_cheque(adversario))
    {
        printf("XEQUE!\n");
    }

    // Alterna a vez
    vez = (vez == 'B') ? 'P' : 'B';
}
bool esta_em_cheque_mate(char cor)
{
    return esta_em_cheque(cor) && !tem_movimentos_validos(cor);
}
bool esta_em_cheque(char cor)
{
    int rei_linha, rei_coluna;
    encontrar_rei(cor, &rei_linha, &rei_coluna);

    for (int i = 0; i < TAMANHO_TABULEIRO; i++)
    {
        for (int j = 0; j < TAMANHO_TABULEIRO; j++)
        {
            if (tabuleiro[i][j]->peca != NULL && tabuleiro[i][j]->peca->cor != cor)
            {
                if (movimento_valido(tabuleiro[i][j]->peca, i, j, rei_linha, rei_coluna))
                {
                    return true; // Rei esta em cheque
                }
            }
        }
    }
    return false; // Rei nao esta em cheque
}
void encontrar_rei(char cor, int *linha, int *coluna)
{
    for (int i = 0; i < TAMANHO_TABULEIRO; i++)
    {
        for (int j = 0; j < TAMANHO_TABULEIRO; j++)
        {
            if (tabuleiro[i][j]->peca != NULL &&
                tabuleiro[i][j]->peca->sigla == 'K' &&
                tabuleiro[i][j]->peca->cor == cor)
            {
                *linha = i;
                *coluna = j;
                return;
            }
        }
    }
}
int valor_da_peca(Peca *p)
{
    if (!p)
        return 0;
    switch (p->sigla)
    {
    case 'P':
        return 100;
    case 'C':
        return 550;
    case 'B':
        return 575;
    case 'T':
        return 850;
    case 'R':
        return 2500;
    case 'K':
        return 19000;
    default:
        return 0;
    }
}
bool tem_movimentos_validos(char cor)
{
    for (int i = 0; i < TAMANHO_TABULEIRO; i++)
    {
        for (int j = 0; j < TAMANHO_TABULEIRO; j++)
        {
            if (tabuleiro[i][j]->peca != NULL && tabuleiro[i][j]->peca->cor == cor)
            {
                for (int k = 0; k < TAMANHO_TABULEIRO; k++)
                {
                    for (int l = 0; l < TAMANHO_TABULEIRO; l++)
                    {
                        // Impede tentativa de capturar peça da mesma cor
                        if (tabuleiro[k][l]->peca != NULL &&
                            tabuleiro[k][l]->peca->cor == cor)
                        {
                            continue;
                        }

                        // Verifica se é um movimento válido
                        if (movimento_valido(tabuleiro[i][j]->peca, i, j, k, l))
                        {
                            // 💡 Simula o movimento
                            Peca *peca_original = tabuleiro[k][l]->peca;
                            tabuleiro[k][l]->peca = tabuleiro[i][j]->peca;
                            tabuleiro[i][j]->peca = NULL;

                            bool em_cheque = esta_em_cheque(cor);

                            // 🔁 Desfaz o movimento
                            tabuleiro[i][j]->peca = tabuleiro[k][l]->peca;
                            tabuleiro[k][l]->peca = peca_original;

                            if (!em_cheque)
                            {
                                return true; // Encontrou um movimento válido
                            }
                        }
                    }
                }
            }
        }
    }
    return false; //  Nenhum movimento possível
}
int caminho_livre(int li, int ci, int lf, int cf)
{
    int d_lin = (lf - li) ? (lf - li) / abs(lf - li) : 0; // Direção da linha
    int d_col = (cf - ci) ? (cf - ci) / abs(cf - ci) : 0; // Direção da coluna
    int i = li + d_lin;
    int j = ci + d_col;

    while (i != lf || j != cf)
    {
        if (tabuleiro[i][j]->peca != NULL)
            return 0; // Há uma peça no caminho
        i += d_lin;
        j += d_col;
    }
    return 1; // Caminho livre
}
