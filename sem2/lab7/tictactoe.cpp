// tic-tac-toe game
// minimax algorithm + alpha-beta pruning
#include <iostream>
#include <cstdlib>
#include <limits>
using namespace std;
#include "tictactoe.h"

int main() 
{
   menu();
   return 0;
}

void startGame(bool isComputer = true, bool showLogic = false)
{
    char winner = ' ';

    resetBoard();

    while (winner == ' ' && checkFreeSpaces() != 0)
    {
        if(!showLogic)
            clearConsole();
        cout << "\tTIC TAC TOE\n";
        printBoard();

        playerMove(true);
        winner = checkWinner();
        if(winner != ' ' || checkFreeSpaces() == 0)
        {
            break;
        }

        if(!showLogic)
        {
            clearConsole();
            cout << "\tTIC TAC TOE\n";
            printBoard();
        }

        if(isComputer) computerMove(showLogic);
        else playerMove(false);

        winner = checkWinner();
        if(winner != ' ' || checkFreeSpaces() == 0)
        {
            break;
        }
    }

    if(!showLogic)
        clearConsole();
    cout << "\tTIC TAC TOE\n";
    printBoard();
    if(isComputer) printWinner(winner, true);
    else printWinner(winner, false);
}

void resetBoard() 
{
    for(int i = 0; i < 3; i++) 
    {
        for(int j = 0; j < 3; j++) 
        {
            board[i][j] = ' ';
        }
    }
}

void printBoard() 
{
    cout << "\t " << board[0][0] << " | " << board[0][1] << " | " << board[0][2] << " \n";
    cout << "\t---|---|---\n";
    cout << "\t " << board[1][0] << " | " << board[1][1] << " | " << board[1][2] << " \n";
    cout << "\t---|---|---\n";
    cout << "\t " << board[2][0] << " | " << board[2][1] << " | " << board[2][2] << " \n";
}


int checkFreeSpaces() 
{
    int freeSpaces = 9;

    for(int i = 0; i < 3; i++) 
    {
        for(int j = 0; j < 3; j++) 
        {
            if (board[i][j] != ' ')
            {
                freeSpaces--;
            }
        }
    }
    return freeSpaces;
}

void playerMove(bool is1Player = true) 
{
    pair<int, int> move;

    if (is1Player)
        cout << "\n Player1 turn\n";
    else 
        cout << "\n Player2 turn:\n";

    do
    {
        move = getValidInputMove("Enter row and column #(1-3) separated by space: ");
        int x = move.first - 1;
        int y = move.second - 1;

        if(board[x][y] != ' ')
        {
            cout << "\n\tInvalid move! The cell is already occupied.\n\n";
        }
        else 
        {
            if (is1Player) board[x][y] = player;
            else board[x][y] = player2;
            break;
        }
    } while (board[move.first-1][move.second-1] != ' ');
}

pair<int, int> getValidInputMove(const string& prompt) 
{
    pair<int, int> values;
    bool validInput = false;
    int x, y;

    while (!validInput) 
    {
        cout << prompt;
        cin >> x >> y;

        if (cin.fail() || x < 1 || x > 3 || y < 1 || y > 3) 
        {
            cerr << "\n\tInvalid input. Please enter two numbers between 1 and 3 separated by a space.\n\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } 
        else 
        {
            validInput = true;
            values = make_pair(x, y);
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    return values;
}

void computerMove(bool showLogic = false) 
{
    int bestVal = INT_MIN;
    int bestMoveX = -1;
    int bestMoveY = -1;

    for(int i = 0; i < 3; i++) 
    {
        for(int j = 0; j < 3; j++) 
        {
            if (board[i][j] == ' ') 
            {
                board[i][j] = computer;
                int moveVal = minimax(false, INT_MIN, INT_MAX, 0, showLogic);
                board[i][j] = ' ';

                if (moveVal > bestVal) 
                {
                    bestVal = moveVal;
                    bestMoveX = i;
                    bestMoveY = j;
                }
            }
        }
    }

    board[bestMoveX][bestMoveY] = computer;
}

char checkWinner() 
{
    for(int i = 0; i < 3; i++)
    {
        if(board[i][0] == board[i][1] && board[i][0] == board[i][2] && board[i][0] != ' ')
        {
            return board[i][0];
        }
    }

    for(int i = 0; i < 3; i++)
    {
        if(board[0][i] == board[1][i] && board[0][i] == board[2][i] && board[0][i] != ' ')
        {
            return board[0][i];
        }
    }

    if(board[0][0] == board[1][1] && board[0][0] == board[2][2] && board[0][0] != ' ')
    {
        return board[0][0];
    }

    if(board[0][2] == board[1][1] && board[0][2] == board[2][0] && board[0][2] != ' ')
    {
        return board[0][2];
    }

    return ' ';
}

void printWinner(char winner, bool is1Player = true) 
{
    if(is1Player)
    {
        if(winner == player)
        {
        cout << "\n\tYou win!";
        }
        else if(winner == computer)
        {
        cout << "\n\tYou lose!";
        }
        else
        {
        cout << "\n\tIt's a tie!";
        }
    }
    else 
    {
        if(winner == player)
        {
        cout << "\n      Player #1 wins!";
        }
        else if(winner == player2)
        {
        cout << "\n      Player #2 wins!";
        }
        else
        {
        cout << "\n\tIt's a tie!";
        }
    }
}

int minimax(bool maximizing, int alpha, int beta, int depth = 0, bool showLogic = false) 
{
    int score = 0;
    char win = checkWinner();
    if (win == computer)
        return 10 - depth;
    if (win == player)
        return depth - 10;
    if (checkFreeSpaces() == 0)
        return 0;

    if (maximizing)
    {
        int bestVal = INT_MIN;
        for (int i = 0; i < 3; ++i) 
        {
            for (int j = 0; j < 3; ++j) 
            {
                if (board[i][j] == ' ') 
                {
                    board[i][j] = computer;
                    int value = minimax(false, alpha, beta, depth + 1, showLogic);
                    bestVal = max(bestVal, value);

                    if (showLogic && depth < 3)
                        cout << string(2 * depth, ' ') << "Max: (" << i << ", " << j << ") - Score: " << value << " Alpha: " << alpha << " Beta: " << beta << endl;
                    
                    board[i][j] = ' ';
                    alpha = max(alpha, bestVal);
                    if (beta <= alpha) // beta cut-off
                    {
                        if (showLogic)
                            cout << string(2 * depth, ' ') << "Beta cut-off at depth " << depth << endl;
                        break;
                    }
                }
            }
            if (beta <= alpha) // beta cut-off
                break;
        }
        score = bestVal;
    } 
    else
    {
        int bestVal = INT_MAX;
        for (int i = 0; i < 3; ++i) 
        {
            for (int j = 0; j < 3; ++j) 
            {
                if (board[i][j] == ' ') 
                {
                    board[i][j] = player;
                    int value = minimax(true, alpha, beta, depth + 1, showLogic);
                    bestVal = min(bestVal, value);

                    if (showLogic && depth < 3)
                        cout << string(2 * depth, ' ') << "Min: (" << i << ", " << j << ") - Score: " << value << " Alpha: " << alpha << " Beta: " << beta << endl;
                    
                    board[i][j] = ' ';
                    beta = min(beta, bestVal);
                    if (beta <= alpha) // alpha cut-off
                    {
                        if (showLogic)
                            cout << string(2 * depth, ' ') << "Alpha cut-off at depth " << depth << endl;
                        break;
                    }
                }
            }
            if (beta <= alpha) // alpha cut-off
                break;
        }
        score = bestVal;
    }
    return score;
}

void clearConsole() 
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void playAgain()
{
    char again = getValidInputForPlayAgain();

    switch (again)
    {
        case 'y':
        case 'Y':
            menu();
            break;

        case 'n':
        case 'N':
            cout << "Exiting...\n";
            break;

        default:
            break;
    }
}

char getValidInputForPlayAgain()
{
    char again;
    bool validInput = false;

    while (!validInput)
    {
        cout << "\n\nWanna play again?\ny - Yes\nn - No\n";
        cin >> again;

        switch (again)
        {
            case 'y':
            case 'Y':
            case 'n':
            case 'N':
                validInput = true;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;

            default:
                cout << "\n\tInvalid choice. Please enter 'y' or 'n'.";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
        }
    }

    return again;
}

void menu()
{
    int choice;

    cout << "\n   Menu:\n";
    cout << "1. 1 Player\n";
    cout << "2. 2 Players\n";
    cout << "3. Show computer's logic mode\n";
    cout << "4. Exit\n";
    choice = getValidInputMenu("Enter your choice: ");

    switch(choice)
    {
        case 1:
        {
            startGame(true, false);
            playAgain();
            break;
        }

        case 2:
        {
            startGame(false, false);
            playAgain();
            break;
        }

        case 3:
        {
            startGame(true, true);
            playAgain();
            break;
        }

        case 4:
        {
            cout << "Exiting...";
            break;
        }

        default:
            break;
    }
}

int getValidInputMenu(const string& prompt) 
{
    int value;
    bool validInput = false;
    int choice;

    while (!validInput) 
    {
        cout << prompt;
        cin >> choice;

        if (cin.fail() || choice < 1 || choice > 4) 
        {
            cerr << "\n\tInvalid input.\n\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } 
        else 
        {
            validInput = true;
            value = choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    return value;
}
