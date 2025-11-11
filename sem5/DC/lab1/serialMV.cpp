#include <iostream>

// initialization of data
void DummyDataInit(double *pMatrix, double *pVector, int size)
{
    for (int i = 0; i < size; i++)
    {
        pVector[i] = 1;
        for (int j = 0; j < size; j++)
        {
            pMatrix[i * size + j] = i;
        }
    }
}

// function for random setting of the matrix and vector elements
void RandomDataInit(double *pMatrix, double *pVector, int Size)
{
    srand(unsigned(clock()));
    for (int i = 0; i < Size; i++)
    {
        pVector[i] = rand() / double(1000000);
        for (int j = 0; j < Size; j++)
            pMatrix[i * Size + j] = rand() / double(1000000);
    }
}

// function for memory allocation and data init
void ProcessInit(double *&pMatrix, double *&pVector, double *&pResult, int &size)
{
    // size for initial matrix and vectors
    do
    {
        std::cout << "\nEnter the size of the initial objects: ";
        std::cin >> size;
        if (size <= 0)
            std::cout << "\nThe size value must be positive number";
    } while (size <= 0);
    std::cout << "\nChosen object size: " << size << std::endl;

    pMatrix = new double[size * size];
    pVector = new double[size];
    pResult = new double[size];
    RandomDataInit(pMatrix, pVector, size);
}

// function for formatted matrix output
void PrintMatrix(double *pMatrix, int RowCount, int ColCount)
{
    for (int i = 0; i < RowCount; i++)
    {
        for (int j = 0; j < ColCount; j++)
            printf("%9.4f ", pMatrix[i * ColCount + j]);
        printf("\n");
    }
}
// function for formatted vector output
void PrintVector(double *pVector, int Size)
{
    for (int i = 0; i < Size; i++)
        printf("%7.4f ", pVector[i]);
    printf("\n");
}

// function for matrix-vector multiplication
void ResultCalculation(double *pMatrix, double *pVector, double *pResult, int size)
{
    for (int i = 0; i < size; i++)
    {
        pResult[i] = 0;
        for (int j = 0; j < size; j++)
            pResult[i] += pMatrix[i * size + j] * pVector[j];
    }
}

void ProcessTermination(double *pMatrix, double *pVector, double *pResult)
{
    delete[] pMatrix;
    delete[] pVector;
    delete[] pResult;
}

int main()
{
    double *pMatrix;
    double *pVector;
    double *pResult;
    int size;
    time_t start, finish;
    double duration;

    std::cout << "\nSerial matrix-vector multiplication program\n";

    ProcessInit(pMatrix, pVector, pResult, size);
    // //std::cout << "\nInitial matrix:\n";
    // PrintMatrix(pMatrix, size, size);
    // //std::cout << "Initial vector:\n";
    // PrintVector(pVector, size);

    start = clock();
    // matrix-vector multiplication
    ResultCalculation(pMatrix, pVector, pResult, size);
    finish = clock();
    // result vector
    //std::cout << "\nResult vector:\n";
    // PrintVector(pResult, size);

    duration = (finish - start) / double(CLOCKS_PER_SEC);

    // printing the time spent by matrix-vector multiplication
    printf("\n Time of execution: %f \n\n", duration);
    // computational process termination
    ProcessTermination(pMatrix, pVector, pResult);

    return 0;
}