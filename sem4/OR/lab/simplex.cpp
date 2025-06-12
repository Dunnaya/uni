#include "simplex.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>

using namespace std;

void LinearProgram::readInput(const string& filename) 
{
    ifstream file(filename);
    if (!file) 
    {
        cerr << "Failed opening the file!" << endl;
        exit(1);
    }
    
    file >> numVariables >> numConstraints;
    
    objective.resize(numVariables);
    for (auto &c : objective) file >> c;
    
    constraints.resize(numConstraints, vector<double>(numVariables));
    rhs.resize(numConstraints);
    
    for (int i = 0; i < numConstraints; ++i)
        for (int j = 0; j < numVariables; ++j)
            file >> constraints[i][j];
    
    for (auto &b : rhs) file >> b;
}

Tableau::Tableau(const LinearProgram &lp) 
{
    rows = lp.numConstraints + 1;
    cols = lp.numVariables + lp.numConstraints + 1;
    
    table.resize(rows, vector<double>(cols, 0.0));
    
    // set up constraint rows
    for (int i = 0; i < lp.numConstraints; ++i) 
    {
        // copy constraint coefficients
        for (int j = 0; j < lp.numVariables; ++j) 
            table[i][j] = lp.constraints[i][j];
        
        // set slack variable
        table[i][lp.numVariables + i] = 1;
        
        // set RHS (vector of right-hand sides of constraints)
        table[i][cols-1] = lp.rhs[i];
    }
    
    // set up objective row (negated)
    for (int j = 0; j < lp.numVariables; ++j)
        table[rows-1][j] = -lp.objective[j];
}

void Tableau::printTableau(int iteration) 
{
    cout << "\nIteration " << iteration << ":" << endl;
    cout << setw(10) << "Basis";
    
    // print variable names
    for (int j = 0; j < cols; ++j) 
        cout << setw(15) << (j < cols-1 ? "x" + to_string(j+1) : "b");
    cout << endl;

    // print table rows
    for (int i = 0; i < rows; ++i) 
    {
        cout << setw(10) << (i < rows-1 ? "s" + to_string(i+1) : "Z");

        for (int j = 0; j < cols; ++j) 
            cout << setw(15) << fixed << setprecision(2) << table[i][j];
        cout << endl;
    }
    cout << endl;
}

void Tableau::pivot(int row, int col) 
{
    double pivotElement = table[row][col];
    
    // normalize the pivot row
    for (auto &x : table[row]) x /= pivotElement;
    
    // upd other rows
    for (int i = 0; i < rows; ++i) 
    {
        if (i == row) continue;
        
        double ratio = table[i][col];
        for (int j = 0; j < cols; ++j)
            table[i][j] -= ratio * table[row][j];
    }
}

pair<int, int> Tableau::findPivot() 
{
    // find entering variable (column)
    int col = -1;
    for (int j = 0; j < cols-1; ++j) 
    {
        if (table[rows-1][j] < 0) 
        {
            col = j;
            break;
        }
    }
    if (col == -1) return make_pair(-1, -1); // optimal solution found
    
    // find leaving variable (row)
    int row = -1;
    double minRatio = numeric_limits<double>::max();
    for (int i = 0; i < rows-1; ++i) 
    {
        if (table[i][col] > 0) 
        {
            double ratio = table[i][cols-1] / table[i][col];
            if (ratio < minRatio) 
            {
                minRatio = ratio;
                row = i;
            }
        }
    }
    return make_pair(row, col);
}

Solver::Solver(const LinearProgram &linear_program) 
    : tableau(linear_program), lp(linear_program) {}

void Solver::solve() 
{
    int iteration = 0;
    tableau.printTableau(iteration);

    while (true) 
    {
        auto [row, col] = tableau.findPivot();
        
        // check if we're done or no solution exists
        if (col == -1) break;
        if (row == -1) 
        {
            cout << "The solution does not exist or is infinite." << endl;
            return;
        }

        cout << "Pivot: row " << row+1 << ", col " << col+1 << endl;
        
        tableau.pivot(row, col);
        tableau.printTableau(++iteration);
    }

    // find optimal solution values
    vector<double> xValues(lp.numVariables, 0.0);

    // find basic variables
    for (int j = 0; j < lp.numVariables; ++j) 
    {
        for (int i = 0; i < tableau.rows-1; ++i) 
        {
            // check if this is a basic column
            if (tableau.table[i][j] == 1.0) 
            {
                bool isBasic = true;
                for (int k = 0; k < tableau.rows; ++k) 
                {
                    if (k != i && tableau.table[k][j] != 0.0) 
                    {
                        isBasic = false;
                        break;
                    }
                }
                
                if (isBasic) 
                {
                    xValues[j] = tableau.table[i][tableau.cols-1];
                    break;
                }
            }
        }
    }
    
    // finally print solutiond
    cout << "\nOptimal solution:" << endl;
    for (int i = 0; i < lp.numVariables; ++i) 
    {
        cout << "x" << i+1 << " = " << fixed << setprecision(2) << xValues[i] << endl;
    }
    
    cout << "Zmax = " << tableau.table.back().back() << endl;
}

int main(int argc, char* argv[]) 
{
    if (argc < 2) 
    {
        cerr << "Using: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    LinearProgram lp;
    lp.readInput(argv[1]);
    
    Solver solver(lp);
    solver.solve();
    
    return 0;
}