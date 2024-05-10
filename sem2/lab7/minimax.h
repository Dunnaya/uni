#ifndef MINIMAX_H
#define MINIMAX_H

char board[3][3];
const char player = 'X';
const char computer = 'O';

const int INF = INT_MAX/2;

void resetBoard();
void printBoard();
int checkFreeSpaces();
void playerMove();
void computerMove();
char checkWinner();
void printWinner(char);

struct Move 
{
    int row;
    int col;
};

void bestMove();
int minimax(char board[3][3], bool isMaximizing, int depth);

#endif // MINIMAX_H
