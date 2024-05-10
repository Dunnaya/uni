// minimax algorithm
#include <iostream>
using namespace std;
#include "tictactoe.h"

int main() 
{
   menu();
   return 0;
}

void startGame1()
{
    char winner = ' ';

    resetBoard();

    while (winner == ' ' && checkFreeSpaces() != 0)
    {
        printBoard();

        playerMove();
        winner = checkWinner();
        if(winner != ' ' || checkFreeSpaces() == 0)
        {
            break;
        }

        computerMove();
        winner = checkWinner();
        if(winner != ' ' || checkFreeSpaces() == 0)
        {
            break;
        }
    }
    printBoard();
    printWinner(winner);
}

void resetBoard() 
{
    for(int i = 0; i < 3; i++) 
    {
        for(int j = 0; j < 3; j++) 
        {
            board[i][j] = ' '; // just replace all with empty spaces
        }
    }
}

void printBoard() 
{
    cout << " " << board[0][0] << " | " << board[0][1] << " | " << board[0][2] << " \n";
    cout << "---|---|---\n";
    cout << " " << board[1][0] << " | " << board[1][1] << " | " << board[1][2] << " \n";
    cout << "---|---|---\n";
    cout << " " << board[2][0] << " | " << board[2][1] << " | " << board[2][2] << " \n";
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

void playerMove() 
{
    int x, y;
    do
    {
        cout << "\nEnter row #(1-3) and column #(1-3): ";
        cin >> x;
        cin >> y;
        x--;
        y--;

        if(board[x][y] != ' ')
        {
            cout << "Invalid move!\n";
        }
        else 
        {
            board[x][y] = player;
            break;
        }
    } while (board[x][y] != ' ');
}

void computerMove() 
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
                int moveVal = minimax(false);
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
    //rows
    for(int i = 0; i <3; i++)
    {
        if(board[i][0] == board[i][1] && board[i][0] == board[i][2])
        {
            return board[i][0];
        }
    }
    //columns
    for(int i = 0; i <3; i++)
    {
        if(board[0][i] == board[1][i] && board[0][i] == board[2][i])
        {
            return board[0][i];
        }
    }
    //diagonals
    if(board[0][0] == board[1][1] && board[0][0] == board[2][2])
    {
        return board[0][0];
    }

    if(board[0][2] == board[1][1] && board[0][2] == board[2][0])
    {
        return board[0][2];
    }

    return ' ';
}

void printWinner(char winner) 
{
    if(winner == player)
    {
      cout << "You win!";
    }
    else if(winner == computer)
    {
      cout << "You lose!";
    }
    else
    {
      cout << "It's a tie!";
    }
}

int minimax(bool maximizing) 
{
    int score = 0;
    char win = checkWinner();
    if (win == computer) {
        return 10;
    } else if (win == player) {
        return -10;
    } else if (checkFreeSpaces() == 0) {
        return 0;
    }

    if (maximizing) //for computer
    {
        int bestVal = INT_MIN;
        for (int i = 0; i < 3; ++i) 
        {
            for (int j = 0; j < 3; ++j) 
            {
                if (board[i][j] == ' ') 
                {
                    board[i][j] = computer;
                    bestVal = max(bestVal, minimax(false));
                    board[i][j] = ' ';
                }
            }
        }
        score = bestVal;
    } 
    else //minimizing for player
    {
        int bestVal = INT_MAX;
        for (int i = 0; i < 3; ++i) 
        {
            for (int j = 0; j < 3; ++j) 
            {
                if (board[i][j] == ' ') 
                {
                    board[i][j] = player;
                    bestVal = min(bestVal, minimax(true));
                    board[i][j] = ' ';
                }
            }
        }
        score = bestVal;
    }
    return score;
}

void menu()
{
    int choice;

    cout << "\n   Menu:\n";
    cout << "1. 1 Player\n";
    cout << "2. 2 Players\n";
    cout << "3. Exit\n";
    cout << "Enter your choice: ";
    cin >> choice;

    switch(choice)
    {
        case 1:
        {
            char again;
            startGame1();
            cout << "\n\nWanna play again?\ny - Yes\nn - No\n";
            cin >> again;

            switch (again)
            {
                case 'y':
                {
                    menu();
                    break;
                }

                case 'Y':
                {
                    menu();
                    break;
                }

                case 'n':
                {
                    cout << "Exiting...";
                    break;
                }

                case 'N':
                {
                    cout << "Exiting...";
                    break;
                }
            
                default:
                    cout << "Invalid choice.\n";
            }

            break;
        }

        case 2:
        {
            break;
        }

        case 3:
        {
            cout << "Exiting...";
            break;
        }

        default:
            cout << "Invalid choice.\n";
    }
}
