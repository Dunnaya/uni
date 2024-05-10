#ifndef MINIMAX_H
#define MINIMAX_H

char board[3][3];
const char player = 'X';
const char player2 = 'O';
const char computer = 'O';

void resetBoard();
void printBoard();
int checkFreeSpaces();
void playerMove();
void player2Move();
void computerMove();
char checkWinner();
void printWinner(char);
void printWinner2(char); 

int minimax(bool maximizing);

void startGame1();
void startGame2();
void playAgain();

void menu();

#endif
