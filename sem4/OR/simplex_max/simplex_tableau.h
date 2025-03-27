#ifndef SIMPLEX_TABLEAU_H
#define SIMPLEX_TABLEAU_H

#include <vector>
#include <utility>
#include "linear_program.h"

class SimplexTableau 
{
    public:
        std::vector<std::vector<double> > table;
        int rows, cols;
        
        SimplexTableau(const LinearProgram &lp);
        void printTableau(int iteration);
        void pivot(int row, int col);
        std::pair<int, int> findPivot();
};

#endif // SIMPLEX_TABLEAU_H