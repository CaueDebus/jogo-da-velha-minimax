#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "conio.h"
#include <mmsystem.h> // PlaySound(); -> compiler linker settings -lwinmm
#include "game.h"
#include <math.h>

int minimax(gameStats * gameStatus,int depth, int isMax); // declaration of minimax function

void drawScreen(gameStats * gameStatus)
{
    // players names
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
    else if (gameStatus->gameOp == 2)
    {
        gotoxy(1, 2);
        textcolor(3);
        printf("Jogador 2 (O): ");
        textcolor(7);
        printf("BOT\n");
    }

    // game config
    gotoxy(1, 3);
    textcolor(5);
    printf("Melhor de: ");
    textcolor(7);
    printf("%d\n", gameStatus->gameType);

    // scoreboard
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

    // board values
    gotoxy(32, 4);
    textcolor(7);
    printf("%c", gameStatus->board[0][0]);
    gotoxy(52, 4);
    textcolor(7);
    printf("%c", gameStatus->board[0][1]);
    gotoxy(72, 4);
    textcolor(7);
    printf("%c", gameStatus->board[0][2]);
    gotoxy(32, 12);
    textcolor(7);
    printf("%c", gameStatus->board[1][0]);
    gotoxy(52, 12);
    textcolor(7);
    printf("%c", gameStatus->board[1][1]);
    gotoxy(72, 12);
    textcolor(7);
    printf("%c", gameStatus->board[1][2]);
    gotoxy(32, 20);
    textcolor(7);
    printf("%c", gameStatus->board[2][0]);
    gotoxy(52, 20);
    textcolor(7);
    printf("%c", gameStatus->board[2][1]);
    gotoxy(72, 20);
    textcolor(7);
    printf("%c", gameStatus->board[2][2]);

    // board horizontal lines
    for(int i = 25; i <= 80; i++)
    {
        gotoxy(i, 8);
        textcolor(7);
        printf("%c", 196);
        gotoxy(i, 16);
        textcolor(7);
        printf("%c", 196);
    }

    // board vertical lines
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

void stdCfgFile()
{
    FILE* entrada;

    entrada = fopen("./config/entrada.conf", "w");

    fprintf(entrada, "best-of: 3");
    fprintf(entrada, "\ngame-mode(1- 1x1 / 2- 1vBOT): 1");

    fclose(entrada);
}

void getGameProperties(gameStats* gameStatus)
{
    FILE* entrada;

    entrada = fopen("./config/entrada.conf", "r");

    if(entrada == NULL)
    {
        stdCfgFile(); // if there isnt .conf file it will create a std one
    }

    fscanf(entrada, "best-of: %d", &gameStatus->gameType);

    if (gameStatus->gameType > 7) // max gameType == 7
    {
        gameStatus->gameType = 3;
    }

    fscanf(entrada, "\ngame-mode(1- 1x1 / 2- 1vBOT): %d", &gameStatus->gameOp);

    if (gameStatus->gameOp != 1 && gameStatus->gameOp != 2) // in case gameOp == inst 1 or 2 set gameOp to 1
    {
        gameStatus->gameOp = 1;
    }

    fclose(entrada);
}

void drawGameId()
{
    FILE* saida;

    saida = fopen("./gamehistory/saida.gamehistory", "a"); // using append to save other games history at the same file

    int idNum = rand() % 1000000000;

    fprintf(saida, "\nid do jogo: #%d\n", idNum);

    fclose(saida);
}

void drawGameHistory(gameStats* gameStatus)
{
    FILE* saida;

    saida = fopen("./gamehistory/saida.gamehistory", "a"); // using append to save other games history at the same file

    fprintf(saida, "\n     Jogo de numero: %d\n", gameStatus->currentGame + 1);
    fprintf(saida, "      %c | %c | %c\n      ----------\n      %c | %c | %c\n      ----------\n      %c | %c | %c\n", gameStatus->board[0][0], gameStatus->board[0][1], gameStatus->board[0][2], gameStatus->board[1][0], gameStatus->board[1][1], gameStatus->board[1][2], gameStatus->board[2][0], gameStatus->board[2][1], gameStatus->board[2][2]);

    fclose(saida);
}

void getPlayersName(gameStats* gameStatus)
{
    printf("Digite o nome do jogador 1: ");
    scanf("%s", gameStatus->ply1);
    if (gameStatus->gameOp == 1) // if the gameOp == 2 the second player will be BOT, so no second name needed
    {
        printf("Digite o nome do jogador 2: ");
        scanf("%s", gameStatus->ply2);
    }
}

void gameInit(gameStats * gameStatus) // initialize all game vars
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
            gameStatus->board[i][j] = 'A';
        }
    }
}

int boardFull(gameStats * gameStatus) // check if there aren any empty spaces in the board
{
    int i, j;
    for(i = 0; i < 3; i++)
        for(j = 0; j < 3; j++)
            if(gameStatus->board[i][j] == ' ')
                return 0;

    return 1;
}

int verifyWinner(gameStats * gameStatus)
{
    // checking horizontally X
    if(gameStatus->board[0][0]=='X' && gameStatus->board[0][1]=='X' && gameStatus->board[0][2]=='X'){ return -10; }
    if(gameStatus->board[1][0]=='X' && gameStatus->board[1][1]=='X' && gameStatus->board[1][2]=='X'){ return -10; }
    if(gameStatus->board[2][0]=='X' && gameStatus->board[2][1]=='X' && gameStatus->board[2][2]=='X'){ return -10; }
    // checking vertically X
    if(gameStatus->board[0][0]=='X' && gameStatus->board[1][0]=='X' && gameStatus->board[2][0]=='X'){ return -10; }
    if(gameStatus->board[0][1]=='X' && gameStatus->board[1][1]=='X' && gameStatus->board[2][1]=='X'){ return -10; }
    if(gameStatus->board[0][2]=='X' && gameStatus->board[1][2]=='X' && gameStatus->board[2][2]=='X'){ return -10; }
    // checking diagonally X
    if(gameStatus->board[0][0]=='X' && gameStatus->board[1][1]=='X' && gameStatus->board[2][2]=='X'){ return -10; }
    if(gameStatus->board[0][2]=='X' && gameStatus->board[1][1]=='X' && gameStatus->board[2][0]=='X'){ return -10; }

    // checking horizontally O
    if(gameStatus->board[0][0]=='O' && gameStatus->board[0][1]=='O' && gameStatus->board[0][2]=='O'){ return 10; }
    if(gameStatus->board[1][0]=='O' && gameStatus->board[1][1]=='O' && gameStatus->board[1][2]=='O'){ return 10; }
    if(gameStatus->board[2][0]=='O' && gameStatus->board[2][1]=='O' && gameStatus->board[2][2]=='O'){ return 10; }
    // checking vertically O
    if(gameStatus->board[0][0]=='O' && gameStatus->board[1][0]=='O' && gameStatus->board[2][0]=='O'){ return 10; }
    if(gameStatus->board[0][1]=='O' && gameStatus->board[1][1]=='O' && gameStatus->board[2][1]=='O'){ return 10; }
    if(gameStatus->board[0][2]=='O' && gameStatus->board[1][2]=='O' && gameStatus->board[2][2]=='O'){ return 10; }
    // checking diagonally O
    if(gameStatus->board[0][0]=='O' && gameStatus->board[1][1]=='O' && gameStatus->board[2][2]=='O'){ return 10; }
    if(gameStatus->board[0][2]=='O' && gameStatus->board[1][1]=='O' && gameStatus->board[2][0]=='O'){ return 10; }

    if (gameStatus->playsCounter == 9 || boardFull(gameStatus)); // check for tie
    {
        return 0;
    }
    else
    {
        return 0; // game still ongoing
    }
}

void clearVariables(gameStats * gameStatus) // clear all vars values
{
    gameStatus->plyr = 0;
    gameStatus->playsCounter = 0;
    gameStatus->rows = 0;
    gameStatus->cols = 0;
    for(int i = 0;i <3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            gameStatus->board[i][j] = ' ';
        }
    }
}

void drawWinner(gameStats * gameStatus) // check the "best of X" winner
{
    if(gameStatus->ply1Won > gameStatus->gameType % 2 || gameStatus->ply1Won > gameStatus->ply2Won)
    {
        clrscr();
        gotoxy(45, 15);
        textcolor(12);
        printf("JOGADOR %s FOI O VENCEDOR", gameStatus->ply1);
        PlaySound(TEXT("./sounds/wow.wav"), NULL, SND_ASYNC);
    }
    else if(gameStatus->ply2Won > gameStatus->gameType % 2 || gameStatus->ply2Won > gameStatus->ply1Won)
    {
        if (gameStatus->gameOp == 2)
        {
            clrscr();
            gotoxy(45, 15);
            textcolor(12);
            printf("O BOT FOI O VENCEDOR");
            PlaySound(TEXT("./sounds/wow.wav"), NULL, SND_ASYNC);
        }
        else
        {
            clrscr();
            gotoxy(45, 15);
            textcolor(12);
            printf("JOGADOR %s FOI O VENCEDOR", gameStatus->ply2);
            PlaySound(TEXT("./sounds/wow.wav"), NULL, SND_ASYNC);
        }
    }
    else
    {
        clrscr();
        gotoxy(45, 15);
        textcolor(12);
        printf("O JOGO EMPATOU");
        PlaySound(TEXT("./sounds/empate.wav"), NULL, SND_ASYNC);
    }
}

void drawEndGame()
{
    const char* legenda[] = { "Muito ", "obrigado ", "por ", "jogar ", "jogo ", "da ", "velha ", "feito ", "por ", "Caue ", "Felipe ", "Knies ", "Debus ", "e ", "Eduardo ", "Henrique ", "Joner ", "Lemos" };

    clrscr();
    PlaySound(TEXT("./sounds/fim.wav"), NULL, SND_ASYNC);
    gotoxy(5, 5);
    textcolor(7);
    for (int i = 0; i < 18; i++)
    {
        printf("%s", legenda[i]);
        Sleep(450);
    }
}

// https://www.youtube.com/watch?v=cwzKjFkSyIE

void bot(gameStats * gameStatus)
{
    int i, j, pt, bpt = -10000, index_x = 0, index_y = 0;

    // traverse all cells
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            // check if cell is empty
            if (gameStatus->board[i][j] == ' ')
            {
                // make a move
                gameStatus->board[i][j] = 'O';
                // call minimax
                pt = minimax(gameStatus, 0, 0);
                // undo the move
                gameStatus->board[i][j] = ' ';
                if (pt > bpt) // choose max value
                {
                    bpt = pt;
                    index_x = i; // save the best move row
                    index_y = j; // save the best move col
                }
            }
        }
    }
    gameStatus->board[index_x][index_y] = 'O';
}

int minimax(gameStats * gameStatus, int depth, int isMax)
{
    int score, i, j, pt;
    score = verifyWinner(gameStatus);
    if(score || gameStatus->playsCounter == 9 || boardFull(gameStatus)) // check if score != 0 or playsCounter reached it's max value -> boad is full or if board is full
    {
        if (score == 10)
            return score - depth; // maximizer won
        else if (score == -10)
            return score + depth; // minimizer won
        else
            return score; // tie
    }
    if (isMax) // max
    {
        int bpt = -10000;
        // traverse all cells
        for (i = 0; i < 3; i++)
        {
            for (j = 0; j < 3; j++)
            {
                // check if cell is empty
                if (gameStatus->board[i][j] == ' ')
                {
                    // make a move
                    gameStatus->board[i][j] = 'O';
                    // call minimax recursively
                    pt = minimax(gameStatus, depth + 1, 0);
                    // undo the move
                    gameStatus->board[i][j] = ' ';
                    if (pt > bpt) // choose max value
                    {
                        bpt = pt;
                    }
                }
            }
        }
        return bpt;
    }
    else // min
    {
        int bpt = 10000;
        // traverse all cells
        for (i = 0; i < 3; i++)
        {
            for (j = 0; j < 3; j++)
            {
                // check if cell is empty
                if (gameStatus->board[i][j] == ' ')
                {
                    // make a move
                    gameStatus->board[i][j] = 'X';
                    // call minimax recursively
                    pt = minimax(gameStatus, depth + 1, 1);
                    // undo the move
                    gameStatus->board[i][j] = ' ';
                    if (pt < bpt) // choose min value
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

    drawGameId();
    gameInit(&gameStatus); // initialize game
    getGameProperties(&gameStatus); // get game config
    getPlayersName(&gameStatus); // get players names

    for(int i = 0; i < gameStatus.gameType; i++) // continue process until all matches from gameType were played
    {
        gameStatus.currentGame = i;
        gameStatus.endGame = 0;
        clearVariables(&gameStatus); // after a game end clear all variables
        while(gameStatus.endGame == 0) // while game sill ongoing
        {
            // clrscr() and drawScreen() are basically refreshing the screen after a move
            clrscr();                  
            drawScreen(&gameStatus); 

            if(gameStatus.playsCounter % 2 == 0) // if plays counter is even it means that it is player's 1 turn
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

            if (gameStatus.gameOp == 2 && gameStatus.plyr == 2) // in case gameOp == 2 get BOT's move
            {
                bot(&gameStatus);
                gameStatus.playsCounter++;
            }
            else if (gameStatus.gameOp == 2 && gameStatus.plyr == 1) // in case gameOp == 2 get player's move
            {
                gotoxy(1, 41);
                printf("Digite a linha: ");
                scanf("%d", &gameStatus.rows);

                gotoxy(1, 42);
                printf("Digite a coluna: ");
                scanf("%d", &gameStatus.cols);
            }

            if (gameStatus.gameOp == 1) // in case gameOp == 1 get player's move
            {
                gotoxy(1, 41);
                printf("Digite a linha: ");
                scanf("%d", &gameStatus.rows);

                gotoxy(1, 42);
                printf("Digite a coluna: ");
                scanf("%d", &gameStatus.cols);
            }

            // check if row and col values are valid
            if(gameStatus.rows < 1 || gameStatus.cols < 1 || gameStatus.rows > 3 || gameStatus.cols > 3) // cells that doesnt exist
            {
                gameStatus.rows = 0;
                gameStatus.cols = 0;
            }
            else if(gameStatus.board[gameStatus.rows -1][gameStatus.cols -1] != ' ') // not empty cells
            {
                gameStatus.rows = 0;
                gameStatus.cols = 0;
            }
            else // valid cells
            {
                if(gameStatus.plyr == 1) // if is player 1 draw X
                {
                    gameStatus.board[gameStatus.rows -1][gameStatus.cols -1] = 'X';
                }
                else // if is player 2 draw X
                {
                    if (gameStatus.gameOp != 2) // in case gameOp == 2 bot() function already makes it own move
                    {
                        gameStatus.board[gameStatus.rows - 1][gameStatus.cols - 1] = 'O';
                    }
                }
                gameStatus.playsCounter++; // add + 1 to playsCounter
            }

            // check for winner
            int check = verifyWinner(&gameStatus);
            if (check == 10) // O won
            {
                gameStatus.ply2Won++;
                gameStatus.endGame = 1;
                drawGameHistory(&gameStatus);
            }
            else if (check == -10) // X won
            {
                gameStatus.ply1Won++;
                gameStatus.endGame = 1;
                drawGameHistory(&gameStatus);
            }
            else if (boardFull(&gameStatus)) // tie
            {
                gameStatus.endGame = 1;
                drawGameHistory(&gameStatus);
            }
        }
    }
    drawWinner(&gameStatus); // draw "best of" winner
    getch(); // wait for a key pressed to continue
    drawEndGame();
    getch(); // wait for a key pressed to continue

    return 0;
}
