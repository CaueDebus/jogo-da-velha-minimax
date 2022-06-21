#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h> // para usar o Sleep();
#include "conio.h" // funções gráficas
#include <mmsystem.h> // PlaySound(); -> usar nas configs do compilador -lwinmm
#include "game.h"
#include <math.h>

int minimax(gameStats * gameStatus, int isMax);

void drawScreen(gameStats * gameStatus)
{
    // nome dos jogadores
    gotoxy(1, 1);
    textcolor(1);
    printf("Jogador 1 (X): ");
    textcolor(7);
    printf("%s\n", gameStatus->ply1);

    if (gameStatus->gameOp == 1)
    {
        gotoxy(1, 2);
        textcolor(3);
        printf("Jogador 2 (O): ");
        textcolor(7);
        printf("%s\n", gameStatus->ply2);
    }


    // tipo do jogo
    gotoxy(1, 3);
    textcolor(5);
    printf("Melhor de: ");
    textcolor(7);
    printf("%d\n", gameStatus->gameType);

    // placar
    gotoxy(1, 4);
    textcolor(6);
    printf("Jogador 1 venceu: ");
    textcolor(7);
    printf("%d\n", gameStatus->ply1Won);

    gotoxy(1, 5);
    textcolor(10);
    printf("Jogador 2 venceu: ");
    textcolor(7);
    printf("%d\n", gameStatus->ply2Won);

    // jogo da velha
    gotoxy(32, 4);
    textcolor(7);
    printf("%c", gameStatus->squares[0][0]);

    gotoxy(52, 4);
    textcolor(7);
    printf("%c", gameStatus->squares[0][1]);

    gotoxy(72, 4);
    textcolor(7);
    printf("%c", gameStatus->squares[0][2]);

    gotoxy(32, 12);
    textcolor(7);
    printf("%c", gameStatus->squares[1][0]);

    gotoxy(52, 12);
    textcolor(7);
    printf("%c", gameStatus->squares[1][1]);

    gotoxy(72, 12);
    textcolor(7);
    printf("%c", gameStatus->squares[1][2]);

    gotoxy(32, 20);
    textcolor(7);
    printf("%c", gameStatus->squares[2][0]);

    gotoxy(52, 20);
    textcolor(7);
    printf("%c", gameStatus->squares[2][1]);

    gotoxy(72, 20);
    textcolor(7);
    printf("%c", gameStatus->squares[2][2]);

    // linhas horizontais
    for(int i = 25; i <= 80; i++)
    {
        gotoxy(i, 8);
        textcolor(7);
        printf("%c", 196);
        gotoxy(i, 16);
        textcolor(7);
        printf("%c", 196);
    }

    // linhas verticais
    for(int i = 30; i <= 50; i++)
    {
        gotoxy(42, i-28);
        textcolor(7);
        printf("%c\n", 179);
        gotoxy(62, i-28);
        textcolor(7);
        printf("%c\n", 179);
    }
}

void getGameProperties(gameStats* gameStatus)
{
    FILE* entrada;

    entrada = fopen("entrada.conf", "r");

    // se arquivo entrada estiver vazio assumir gameType md3
    if(entrada == NULL)
    {
        gameStatus->gameType = 3;
        gameStatus->gameOp = 1; // 2v2
    }

    fscanf(entrada, "best-of: %d", &gameStatus->gameType);
    fscanf(entrada, "\ngame-mode(1- 2x2 / 2- 1vBOT): %d", &gameStatus->gameOp);

    fclose(entrada);
}

void drawGameHistoric(gameStats* gameStatus)
{
    FILE* saida;

    saida = fopen("saida.gamehistoric", "a");

    fprintf(saida, "\nJogo de numero: %d\n", gameStatus->currentGame + 1);
    fprintf(saida, "%c | %c | %c\n", gameStatus->squares[0][0], gameStatus->squares[0][1], gameStatus->squares[0][2]);
    fprintf(saida, "----------\n");
    fprintf(saida, "%c | %c | %c\n", gameStatus->squares[1][0], gameStatus->squares[1][1], gameStatus->squares[1][2]);
    fprintf(saida, "----------\n");
    fprintf(saida, "%c | %c | %c\n", gameStatus->squares[2][0], gameStatus->squares[2][1], gameStatus->squares[2][2]);

    fclose(saida);
}

void getPlayersName(gameStats* gameStatus)
{
    printf("Digite o nome do jogador 1: ");
    scanf("%s", gameStatus->ply1);
    if (gameStatus->gameOp == 1)
    {
        printf("Digite o nome do jogador 2: ");
        scanf("%s", gameStatus->ply2);
    }
}

void gameInit(gameStats * gameStatus)
{
    gameStatus->ply1Won = 0;
    gameStatus->ply2Won = 0;
    gameStatus->plyr = 0;
    gameStatus->playsCounter = 0;
    gameStatus->rows = 0;
    gameStatus->cols = 0;
    for(int i = 0;i <3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            gameStatus->squares[i][j] = 'A';
        }
    }
}

int verifyWinner(gameStats * gameStatus)
{
    /*ganha por linha*/
    if(gameStatus->squares[0][0]=='X' && gameStatus->squares[0][1]=='X' && gameStatus->squares[0][2]=='X'){ return -10; }
    if(gameStatus->squares[1][0]=='X' && gameStatus->squares[1][1]=='X' && gameStatus->squares[1][2]=='X'){ return -10; }
    if(gameStatus->squares[2][0]=='X' && gameStatus->squares[2][1]=='X' && gameStatus->squares[2][2]=='X'){ return -10; }
    /*ganha por coluna*/
    if(gameStatus->squares[0][0]=='X' && gameStatus->squares[1][0]=='X' && gameStatus->squares[2][0]=='X'){ return -10; }
    if(gameStatus->squares[0][1]=='X' && gameStatus->squares[1][1]=='X' && gameStatus->squares[2][1]=='X'){ return -10; }
    if(gameStatus->squares[0][2]=='X' && gameStatus->squares[1][2]=='X' && gameStatus->squares[2][2]=='X'){ return -10; }
    /*ganha por diagonal*/
    if(gameStatus->squares[0][0]=='X' && gameStatus->squares[1][1]=='X' && gameStatus->squares[2][2]=='X'){ return -10; }
    if(gameStatus->squares[0][2]=='X' && gameStatus->squares[1][1]=='X' && gameStatus->squares[2][0]=='X'){ return -10; }

    /*ganha por linha*/
    if(gameStatus->squares[0][0]=='O' && gameStatus->squares[0][1]=='O' && gameStatus->squares[0][2]=='O'){ return 10; }
    if(gameStatus->squares[1][0]=='O' && gameStatus->squares[1][1]=='O' && gameStatus->squares[1][2]=='O'){ return 10; }
    if(gameStatus->squares[2][0]=='O' && gameStatus->squares[2][1]=='O' && gameStatus->squares[2][2]=='O'){ return 10; }
    /*ganha por coluna*/
    if(gameStatus->squares[0][0]=='O' && gameStatus->squares[1][0]=='O' && gameStatus->squares[2][0]=='O'){ return 10; }
    if(gameStatus->squares[0][1]=='O' && gameStatus->squares[1][1]=='O' && gameStatus->squares[2][1]=='O'){ return 10; }
    if(gameStatus->squares[0][2]=='O' && gameStatus->squares[1][2]=='O' && gameStatus->squares[2][2]=='O'){ return 10; }
    /*ganha por diagonal*/
    if(gameStatus->squares[0][0]=='O' && gameStatus->squares[1][1]=='O' && gameStatus->squares[2][2]=='O'){ return 10; }
    if(gameStatus->squares[0][2]=='O' && gameStatus->squares[1][1]=='O' && gameStatus->squares[2][0]=='O'){ return 10; }

    if (gameStatus->playsCounter == 9)
    {
        return -5; // estado de retorno velha
    }
    else
    {
        return 0; // estado de retorno jogo ainda em andamento
    }
}

void clearVariables(gameStats * gameStatus)
{
    gameStatus->plyr = 0;
    gameStatus->playsCounter = 0;
    gameStatus->rows = 0;
    gameStatus->cols = 0;
    for(int i = 0;i <3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            gameStatus->squares[i][j] = ' ';
        }
    }
}

void drawWinner(gameStats * gameStatus)
{
    if(gameStatus->ply1Won > gameStatus->gameType % 2 || gameStatus->ply1Won > gameStatus->ply2Won)
    {
        clrscr();
        gotoxy(45, 15);
        textcolor(12);
        printf("JOGADOR %s FOI O VENCEDOR", gameStatus->ply1);
        PlaySound(TEXT("wow.wav"), NULL, SND_ASYNC);
    }
    else if(gameStatus->ply2Won > gameStatus->gameType % 2 || gameStatus->ply2Won > gameStatus->ply1Won)
    {
        if (gameStatus->gameOp == 2)
        {
            clrscr();
            gotoxy(45, 15);
            textcolor(12);
            printf("O BOT FOI O VENCEDOR");
            PlaySound(TEXT("wow.wav"), NULL, SND_ASYNC);
        }
        else
        {
            clrscr();
            gotoxy(45, 15);
            textcolor(12);
            printf("JOGADOR %s FOI O VENCEDOR", gameStatus->ply2);
            PlaySound(TEXT("wow.wav"), NULL, SND_ASYNC);
        }
    }
    else
    {
        clrscr();
        gotoxy(45, 15);
        textcolor(12);
        printf("O JOGO EMPATOU");
        PlaySound(TEXT("empate.wav"), NULL, SND_ASYNC);
    }
}

void drawEndGame()
{
    const char* legenda[] = { "Muito ", "obrigado ", "por ", "jogar ", "jogo ", "da ", "velha ", "feito ", "por ", "Caue ", "Felipe ", "Knies ", "Debus ", "e ", "Eduardo ", "Henrique ", "Joner ", "Lemos" };

    clrscr();
    PlaySound(TEXT("fim.wav"), NULL, SND_ASYNC);
    gotoxy(5, 5);
    textcolor(7);
    for (int i = 0; i < 18; i++)
    {
        printf("%s", legenda[i]);
        Sleep(450);
    }
}

// https://www.youtube.com/watch?v=cwzKjFkSyIE
// o processo de todas as formas onde O ganha retorna 1 e onde X ganha retorna -1
// no caso de velha minimax retorna -5
// jogo ainda em andamento a função minimax continua a recursao

// o bot prefere nao perder doque ganhar
// ele funciona apenas se a primeira casa marcada e na linha 3 e coluna 3

void bot(gameStats * gameStatus)
{
    int i, j, index_x, index_y;
    double pt;
    double bpt = -INFINITY;
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            if (gameStatus->squares[i][j] == ' ')
            {
                gameStatus->squares[i][j] = 'O';
                pt = minimax(gameStatus, 0);
                gameStatus->squares[i][j] = ' ';
                if (pt > bpt)
                {
                    bpt = pt;
                    index_x = i;
                    index_y = j;
                }
            }
        }
    }
    gameStatus->squares[index_x][index_y] = 'O';
}

int minimax(gameStats * gameStatus, int isMax)
{
    int score, i, j;
    double pt;
    score = verifyWinner(gameStatus);
    if (score != 0)
    {
        return score;
    }
    if (isMax) // maximo
    {
        double bpt = -INFINITY;
        for (i = 0; i < 3; i++)
        {
            for (j = 0; j < 3; j++)
            {
                if (gameStatus->squares[i][j] == ' ')
                {
                    gameStatus->squares[i][j] = 'O';
                    pt = minimax(gameStatus, 0);
                    gameStatus->squares[i][j] = ' ';
                    if (pt > bpt)
                    {
                        bpt = pt;
                    }
                }
            }
        }
        return bpt;
    }
    else // minimo
    {
        double bpt = INFINITY;
        for (i = 0; i < 3; i++)
        {
            for (j = 0; j < 3; j++)
            {
                if (gameStatus->squares[i][j] == ' ')
                {
                    gameStatus->squares[i][j] = 'X';
                    pt = minimax(gameStatus, 1);
                    gameStatus->squares[i][j] = ' ';
                    if (pt < bpt)
                    {
                        bpt = pt;
                    }
                }
            }
        }
        return bpt;
    }
}

int main()
{
    gameStats gameStatus;

    gameInit(&gameStatus);
    getGameProperties(&gameStatus);
    getPlayersName(&gameStatus);

    for(int i = 0; i < gameStatus.gameType; i++)
    {
        gameStatus.currentGame = i;
        gameStatus.haveWinner = 0;
        clearVariables(&gameStatus);
        while(gameStatus.haveWinner == 0)
        {
            clrscr();
            drawScreen(&gameStatus);

            if(gameStatus.playsCounter % 2 == 0) // jogadas pares = plyr 1
            {
                gameStatus.plyr = 1;
            }
            else
            {
                gameStatus.plyr = 2;
            }

            gotoxy(1, 40);
            if(gameStatus.plyr == 1)
            {
                printf("Vez de jogador %s\n", gameStatus.ply1);
            }
            else
            {
                if (gameStatus.gameOp == 2)
                {
                    printf("Vez de jogador BOT\n");
                }
                else
                {
                    printf("Vez de jogador %s\n", gameStatus.ply2);
                }
            }

            if (gameStatus.gameOp == 2 && gameStatus.plyr == 2)
            {
                bot(&gameStatus);
                gameStatus.playsCounter++;
            }
            else if (gameStatus.gameOp == 2 && gameStatus.plyr == 1)
            {
                gotoxy(1, 41);
                printf("Digite a linha: ");
                scanf("%d", &gameStatus.rows);

                gotoxy(1, 42);
                printf("Digite a coluna: ");
                scanf("%d", &gameStatus.cols);
            }

            if (gameStatus.gameOp == 1)
            {
                gotoxy(1, 41);
                printf("Digite a linha: ");
                scanf("%d", &gameStatus.rows);

                gotoxy(1, 42);
                printf("Digite a coluna: ");
                scanf("%d", &gameStatus.cols);
            }

            // verifica se valores são validos
            if(gameStatus.rows < 1 || gameStatus.cols < 1 || gameStatus.rows > 3 || gameStatus.cols > 3) // casas que nao existem
            {
                gameStatus.rows = 0;
                gameStatus.cols = 0;
            }
            else if(gameStatus.squares[gameStatus.rows -1][gameStatus.cols -1] != ' ') // casas ja ocupadas
            {
                gameStatus.rows = 0;
                gameStatus.cols = 0;
            }
            else // casas válidas
            {
                if(gameStatus.plyr == 1)
                {
                    gameStatus.squares[gameStatus.rows -1][gameStatus.cols -1] = 'X';
                }
                else
                {
                    if (gameStatus.gameOp != 2)
                    {
                        gameStatus.squares[gameStatus.rows - 1][gameStatus.cols - 1] = 'O';
                    }
                }
                gameStatus.playsCounter++;
            }

            // verifica se alguem ganhou
            int check = verifyWinner(&gameStatus);
            if (check == 10) // O ganha
            {
                gameStatus.ply2Won++;
                gameStatus.haveWinner = 1;
                drawGameHistoric(&gameStatus);
            }
            else if (check == -10) // X ganha
            {
                gameStatus.ply1Won++;
                gameStatus.haveWinner = 1;
                drawGameHistoric(&gameStatus);
            }
            else if (check == -5) // velha
            {
                gameStatus.haveWinner = 1;
                drawGameHistoric(&gameStatus);
            }
        }
    }
    drawWinner(&gameStatus);
    getch();
    drawEndGame();
    getch();

    return 0;
}
