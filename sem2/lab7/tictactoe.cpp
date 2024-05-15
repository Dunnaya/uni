// tic-tac-toe game
// minimax algorithm
#include <iostream>
using namespace std;
#include "tictactoe.h"

int main() 
{
   menu();
   return 0;
}

void startGame(bool isComputer = true)
{
    char winner = ' ';

    resetBoard();

    while (winner == ' ' && checkFreeSpaces() != 0)
    {
        printBoard();

        playerMove(true);
        winner = checkWinner();
        if(winner != ' ' || checkFreeSpaces() == 0)
        {
            break;
        }

        if(isComputer) computerMove();
        else playerMove(false);

        winner = checkWinner();
        if(winner != ' ' || checkFreeSpaces() == 0)
        {
            break;
        }
    }

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

void playerMove(bool is1Player = true) 
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
            if (is1Player) board[x][y] = player;
            else board[x][y] = player2;
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
    else 
    {
        if(winner == player)
        {
        cout << "Player #1 wins!";
        }
        else if(winner == player2)
        {
        cout << "Player #2 wins!";
        }
        else
        {
        cout << "It's a tie!";
        }
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

void playAgain()
{
    char again;
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
            startGame(true);
            playAgain();
            break;
        }

        case 2:
        {
            startGame(false);
            playAgain();
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
