#ifndef LINEAR_PROGRAM_H
#define LINEAR_PROGRAM_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class LinearProgram 
{
    public:
        int numVariables, numConstraints;
        std::vector<double> objective;
        std::vector<std::vector<double> > constraints;
        std::vector<double> rhs;

        void readInput(const std::string& filename);
};

#endif // LINEAR_PROGRAM_H