#include "defs.h"
#include "tabuleiro.h"
#include "regras.h"
#include "ia.h"
#include "sistema.h"

Casa *tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO];
char vez = 'B';

int main(void) {
    FILE *arquivo = NULL;
    telaInicial(arquivo);
    return 0;
}