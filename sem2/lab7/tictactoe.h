#ifndef MINIMAX_H
#define MINIMAX_H

char board[3][3];
const char player = 'X';
const char player2 = 'O';
const char computer = 'O';

void resetBoard();
void printBoard();
int checkFreeSpaces();
void playerMove(bool);
void computerMove();
char checkWinner();
void printWinner(char, bool);

int minimax(bool maximizing);

void startGame(bool);
void playAgain();

void menu();

#endif
