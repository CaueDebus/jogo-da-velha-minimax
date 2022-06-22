#include <stdlib.h>

#define MAX_NAME_LEN 128 // de preferencia numeros magicos em potencia de 2

#define SQUARES_NUM 3

typedef struct game{
    unsigned int gameType;
    unsigned int ply1Won;
    unsigned int ply2Won;
    char ply1[MAX_NAME_LEN];
    char ply2[MAX_NAME_LEN];
    int endGame;
    unsigned int plyr;
    unsigned int playsCounter;
    unsigned int rows;
    unsigned int cols;
    unsigned int gameOp;
    unsigned int currentGame;
    char board[SQUARES_NUM][SQUARES_NUM];
} gameStats;
