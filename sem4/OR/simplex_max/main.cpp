#include <iostream>
#include "linear_program.h"
#include "simplex_solver.h"

int main(int argc, char* argv[]) 
{
    if (argc < 2) 
    {
        std::cerr << "Using: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    LinearProgram lp;
    lp.readInput(argv[1]);
    SimplexSolver solver(lp);
    solver.solve();
    return 0;
}