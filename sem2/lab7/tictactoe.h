#ifndef TICTACTOE_H
#define TICTACTOE_H

char board[3][3];
const char player = 'X';
const char player2 = 'O';
const char computer = 'O';

void resetBoard();
void printBoard();
int checkFreeSpaces();
void playerMove(bool);
void computerMove(bool showLogic);
char checkWinner();
void printWinner(char, bool);
void clearConsole();
pair<int, int> getValidInputMove(const string&);
int getValidInputMenu(const string&); 
char getValidInputForPlayAgain();

int minimax(bool maximizing, int alpha, int beta, int depth, bool);

void startGame(bool, bool);
void playAgain();

void menu();

#endif
