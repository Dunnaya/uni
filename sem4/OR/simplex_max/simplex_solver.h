#ifndef SIMPLEX_SOLVER_H
#define SIMPLEX_SOLVER_H

#include "simplex_tableau.h"
#include "linear_program.h"

class SimplexSolver 
{
    public:
        SimplexTableau tableau;
        LinearProgram lp;
        
        SimplexSolver(const LinearProgram &linear_program);
        void solve();
};

#endif // SIMPLEX_SOLVER_H