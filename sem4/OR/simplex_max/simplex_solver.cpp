#include <iostream>
#include <iomanip>
#include "simplex_solver.h"

SimplexSolver::SimplexSolver(const LinearProgram &linear_program) 
    : tableau(linear_program), lp(linear_program) {}

void SimplexSolver::solve() 
{
    int iteration = 0;
    tableau.printTableau(iteration);

    while (true) 
    {
        auto [row, col] = tableau.findPivot();
        if (col == -1) break;
        if (row == -1) {
            std::cout << "The solution does not exist or is infinite." << std::endl;
            return;
        }

        std::cout << "Pivot: row " << row + 1 << ", col " << col + 1 << std::endl;
        
        tableau.pivot(row, col);
        ++iteration;
        tableau.printTableau(iteration);
    }

    std::cout << "\nOptimal solution:" << std::endl;
    std::vector<double> xValues(lp.numVariables, 0.0);

    for (int j = 0; j < lp.numVariables; ++j) {
        int baseRow = -1;
        for (int i = 0; i < tableau.rows - 1; ++i) {
            if (tableau.table[i][j] == 1.0) {
                bool isBaseColumn = true;
                for (int k = 0; k < tableau.rows; ++k) {
                    if (k != i && tableau.table[k][j] != 0.0) {
                        isBaseColumn = false;
                        break;
                    }
                }
                if (isBaseColumn) {
                    baseRow = i;
                    break;
                }
            }
        }
        
        if (baseRow != -1) {
            xValues[j] = tableau.table[baseRow].back();
        }
    }
    
    for (int i = 0; i < lp.numVariables; ++i) {
        std::cout << "x" << i + 1 << " = " << std::fixed << std::setprecision(2) << xValues[i] << std::endl;
    }
    
    std::cout << "Zmax = " << tableau.table.back().back() << std::endl;
}