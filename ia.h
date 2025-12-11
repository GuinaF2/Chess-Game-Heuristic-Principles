#ifndef IA_H
#define IA_H

#include "defs.h"

// Funções auxiliares de análise
int valor_da_peca(Peca *p);
bool casa_sob_ataque(int linha, int coluna, char cor_adversaria);
bool protege_pecas_aliadas(int origem_linha, int origem_coluna, int destino_linha, int destino_coluna, char cor);
int avaliar_seguranca_rei(char cor);
int avaliar_estado(void);

// Geração e Ordenação
int gerar_jogadas(char cor, Jogada jogadas[]);
void ordenar_jogadas(Jogada jogadas[], int n);

// Minimax e Execução
int minimax(int profundidade, int maximizando, int alfa, int beta);
void jogada_computador(char *usuariobranco, char *usuariopreto, FILE *arquivo);

#endif