#ifndef MINIMAX_H
#define MINIMAX_H

char board[3][3];
const char player = 'X';
const char computer = 'O';

void resetBoard();
void printBoard();
int checkFreeSpaces();
void playerMove();
void computerMove();
char checkWinner();
void printWinner(char);

int minimax(bool maximizing);

#endif // MINIMAX_H
