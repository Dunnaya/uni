#include "linear_program.h"

void LinearProgram::readInput(const std::string& filename) 
{
    std::ifstream file(filename);
    if (!file) 
    {
        std::cerr << "Failed opening the file!" << std::endl;
        exit(1);
    }
    
    file >> numVariables >> numConstraints;
    objective.resize(numVariables);
    for (double &c : objective) file >> c;
    
    constraints.resize(numConstraints, std::vector<double>(numVariables));
    rhs.resize(numConstraints);
    
    for (int i = 0; i < numConstraints; ++i)
        for (int j = 0; j < numVariables; ++j)
            file >> constraints[i][j];
    
    for (double &b : rhs) file >> b;
}