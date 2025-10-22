#include <iostream>
#include <stdlib.h>
#include <time.h>

void DummyDataInit(double *pAMatrix, double *pBMatrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            pAMatrix[i * size + j] = 1;
            pBMatrix[i * size + j] = 1;
        }
    }
}

void RandomDataInit(double *pAMatrix, double *pBMatrix, int size)
{
    srand(unsigned(clock()));
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            pAMatrix[i * size + j] = rand() / double(1000000);
            pBMatrix[i * size + j] = rand() / double(1000000);
        }
    }
}

void ProcessInit(double* &pAMatrix, double* &pBMatrix, double* &pCMatrix, int &size)
{
    do {
        std::cout << "\nEnter the size of the initial matrices: ";
        std::cin >> size;
        if (size <= 0)
            std::cout << "\nThe size value must be positive number";
    } while (size <= 0);

    //memory alloc
    pAMatrix = new double[size * size];
    pBMatrix = new double[size * size];
    pCMatrix = new double[size * size];

    //init of matrix elements
    RandomDataInit(pAMatrix, pBMatrix, size);
    for(int i = 0; i < size * size; i++)
        pCMatrix[i] = 0;
}

void PrintMatrix(double *pMatrix, int colCount, int rowCount)
{
    for (int i = 0; i < rowCount; i++)
    {
        for (int j = 0; j < colCount; j++)
            printf("%7.4f ", pMatrix[i * colCount + j]);
        std::cout << std::endl;
    }
}

void ResultCalculation(double *pAMatrix, double *pBMatrix, double *pCMatrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            for (int k = 0; k < size; k++)
            {
                pCMatrix[i * size + j] += pAMatrix[i * size + k] * pBMatrix[k * size + j];
            }
        }
    }
}

void ProcessTermination(double *pAMatrix, double *pBMatrix, double *pCMatrix)
{
    delete[] pAMatrix;
    delete[] pBMatrix;
    delete[] pCMatrix;
}

int main() {
    double *pAMatrix;
    double *pBMatrix;
    double *pCMatrix;
    int size;
    time_t start, finish;
    double duration;

    std::cout << "Serial Matrix Multiplication Program\n";
    ProcessInit(pAMatrix, pBMatrix, pCMatrix, size);

    // matrix output
    // std::cout << "\nInitial A Matrix:" << std::endl;
    // PrintMatrix(pAMatrix, size, size);
    // std::cout << "\nInitial B Matrix:" <<  std::endl;
    // PrintMatrix(pBMatrix, size, size);
    
    //matrix multiplication
    start = clock();
    ResultCalculation(pAMatrix, pBMatrix, pCMatrix, size);
    finish = clock();
    duration = double(finish - start) / CLOCKS_PER_SEC;

    // std::cout << "\nResult Matrix C (A x B):" << std::endl;
    // PrintMatrix(pCMatrix, size, size);

    printf("\nTime of execution: %f\n", duration);
    ProcessTermination(pAMatrix, pBMatrix, pCMatrix);

    return 0;
}