#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <iomanip>
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

        void printTableau(int iteration) 
        {
            cout << "\nIteration " << iteration << ":" << endl;
            cout << setw(10) << "Basis";
            
            // Друк назв змінних
            for (int j = 0; j < cols; ++j) 
            {
                if (j < cols - 1)
                    cout << setw(15) << "x" + to_string(j + 1);
                else
                    cout << setw(15) << "b";
            }
            cout << endl;

            // Друк рядків таблиці
            for (int i = 0; i < rows; ++i) 
            {
                // Назва базисної змінної
                if (i < rows - 1)
                    cout << setw(10) << "s" + to_string(i + 1);
                else
                    cout << setw(10) << "Z";

                // Друк значень
                for (int j = 0; j < cols; ++j) 
                {
                    cout << setw(15) << fixed << setprecision(2) << table[i][j];
                }
                cout << endl;
            }
            cout << endl;
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
        LinearProgram lp;
        
        SimplexSolver(const LinearProgram &linear_program) : tableau(linear_program), lp(linear_program) {}
        
        void solve() 
        {
            int iteration = 0;
            tableau.printTableau(iteration);

            while (true) 
            {
                auto [row, col] = tableau.findPivot();
                if (col == -1) break;
                if (row == -1) {
                    cout << "The solution does not exist or is infinite." << endl;
                    return;
                }

                cout << "Pivot: row " << row + 1 << ", col " << col + 1 << endl;
                
                tableau.pivot(row, col);
                ++iteration;
                tableau.printTableau(iteration);
            }

            // Виведення значень змінних x
            cout << "\nOptimal solution:" << endl;
            vector<double> xValues(lp.numVariables, 0.0);

            // Пошук базисних змінних
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
            
            // Виведення значень змінних
            for (int i = 0; i < lp.numVariables; ++i) {
                cout << "x" << i + 1 << " = " << fixed << setprecision(2) << xValues[i] << endl;
            }
            
            // Виведення оптимального значення цільової функції
            cout << "Zmax = " << tableau.table.back().back() << endl;
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