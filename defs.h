// defs.h
#ifndef DEFS_H
#define DEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <conio.h>
#include <limits.h>
#include <time.h>

// --- CONSTANTES ---
#define TAMANHO_TABULEIRO 8
#define NUM_CASAS (TAMANHO_TABULEIRO * TAMANHO_TABULEIRO)
#define MAX_JOGADAS 100
#define VALOR_XEQUE 1000
#define COR_COMPUTADOR "P"
#define ANSI_RESET "\x1b[0m"
#define ANSI_RED "\x1b[31m"
#define ANSI_YELLOW "\x1b[33m"

// --- ESTRUTURAS ---
typedef struct {
    char sigla; 
    char cor;   
} Peca;

typedef struct {
    int indice;
    int linha;
    int coluna;
    Peca *peca;
} Casa;

typedef struct {
    char tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO][2];
    char vez;
} EstadoJogo;

typedef struct {
    int origem_linha, origem_coluna;
    int destino_linha, destino_coluna;
    int valor_heuristico;
} Jogada;


extern Casa *tabuleiro[TAMANHO_TABULEIRO][TAMANHO_TABULEIRO];
extern char vez;

int abs(int x);

#endif