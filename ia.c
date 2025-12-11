#include "ia.h"
#include "regras.h"
#include "tabuleiro.h"
#include <limits.h> 

int minimax(int profundidade, int maximizando, int alfa, int beta)
{
    if (profundidade == 0)
    {
        return avaliar_estado();
    }

    Jogada jogadas[MAX_JOGADAS];
    int n = gerar_jogadas(maximizando ? 'P' : 'B', jogadas);

    if (n == 0)
    {
        if (esta_em_cheque(maximizando ? 'P' : 'B'))
        {
            return maximizando ? -100000 : 100000; // Xeque-mate
        }
        return 0; // Afogamento
    }

    ordenar_jogadas(jogadas, n);

    if (maximizando)
    {
        int maxEval = INT_MIN;
        for (int i = 0; i < n; i++)
        {
            Jogada j = jogadas[i];

            Peca *movida = tabuleiro[j.origem_linha][j.origem_coluna]->peca;
            Peca *capturada = tabuleiro[j.destino_linha][j.destino_coluna]->peca;

            tabuleiro[j.destino_linha][j.destino_coluna]->peca = movida;
            tabuleiro[j.origem_linha][j.origem_coluna]->peca = NULL;

            int eval = minimax(profundidade - 1, 0, alfa, beta);

            tabuleiro[j.origem_linha][j.origem_coluna]->peca = movida;
            tabuleiro[j.destino_linha][j.destino_coluna]->peca = capturada;

            if (eval > maxEval)
            {
                maxEval = eval;
            }

            alfa = (alfa > eval) ? alfa : eval;
            if (beta <= alfa)
            {
                break;
            }
        }
        return maxEval;
    }
    else
    {
        int minEval = INT_MAX;
        for (int i = 0; i < n; i++)
        {
            Jogada j = jogadas[i];

            Peca *movida = tabuleiro[j.origem_linha][j.origem_coluna]->peca;
            Peca *capturada = tabuleiro[j.destino_linha][j.destino_coluna]->peca;

            tabuleiro[j.destino_linha][j.destino_coluna]->peca = movida;
            tabuleiro[j.origem_linha][j.origem_coluna]->peca = NULL;

            int eval = minimax(profundidade - 1, 1, alfa, beta);

            tabuleiro[j.origem_linha][j.origem_coluna]->peca = movida;
            tabuleiro[j.destino_linha][j.destino_coluna]->peca = capturada;

            if (eval < minEval)
            {
                minEval = eval;
            }

            beta = (beta < eval) ? beta : eval;
            if (beta <= alfa)
            {
                break;
            }
        }
        return minEval;
    }
}
void jogada_computador(char *usuariobranco, char *usuariopreto, FILE *arquivo) {
    Jogada jogadas[MAX_JOGADAS];
    int n = gerar_jogadas('P', jogadas);
    
    if (n == 0) {
        printf("Computador não tem jogadas disponíveis.\n");
        return;
    }

    Jogada melhor = jogadas[0];
    int melhor_valor = INT_MIN;
    bool captura_vantajosa = false;

    // Avalia todas as jogadas
    for (int i = 0; i < n; i++) {
        Peca *peca = tabuleiro[jogadas[i].origem_linha][jogadas[i].origem_coluna]->peca;
        Peca *alvo = tabuleiro[jogadas[i].destino_linha][jogadas[i].destino_coluna]->peca;
        
        // Simula o movimento
        tabuleiro[jogadas[i].destino_linha][jogadas[i].destino_coluna]->peca = peca;
        tabuleiro[jogadas[i].origem_linha][jogadas[i].origem_coluna]->peca = NULL;
        
        bool em_cheque = esta_em_cheque('P');
        bool atacando_rei = (alvo && alvo->sigla == 'K');
        bool destino_atacado = casa_sob_ataque(jogadas[i].destino_linha, jogadas[i].destino_coluna, 'B');
        
        // Calcula valor heurístico
        int valor = 0;
        
        // Fator ofensivo (ataque)
        if (alvo) {
            valor += valor_da_peca(alvo) * 10; // Bônus por captura
            if (atacando_rei) valor += 5000;    // Bônus enorme por atacar o rei
        } else {
            valor += 5; // Pequeno bônus por movimento
        }
        
        // Fator defensivo
        if (casa_sob_ataque(jogadas[i].origem_linha, jogadas[i].origem_coluna, 'B')) {
            valor += 100; // Bônus por sair de casa atacada
        }
        if (destino_atacado) {
            valor -= valor_da_peca(peca); // Penalidade por mover para casa atacada
        }
        if (em_cheque) {
            valor = INT_MIN; // Invalida jogadas que deixam o rei em cheque
        }
        
        // Restaura tabuleiro
        tabuleiro[jogadas[i].origem_linha][jogadas[i].origem_coluna]->peca = peca;
        tabuleiro[jogadas[i].destino_linha][jogadas[i].destino_coluna]->peca = alvo;
        
        // Atualiza melhor jogada
        if (valor > melhor_valor) {
            melhor_valor = valor;
            melhor = jogadas[i];
            captura_vantajosa = (alvo != NULL);
        }
    }

    // Executa a melhor jogada encontrada
    printf("Computador joga de %c%d para %c%d\n",
           melhor.origem_coluna + 'A', 8 - melhor.origem_linha,
           melhor.destino_coluna + 'A', 8 - melhor.destino_linha);
    
    mover_peca(melhor.origem_linha, melhor.origem_coluna,
               melhor.destino_linha, melhor.destino_coluna,
               usuariobranco, usuariopreto, arquivo);
}
int avaliar_estado()
{
    int soma = 0;
    int rei_b_linha, rei_b_coluna;
    int rei_p_linha, rei_p_coluna;
    int capturas_potenciais = 0;

    encontrar_rei('B', &rei_b_linha, &rei_b_coluna);
    encontrar_rei('P', &rei_p_linha, &rei_p_coluna);

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            Peca *p = tabuleiro[i][j]->peca;
            int valor = valor_da_peca(p);
            if (p != NULL)
            {
                // Peças brancas somam, pretas subtraem
                soma += (p->cor == 'B') ? valor : -valor;

                // Bônus por peças atacando outras peças
                if (p->cor == 'P')
                    for (int k = 0; k < TAMANHO_TABULEIRO; k++)
                    {
                        for (int l = 0; l < TAMANHO_TABULEIRO; l++)
                        {
                            if (tabuleiro[k][l]->peca && tabuleiro[k][l]->peca->cor == 'B')
                            {
                                if (movimento_valido(p, i, j, k, l))
                                {
                                    // Bônus maior para ataques a peças valiosas
                                    int valor_alvo = valor_da_peca(tabuleiro[k][l]->peca);
                                    soma -= valor_alvo / 2; // Quanto mais valioso o alvo, maior o bônus
                                    capturas_potenciais++;
                                }
                            }
                        }
                    }
            }

            // Bônus adicional se houver múltiplas capturas potenciais
            if (capturas_potenciais > 1)
            {
                soma -= 200 * capturas_potenciais;
            }

            char cor = p->cor;
            char cor_inimiga = (cor == 'B') ? 'P' : 'B';

            // Verificar se a peça está sob ataque
            bool sob_ataque = casa_sob_ataque(i, j, cor_inimiga);
            bool protegida = casa_sob_ataque(i, j, cor);

            if (sob_ataque)
            {
                valor -= valor_da_peca(p);
            }
            // Penalizar peças sob ataque não protegidas
            if (sob_ataque && !protegida)
            {
                valor -= valor_da_peca(p) * 2; // Penalidade de 100% do valor
            }

            // Bônus por peças protegidas
            if (protegida)
            {
                valor += valor_da_peca(p) / 2; // Bônus de 25% do valor
            }

            // Penalidade maior se for peça valiosa sob ataque
            if (sob_ataque && (p->sigla == 'T' || p->sigla == 'Q' || p->sigla == 'B' || p->sigla == 'C'))
            {
                valor -= 300; // Penalidade adicional para rainha e torres
            }

            // Bônus por mobilidade (quantos movimentos a peça tem)
            valor += contar_movimentos_validos(i, j) * 15;

            soma += (cor == 'B' ? valor : -valor);

            if (esta_em_cheque('P'))
            {                // Se o computador (pretas) está em xeque
                soma -= 500; // Penalidade por estar em xeque
            }
            if (esta_em_cheque('B'))
            {                // Se o jogador (brancas) está em xeque
                soma += 200; // Bônus por dar xeque
            }
        }
    }

    // Avaliar segurança dos reis
    soma += avaliar_seguranca_rei('B') * 10;
    soma -= avaliar_seguranca_rei('P') * 10;

    return soma;
}
void ordenar_jogadas(Jogada jogadas[], int n)
{
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (jogadas[j].valor_heuristico < jogadas[j + 1].valor_heuristico)
            {
                Jogada tmp = jogadas[j];
                jogadas[j] = jogadas[j + 1];
                jogadas[j + 1] = tmp;
            }
}
int contar_movimentos_validos(int linha, int coluna)
{
    Peca *p = tabuleiro[linha][coluna]->peca;
    if (!p)
        return 0;

    int count = 0;
    for (int i = 0; i < TAMANHO_TABULEIRO; i++)
    {
        for (int j = 0; j < TAMANHO_TABULEIRO; j++)
        {
            if (movimento_valido(p, linha, coluna, i, j))
            {
                // Verificar se não é captura da mesma cor
                if (tabuleiro[i][j]->peca == NULL ||
                    tabuleiro[i][j]->peca->cor != p->cor)
                {
                    count++;
                }
            }
        }
    }
    return count;
}
bool casa_sob_ataque(int linha, int coluna, char cor_adversaria)
{
    for (int i = 0; i < TAMANHO_TABULEIRO; i++)
    {
        for (int j = 0; j < TAMANHO_TABULEIRO; j++)
        {
            if (tabuleiro[i][j]->peca != NULL &&
                tabuleiro[i][j]->peca->cor == cor_adversaria)
            {
                Peca *p = tabuleiro[i][j]->peca;
                // Verificação peão
                if (p->sigla == 'P')
                {
                    int direcao = (p->cor == 'B') ? -1 : 1;
                    if ((i + direcao == linha) && (j + 1 == coluna || j - 1 == coluna))
                    {
                        return true;
                    }
                    continue;
                }
                // Verifica normalmente para outras peças
                if (movimento_valido(p, i, j, linha, coluna))
                {
                    return true;
                }
            }
        }
    }
    return false;
}
int gerar_jogadas(char cor, Jogada jogadas[])
{
    int count = 0;
    char cor_inimiga = (cor == 'B') ? 'P' : 'B';

    for (int i = 0; i < TAMANHO_TABULEIRO; i++)
    {
        for (int j = 0; j < TAMANHO_TABULEIRO; j++)
        {
            Peca *p = tabuleiro[i][j]->peca;
            if (p && p->cor == cor)
            {
                bool peca_sob_ataque = casa_sob_ataque(i, j, cor_inimiga);
                for (int k = 0; k < TAMANHO_TABULEIRO; k++)
                {
                    for (int l = 0; l < TAMANHO_TABULEIRO; l++)
                    {
                        if (!movimento_valido(p, i, j, k, l))
                            continue;
                        if (tabuleiro[k][l]->peca && tabuleiro[k][l]->peca->cor == cor)
                            continue;

                        // Simular movimento
                        Peca *movida = p;
                        Peca *capturada = tabuleiro[k][l]->peca;
                        tabuleiro[k][l]->peca = movida;
                        tabuleiro[i][j]->peca = NULL;

                        bool rei_em_cheque = esta_em_cheque(cor);
                        bool destino_sob_ataque = casa_sob_ataque(k, l, cor_inimiga);
                        bool atacando_rei = (capturada && capturada->sigla == 'K');
                        bool da_xeque = esta_em_cheque(cor_inimiga); // Verifica se dá xeque
                        bool xeque_defendido = false;

                        // Desfazer movimento
                        tabuleiro[i][j]->peca = movida;
                        tabuleiro[k][l]->peca = capturada;

                        if (rei_em_cheque)
                            continue;

                        // Cálculo do valor heurístico
                        int valor = 0;

                        // Valor base da captura (muito aumentado)
                        if (capturada)
                        {
                            valor += valor_da_peca(capturada) * 20; // Aumentei o multiplicador

                            // Bônus extra por atacar o rei
                            if (atacando_rei)
                            {
                                valor += 5000; // Valor extremamente alto para atacar o rei
                            }

                            // Bônus por capturas defendidas
                            if (!destino_sob_ataque)
                            {
                                valor += valor_da_peca(capturada) * 2;
                            }
                        }

                        // Penalidade reduzida por mover para casa atacada
                        if (destino_sob_ataque && !capturada)
                        {
                            valor -= valor_da_peca(p) * 1; // Reduzi a penalidade
                        }

                        if (capturada == NULL &&
                            casa_sob_ataque(k, l, cor_inimiga) &&
                            !protege_pecas_aliadas(i, j, k, l, cor))
                        {
                            valor -= valor_da_peca(p) * 5; // Penalidade severa
                        }

                        // Bônus/penalidades posicionais
                        if (peca_sob_ataque && !casa_sob_ataque(k, l, cor_inimiga))
                        {
                            valor += valor_da_peca(p) * 2;
                        }
                        // Penalidade se move para casa sob ataque
                        if (casa_sob_ataque(k, l, cor_inimiga))
                        {
                            valor -= valor_da_peca(p) * 2; // Duas vezes o valor da peça
                        }
                        // Bônus se move para casa segura
                        if (!casa_sob_ataque(k, l, cor_inimiga) && peca_sob_ataque)
                        {
                            valor += valor_da_peca(p) * 1.3;
                        }
                        // Penalidade extra por perder peça importante
                        if (casa_sob_ataque(k, l, cor_inimiga) && (p->sigla == 'T' || p->sigla == 'R'))
                        {
                            valor -= valor_da_peca(p) * 3;
                        }
                        // Bonus se protege peça aliada
                        if (protege_pecas_aliadas(i, j, k, l, cor))
                        {
                            valor += 200;
                        }

                        // Valor do xeque
                        if (da_xeque)
                        {
                            if (!xeque_defendido)
                            {
                                valor += 1000; // Xeque não defendido - valor alto
                            }
                            else
                            {
                                // Xeque defendido - valor proporcional à peça que defende
                                valor += (capturada ? valor_da_peca(capturada) / 2 : 100);
                            }
                            if (p->sigla == 'R' && da_xeque && capturada == NULL)
                            {
                                if (!esta_em_cheque_mate(cor_inimiga))
                                {
                                    valor -= valor_da_peca(p) * 3; // Penalidade alta se não leva a xeque-mate
                                }
                            }
                            if (p->sigla == 'T' && da_xeque && capturada == NULL)
                            {
                                if (!esta_em_cheque_mate(cor_inimiga))
                                {
                                    valor -= valor_da_peca(p) * 2.5; // Penalidade alta se não leva a xeque-mate
                                }
                            }
                            if (p->sigla == 'C' && da_xeque && capturada == NULL)
                            {
                                if (!esta_em_cheque_mate(cor_inimiga))
                                {
                                    valor -= valor_da_peca(p) * 2; // Penalidade alta se não leva a xeque-mate
                                }
                            }
                            if (p->sigla == 'B' && da_xeque && capturada == NULL)
                            {
                                if (!esta_em_cheque_mate(cor_inimiga))
                                {
                                    valor -= valor_da_peca(p) * 2; // Penalidade alta se não leva a xeque-mate
                                }
                            }
                        }

                        jogadas[count++] = (Jogada){i, j, k, l, valor};
                    }
                }
            }
        }
    }
    return count;
}
bool protege_pecas_aliadas(int origem_linha, int origem_coluna, int destino_linha, int destino_coluna, char cor)
{
    // Simular o movimento
    Peca *movida = tabuleiro[origem_linha][origem_coluna]->peca;
    Peca *capturada = tabuleiro[destino_linha][destino_coluna]->peca;

    tabuleiro[destino_linha][destino_coluna]->peca = movida;
    tabuleiro[origem_linha][origem_coluna]->peca = NULL;

    // Verificar se agora protege peças aliadas
    bool protege = false;
    for (int i = 0; i < TAMANHO_TABULEIRO; i++)
    {
        for (int j = 0; j < TAMANHO_TABULEIRO; j++)
        {
            if (tabuleiro[i][j]->peca && tabuleiro[i][j]->peca->cor == cor)
            {
                // Se a peça estava sob ataque antes e não está mais
                bool estava_sob_ataque = casa_sob_ataque(i, j, (cor == 'B') ? 'P' : 'B');
                bool agora_sob_ataque = false;

                if (i != destino_linha || j != destino_coluna)
                { // Não verificar a peça movida
                    agora_sob_ataque = casa_sob_ataque(i, j, (cor == 'B') ? 'P' : 'B');
                }

                if (estava_sob_ataque && !agora_sob_ataque)
                {
                    protege = true;
                    break;
                }
            }
        }
        if (protege)
            break;
    }

    // Desfazer o movimento
    tabuleiro[origem_linha][origem_coluna]->peca = movida;
    tabuleiro[destino_linha][destino_coluna]->peca = capturada;

    return protege;
}
int avaliar_seguranca_rei(char cor)
{
    int rei_linha, rei_coluna;
    encontrar_rei(cor, &rei_linha, &rei_coluna);

    int seguranca = 0;
    char cor_inimiga = (cor == 'B') ? 'P' : 'B';

    // Penalidade por estar em cheque
    if (esta_em_cheque(cor))
    {
        seguranca -= 500;
    }

    // Verificar casas ao redor do rei
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
                continue;

            int nl = rei_linha + i;
            int nc = rei_coluna + j;

            if (nl >= 0 && nl < 8 && nc >= 0 && nc < 8)
            {
                if (casa_sob_ataque(nl, nc, cor_inimiga))
                {
                    seguranca -= 50;
                }
                else
                {
                    seguranca += 30;
                }
            }
        }
    }
    return seguranca;
}
