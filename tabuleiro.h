#ifndef TABULEIRO_H
#define TABULEIRO_H

#include "defs.h"

Peca *criar_peca(char sigla, char cor);
void inicializar_tabuleiro(void);
void imprimir_tabuleiro(void);
int converter_letra_para_indice(char c);

#endif