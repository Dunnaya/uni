// p - r - o - j - e - c - t //
// minimax algorithm
#include <iostream>
using namespace std;

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

int main() 
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
    return 0;
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
        cout << "Enter row #(1-3) and column #(1-3): ";
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
