#include <iostream>
using namespace std;
#include "minimax.h"

void bestMove()
{
    int bestScore = -INF;
    Move move;
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            if(board[i][j] == ' ')
            {
                board[i][j] = ai;
                int score = minimax(board, 0, true);
                if(score > bestScore)
                {
                    bestScore = score;
                    move.row = i;
                    move.col = j;
                }
            }
        }
    }
    board[move.row][move.col] = ai;
}

int minimax(char board[3][3], bool isMaximizing, int depth)
{
    return 1;
}

int main()
{
    return 0;
}
