#include <iostream>
#include <iomanip>
#include <limits>
#include "simplex_tableau.h"

SimplexTableau::SimplexTableau(const LinearProgram &lp) 
{
    rows = lp.numConstraints + 1;
    cols = lp.numVariables + lp.numConstraints + 1;
    
    table.resize(rows, std::vector<double>(cols, 0.0));
    
    for (int i = 0; i < lp.numConstraints; ++i) 
    {
        for (int j = 0; j < lp.numVariables; ++j) 
        {
            table[i][j] = lp.constraints[i][j];
        }
        table[i][lp.numVariables + i] = 1;
        table[i].back() = lp.rhs[i];
    }
    
    for (int j = 0; j < lp.numVariables; ++j)
        table[rows - 1][j] = -lp.objective[j];
}

void SimplexTableau::printTableau(int iteration) 
{
    std::cout << "\nIteration " << iteration << ":" << std::endl;
    std::cout << std::setw(10) << "Basis";
    
    for (int j = 0; j < cols; ++j) 
    {
        if (j < cols - 1)
            std::cout << std::setw(15) << "x" + std::to_string(j + 1);
        else
            std::cout << std::setw(15) << "b";
    }
    std::cout << std::endl;

    for (int i = 0; i < rows; ++i) 
    {
        if (i < rows - 1)
            std::cout << std::setw(10) << "s" + std::to_string(i + 1);
        else
            std::cout << std::setw(10) << "Z";

        for (int j = 0; j < cols; ++j) 
        {
            std::cout << std::setw(15) << std::fixed << std::setprecision(2) << table[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void SimplexTableau::pivot(int row, int col) 
{
    double pivotElement = table[row][col];
    for (double &x : table[row]) x /= pivotElement;
    
    for (int i = 0; i < rows; ++i) 
    {
        if (i == row) continue;
        double ratio = table[i][col];
        for (int j = 0; j < cols; ++j)
            table[i][j] -= ratio * table[row][j];
    }
}

std::pair<int, int> SimplexTableau::findPivot() 
{
    int col = -1;
    for (int j = 0; j < cols - 1; ++j) 
    {
        if (table[rows - 1][j] < 0) 
        {
            col = j;
            break;
        }
    }
    if (col == -1) return std::make_pair(-1, -1);
    
    int row = -1;
    double minRatio = std::numeric_limits<double>::max();
    for (int i = 0; i < rows - 1; ++i) 
    {
        if (table[i][col] > 0) 
        {
            double ratio = table[i].back() / table[i][col];
            if (ratio < minRatio) 
            {
                minRatio = ratio;
                row = i;
            }
        }
    }
    return std::make_pair(row, col);
}