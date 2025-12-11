#include "sistema.h"
#include "tabuleiro.h"
#include "regras.h"
#include "ia.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ExibirMenu(FILE *arquivo, char *usuario1, char *usuario2)
{

    while (1)
    {
        char op;
        system("cls");
        for (int i = 0; i < 10; i++)
        {
            printf(" -* ");
        }
        exibirLogo();
        for (int i = 0; i < 10; i++)
        {
            printf(" -* ");
        }
        printf("\n");
        printf("1. Novo Jogo\n");
        printf("2. Carregar Jogo\n");
        printf("3. Ultimos Ganhadores\n");
        printf("4. Sair\n");
        printf("\nMade By Guilherme Henrique\n");
        printf("Insira uma opcao: ");
        scanf("%c", &op);
        switch (op)
        {
        case '1':
            inicializar_tabuleiro();
            vez = 'B';
            realizarJogada(usuario1, usuario2, arquivo);
            break;
        case '2':
            if (carregar_jogo())
            {
                realizarJogada(usuario1, usuario2, arquivo);
            }
            break;
        case '3':
            listarGanhadores(arquivo);
            continue;
        case '4':
            exit(0);
        default:
            printf("Opcao invalida.\n");
        }
    }
}
void realizarJogada(char *usuariobranco, char *usuariopreto, FILE *arquivo)
{
    char entrada[32];
    char origem[3], destino[3];
    system("cls");

    while (1)
    {
        imprimir_tabuleiro();

        if (vez == 'P')
        {
            system("cls");
            printf("Vez do Computador (Pretas)\n");
            jogada_computador(usuariobranco, usuariopreto, arquivo);
            continue; // volta pro loop, espera o jogador branco
        }

        printf("Jogador atual: Brancas (B)\n");
        printf("Digite o movimento (ex: e2 e4), 'salvar' ou 'sair': ");

        if (!fgets(entrada, sizeof(entrada), stdin))
        {
            printf("Erro na leitura da entrada.\n");
            continue;
        }

        entrada[strcspn(entrada, "\n")] = '\0';

        if (strcmp(entrada, "sair") == 0)
        {
            break;
        }

        if (strcmp(entrada, "salvar") == 0)
        {
            salvar_jogo();
            continue;
        }

        if (sscanf(entrada, "%2s %2s", origem, destino) != 2)
        {
            printf("Formato invalido. Use: e2 e4\n");
            continue;
        }

        int li = TAMANHO_TABULEIRO - (origem[1] - '0');
        int ci = converter_letra_para_indice(origem[0]);
        int lf = TAMANHO_TABULEIRO - (destino[1] - '0');
        int cf = converter_letra_para_indice(destino[0]);

        if (li < 0 || li >= TAMANHO_TABULEIRO || ci < 0 || ci >= TAMANHO_TABULEIRO ||
            lf < 0 || lf >= TAMANHO_TABULEIRO || cf < 0 || cf >= TAMANHO_TABULEIRO)
        {
            printf("Coordenadas fora do tabuleiro.\n");
            continue;
        }

        system("cls");
        mover_peca(li, ci, lf, cf, usuariobranco, usuariopreto, arquivo);
    }
}
void salvar_jogo()
{
    EstadoJogo estado;
    FILE *f = fopen("xadrez_save.dat", "wb");
    if (f)
    {
        // Salva o estado do tabuleiro
        for (int i = 0; i < TAMANHO_TABULEIRO; i++)
        {
            for (int j = 0; j < TAMANHO_TABULEIRO; j++)
            {
                if (tabuleiro[i][j]->peca)
                {
                    estado.tabuleiro[i][j][0] = tabuleiro[i][j]->peca->sigla;
                    estado.tabuleiro[i][j][1] = tabuleiro[i][j]->peca->cor;
                }
                else
                {
                    estado.tabuleiro[i][j][0] = '\0';
                    estado.tabuleiro[i][j][1] = '\0';
                }
            }
        }
        estado.vez = vez;

        fwrite(&estado, sizeof(EstadoJogo), 1, f);
        fclose(f);
        printf("Jogo salvo com sucesso!\n");
    }
    else
    {
        printf("Erro ao salvar o jogo.\n");
    }
    printf("Pressione qualquer tecla para continuar...");
    getch();
}
int carregar_jogo()
{
    EstadoJogo estado;
    FILE *f = fopen("xadrez_save.dat", "rb");
    if (f)
    {
        fread(&estado, sizeof(EstadoJogo), 1, f);
        fclose(f);

        // Restaura o tabuleiro
        for (int i = 0; i < TAMANHO_TABULEIRO; i++)
        {
            for (int j = 0; j < TAMANHO_TABULEIRO; j++)
            {
                if (tabuleiro[i][j]->peca)
                {
                    free(tabuleiro[i][j]->peca);
                }

                if (estado.tabuleiro[i][j][0] != '\0')
                {
                    tabuleiro[i][j]->peca = criar_peca(estado.tabuleiro[i][j][0], estado.tabuleiro[i][j][1]);
                }
                else
                {
                    tabuleiro[i][j]->peca = NULL;
                }
            }
        }

        vez = estado.vez;
        printf("Jogo carregado com sucesso!\n");
        printf("Proximo jogador: %s (%c)\n",
               (vez == 'B') ? "BRANCAS" : "PRETAS", vez);
        printf("Pressione qualquer tecla para continuar...");
        getch();
        return 1;
    }
    printf("Nenhum jogo salvo encontrado.\n");
    printf("Pressione qualquer tecla para continuar...");
    getch();
    return 0;
}
void atualizarEstatisticas(char *usuario, char *resultado)
{
    FILE *arquivo = fopen("xadrez.txt", "r");
    if (!arquivo)
    {
        printf("Erro ao abrir arquivo de estatisticas.\n");
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (!temp)
    {
        printf("Erro ao criar arquivo temporario.\n");
        fclose(arquivo);
        return;
    }

    char linha[100];
    char nome[11], senha[6];
    int partidas, vitorias, empates, derrotas;
    bool encontrado = false;

    while (fgets(linha, sizeof(linha), arquivo))
    {
        if (strstr(linha, "Nome:") != NULL)
        {
            sscanf(linha, "Nome: %10s", nome);

            fprintf(temp, "%s", linha);

            fgets(linha, sizeof(linha), arquivo);
            sscanf(linha, "Senha: %5s", senha);
            fprintf(temp, "Senha: %s\n", senha);

            fgets(linha, sizeof(linha), arquivo);
            if (strcmp(nome, usuario) == 0)
            {
                encontrado = true;
                sscanf(linha, "%10[^,],%d,%d,%d,%d", nome, &partidas, &vitorias, &empates, &derrotas);

                partidas++;
                if (strcmp(resultado, "vitoria") == 0)
                    vitorias++;
                else if (strcmp(resultado, "empate") == 0)
                    empates++;
                else if (strcmp(resultado, "derrota") == 0)
                    derrotas++;

                fprintf(temp, "%s,%d,%d,%d,%d\n", nome, partidas, vitorias, empates, derrotas);
            }
            else
            {
                fprintf(temp, "%s", linha);
            }
        }
    }

    fclose(arquivo);
    fclose(temp);

    remove("xadrez.txt");
    rename("temp.txt", "xadrez.txt");

    if (!encontrado)
    {
        printf("Usuario %s nao encontrado para atualizacao de estatisticas.\n", usuario);
    }
}
void listarGanhadores(FILE *arquivo)
{
    system("cls");
    char nome[11], linha[100];
    int partidas, vitorias, empates, derrotas;

    arquivo = fopen("xadrez.txt", "r");
    if (!arquivo)
    {
        printf("Nenhuma estatistica encontrada.\n");
        return;
    }
    exibirLogo();
    printf("\nEstatisticas dos Jogadores\n");
    printf("--------------------------\n");

    while (fgets(linha, sizeof(linha), arquivo) != NULL)
    {
        if (sscanf(linha, "%10[^,],%d,%d,%d,%d", nome, &partidas, &vitorias, &empates, &derrotas) == 5)
        {
            printf("Nome: %-10s,Partidas: %d,Vitorias: %d,Empates: %d,Derrotas: %d\n",
                   nome, partidas, vitorias, empates, derrotas);
        }
        else if (strstr(linha, "Nome:") || strstr(linha, "Senha:"))
        {
            continue;
        }
    }
    fclose(arquivo);
    printf("\nPressione qualquer tecla para voltar...");
    getch();
    fflush(stdin);
    system("cls");
}
void realizarCadastro(FILE *arquivo)
{
    exibirLogo();
    system("cls");
    char nomeUser[11], senhaUser[6];
    char nomeExistente[11];
    bool existente = false;

    printf("Insira o Nome de Usuario (max 10 caracteres): ");
    scanf("%10s", nomeUser);
    fflush(stdin);

    printf("Insira uma Senha (max 5 caracteres): ");
    scanf("%5s", senhaUser);
    fflush(stdin);

    FILE *verifica = fopen("xadrez.txt", "r");
    if (verifica)
    {
        while (fscanf(verifica, "Nome: %10s\n", nomeExistente) == 1)
        {

            while (fgetc(verifica) != '\n' && !feof(verifica))
                ;
            while (fgetc(verifica) != '\n' && !feof(verifica))
                ;

            if (strcmp(nomeUser, nomeExistente) == 0)
            {
                existente = true;
                break;
            }
        }
        fclose(verifica);
    }

    if (existente)
    {
        printf("Usuario ja existe.\n");
        return;
    }

    arquivo = fopen("xadrez.txt", "a");
    if (!arquivo)
    {
        printf("Erro ao abrir arquivo para cadastro.\n");
        return;
    }

    fprintf(arquivo, "Nome: %s\nSenha: %s\n%s,0,0,0,0\n", nomeUser, senhaUser, nomeUser);
    fclose(arquivo);
    printf("Cadastro realizado com sucesso!\n");
}
bool realizarLogin(FILE *arquivo, char *user)
{
    system("cls");
    exibirLogo();
    char nomeUser[11], senhaUser[6];
    char nomeArquivo[11], senhaArquivo[6];
    int i = 0;
    char ch;

    printf("Insira o Nome de Usuario: ");
    scanf("%10s", nomeUser);
    fflush(stdin);

    printf("Insira a Senha: ");
    while (1)
    {
        ch = getch();
        if (ch == 13)
            break;
        else if (ch == TAMANHO_TABULEIRO && i > 0)
        {
            i--;
            printf("\b \b");
        }
        else if (ch >= 32 && ch <= 126 && i < 5)
        {
            senhaUser[i++] = ch;
            printf("*");
        }
    }
    senhaUser[i] = '\0';
    printf("\n");

    arquivo = fopen("xadrez.txt", "r");
    if (!arquivo)
    {
        printf("Erro ao abrir arquivo.\n");
        return false;
    }

    while (fscanf(arquivo, "Nome: %10s\nSenha: %5s\n", nomeArquivo, senhaArquivo) == 2)
    {
        while (fgetc(arquivo) != '\n' && !feof(arquivo))
            ;
        if (strcmp(nomeUser, nomeArquivo) == 0 && strcmp(senhaUser, senhaArquivo) == 0)
        {
            strcpy(user, nomeUser);
            fclose(arquivo);
            return true;
        }
    }

    fclose(arquivo);
    printf("Usuario ou senha incorretos.\n");
    return false;
}
void telaInicial(FILE *arquivo)
{
    char *usuario1 = malloc(11 * sizeof(char));
    char *usuario2 = malloc(11 * sizeof(char));
    char op;
    while (1)
    {
        exibirLogo();
        printf("1. Realizar Cadastro\n2. Realizar Login\n3. Jogar sem Login\n4. Sair\nOpcao: ");
        scanf(" %c", &op);
        fflush(stdin);
        switch (op)
        {
        case '1':
            realizarCadastro(arquivo);
            break;
        case '2':
            printf("Login do Jogador:\n");
            if (!realizarLogin(arquivo, usuario1))
            {
                printf("Login do Jogador falhou.\n");
                strcpy(usuario2, "Computador");
                break;
            }
            ExibirMenu(arquivo, usuario1, usuario2);
            break;
        case '3':
            strcpy(usuario1, "Branca");
            strcpy(usuario2, "Computador");
            ExibirMenu(arquivo, usuario1, usuario2);
            break;
        case '4':
            exit(0);
        default:
            printf("Opcao invalida.\n");
        }
    }
}
void exibirLogo()
{
    printf("\n\n");
    printf("    |  ___    ___    ___\n");
    printf("    |  |  |   |  _   |  |   \n");
    printf("  \\_|  |__|   \\__|   |__|   \n");
    printf("      ___     ____                 \n");
    printf("      |  \\   |     \n");
    printf("      |   \\  |----   \n");
    printf("      |___/  |____  \n\n");
    printf("   \\   /      _      __      ___      ____  _____\n");
    printf("    \\ /      / \\    |  \\    |   \\    |          /\n");
    printf("    / \\     /___\\   |   \\   |---/_   |----    /             \n");
    printf("   /   \\   /     \\  |___/   |     \\  |____  /____               \n\n\n");
}
