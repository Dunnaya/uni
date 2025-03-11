#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
using namespace std;

class LinearProgram 
{
    public:
        int numVariables, numConstraints;
        vector<double> objective;
        vector<vector<double> > constraints;
        vector<double> rhs;

        void readInput(const string& filename) 
        {
            ifstream file(filename);
            if (!file) 
            {
                cerr << "Failed opening the file!" << endl;
                exit(1);
            }
            
            file >> numVariables >> numConstraints;
            objective.resize(numVariables);
            for (double &c : objective) file >> c;
            
            constraints.resize(numConstraints, vector<double>(numVariables));
            rhs.resize(numConstraints);
            
            for (int i = 0; i < numConstraints; ++i)
                for (int j = 0; j < numVariables; ++j)
                    file >> constraints[i][j];
            
            for (double &b : rhs) file >> b;
        }
};

class SimplexTableau 
{
    public:
        vector<vector<double> > table;
        int rows, cols;
        
        SimplexTableau(const LinearProgram &lp) 
        {
            rows = lp.numConstraints + 1;
            cols = lp.numVariables + lp.numConstraints + 1;
            
            table.resize(rows, vector<double>(cols, 0.0));
            
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

        void pivot(int row, int col) 
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

        pair<int, int> findPivot() 
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
            double minRatio = numeric_limits<double>::max();
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
            return make_pair(row, col);
        }
};

class SimplexSolver 
{
    public:
        SimplexTableau tableau;
        
        SimplexSolver(const LinearProgram &lp) : tableau(lp) {}
        
        void solve() 
        {
            while (true) 
            {
                auto [row, col] = tableau.findPivot();
                if (col == -1) break;
                if (row == -1) {
                    cout << "The solution does not exist or is infinite." << endl;
                    return;
                }
                tableau.pivot(row, col);
            }
            cout << "The optimal value of the objective function: " << tableau.table.back().back() << endl;
        }
};

int main(int argc, char* argv[]) 
{
    if (argc < 2) 
    {
        cerr << "Using: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    LinearProgram lp;
    lp.readInput(argv[1]);
    SimplexSolver solver(lp);
    solver.solve();
    return 0;
}