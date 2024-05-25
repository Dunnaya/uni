#ifndef TICTACTOE_H
#define TICTACTOE_H

char board[3][3];
const char player = 'X';
const char player2 = 'O';
const char computer = 'O';

struct PointInfo 
{
    int x;
    int y;
    int score;
    int alpha;
    int beta;
    int depth;
    vector<PointInfo> children;
};

vector<PointInfo> pointLogs;

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

int minimax(bool maximizing, int alpha, int beta, int depth, bool, PointInfo* parent);

void displayPointInfo(const PointInfo&, int indent); 
void showLogicMenu();

void startGame(bool, bool);
void playAgain();

void menu();

#endif
