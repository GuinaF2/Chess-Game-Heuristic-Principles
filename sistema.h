#ifndef SISTEMA_H
#define SISTEMA_H

#include "defs.h"

// Menus e Telas
void telaInicial(FILE *arquivo);
void exibirLogo(void);
void ExibirMenu(FILE *arquivo, char *usuario1, char *usuario2);

// Controle do Jogo
void realizarJogada(char *usuariobranco, char *usuariopreto, FILE *arquivo);

// Arquivos e Salve
void salvar_jogo(void);
int carregar_jogo(void);

// Sistema de Login e Estatísticas
void realizarCadastro(FILE *arquivo);
bool realizarLogin(FILE *arquivo, char *user);
void atualizarEstatisticas(char *usuario, char *resultado);
void listarGanhadores(FILE *arquivo);

#endif