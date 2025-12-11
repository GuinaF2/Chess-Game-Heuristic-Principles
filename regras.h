#ifndef REGRAS_H
#define REGRAS_H

#include "defs.h" // Necessário para conhecer 'Peca' e 'bool'

// Validações de movimento
int caminho_livre(int li, int ci, int lf, int cf);
int movimento_valido(Peca *peca, int li, int ci, int lf, int cf);
int contar_movimentos_validos(int linha, int coluna); // Geralmente usada aqui ou na IA

// Lógica de Xeque e Rei
void encontrar_rei(char cor, int *linha, int *coluna);
bool esta_em_cheque(char cor);
bool tem_movimentos_validos(char cor);
bool esta_em_cheque_mate(char cor);

// Ação de mover (precisa de FILE pois salva/atualiza estatisticas se ganhar)
void mover_peca(int linha_origem, int coluna_origem, int linha_destino, int coluna_destino, char *usuariobranco, char *usuariopreto, FILE *arquivo);

#endif