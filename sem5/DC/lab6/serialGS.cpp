#include <iostream>
#include <cmath>

void DummyDataInit(double *pMatrix, int size)
{
    // setting the grid node values
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
            if ((i == 0) || (i == size - 1) || (j == 0) || (j == size - 1))
                pMatrix[i * size + j] = 100;
            else
                pMatrix[i * size + j] = 0;
    }
}

void RandomDataInit(double *pMatrix, int size)
{
    srand(unsigned(clock()));
    // setting the grid node values
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
            if ((i == 0) || (i == size - 1) || (j == 0) || (j == size - 1))
                pMatrix[i * size + j] = 100;
            else
                pMatrix[i * size + j] = rand() / double(1000000);
    }
}

// memory alloc and initialization of grid nodes
void ProcessInit(double *&pMatrix, int &size, double &eps)
{
    // memory allocation
    pMatrix = new double[size * size];
    // setting the grid node values
    RandomDataInit(pMatrix, size);
    printf("\n");
}

void PrintMatrix(double *pMatrix, int rowCount, int colCount)
{
    for (int i = 0; i < rowCount; i++)
    {
        for (int j = 0; j < colCount; j++)
            printf("%7.4f ", pMatrix[i * colCount + j]);
        printf("\n");
    }
}

// Gauss-Seidel algoritm
void ResultCalculation(double *pMatrix, int size, double &eps, int &iterations)
{
    double dm, dmax, temp;
    iterations = 0;
    do
    {
        dmax = 0;
        for (int i = 1; i < size - 1; i++)
            for (int j = 1; j < size - 1; j++)
            {
                temp = pMatrix[size * i + j];
                pMatrix[size * i + j] = 0.25 * (pMatrix[size * i + j + 1] +
                                                pMatrix[size * i + j - 1] +
                                                pMatrix[size * (i + 1) + j] +
                                                pMatrix[size * (i - 1) + j]);
                dm = fabs(pMatrix[size * i + j] - temp);
                if (dmax < dm)
                    dmax = dm;
            }
        iterations++;
    } while (dmax > eps);
}

// computational process termination
void ProcessTermination(double *pMatrix)
{
    delete[] pMatrix;
}

int main()
{
    double *pMatrix; // matrix of the grid nodes
    int size;        // matrix size
    double eps;      // required accuracy
    int iterations;  // iteration number
    time_t start, finish;
    double duration;
    int matrixSizes[] = {10, 100, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000};

    printf("Serial Gauss - Seidel algorithm\n");
    for (int i = 0; i < 12; i++)
    {
        size = matrixSizes[i];
        eps = 1;
        printf("\nChosen size: %d with eps %f", size, eps);
        // process initialization
        ProcessInit(pMatrix, size, eps);
        // Gauss-Seidel method
        start = clock();
        ResultCalculation(pMatrix, size, eps, iterations);
        finish = clock();
        duration = (finish - start) / double(CLOCKS_PER_SEC);

        // printing the time spent by the Gauss-Seidel method
        printf("Execution time: %f\n", duration);
        printf("Iterations: %d\n", iterations);

        // computational process termination
        ProcessTermination(pMatrix);
    }
}