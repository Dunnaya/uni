#ifndef SIMPLEX_H
#define SIMPLEX_H

#include <vector>
#include <string>
#include <utility>

class LinearProgram 
{
    public:
        int numVariables, numConstraints;
        std::vector<double> objective;
        std::vector<std::vector<double> > constraints;
        std::vector<double> rhs;

        void readInput(const std::string& filename);
};

class Tableau 
{
    public:
        std::vector<std::vector<double> > table;
        int rows, cols;
        
        Tableau(const LinearProgram &lp);
        void printTableau(int iteration);
        void pivot(int row, int col);
        std::pair<int, int> findPivot();
};

class Solver 
{
    public:
        Solver(const LinearProgram &linear_program);
        void solve();

    private:
        Tableau tableau;
        const LinearProgram &lp;
};

#endif // SIMPLEX_H