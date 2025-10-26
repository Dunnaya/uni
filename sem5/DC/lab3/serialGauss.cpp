#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int *pSerialPivotPos;  // num of pivot rows selected at the iterations
int *pSerialPivotIter; // iterations, at which the rows were pivots

void DummyDataInitialization(double *pMatrix, double *pVector, int Size)
{
    for (int i = 0; i < Size; i++)
    {
        pVector[i] = i + 1;
        for (int j = 0; j < Size; j++)
        {
            if (j <= i)
                pMatrix[i * Size + j] = 1;
            else
                pMatrix[i * Size + j] = 0;
        }
    }
}

void RandomDataInitialization(double *pMatrix, double *pVector, int Size)
{
    srand(unsigned(clock()));
    for (int i = 0; i < Size; i++)
    {
        pVector[i] = rand() / double(1000000);
        for (int j = 0; j < Size; j++)
        {
            if (j <= i)
                pMatrix[i * Size + j] = rand() / double(1000000);
            else
                pMatrix[i * Size + j] = 0;
        }
    }
}

void ProcessInitialization(double *&pMatrix, double *&pVector, double *&pResult, int &Size)
{
    // setting size of matrix and vector
    // do
    // {
    //     printf("\nEnter the size of initial objects: ");
    //     scanf("%d", &Size);
    //     printf("\nChosen size = %d \n", Size);
    //     if (Size <= 0)
    //         printf("\nSize of objects must be greater than 0!\n");
    // } while (Size <= 0);

    // memory alloc
    pMatrix = new double[Size * Size];
    pVector = new double[Size];
    pResult = new double[Size];

    // DummyDataInitialization(pMatrix, pVector, Size);
    RandomDataInitialization(pMatrix, pVector, Size);
}

void PrintMatrix(double *pMatrix, int RowCount, int ColCount)
{
    for (int i = 0; i < RowCount; i++)
    {
        for (int j = 0; j < ColCount; j++)
            printf("%10.4f ", pMatrix[i * RowCount + j]);
        printf("\n");
    }
    printf("\n");
}

void PrintVector(double *pVector, int Size)
{
    for (int i = 0; i < Size; i++)
        printf("%8.4f ", pVector[i]);
    printf("\n");
}

int FindPivotRow(double *pMatrix, int Size, int Iter)
{
    int PivotRow = -1;   // index of the pivot row
    double MaxValue = 0; // value of the pivot element

    // choose the row, that stores the maximum element
    for (int i = 0; i < Size; i++)
    {
        if ((pSerialPivotIter[i] == -1) &&
            (fabs(pMatrix[i * Size + Iter]) > MaxValue))
        {
            PivotRow = i;
            MaxValue = fabs(pMatrix[i * Size + Iter]);
        }
    }
    return PivotRow;
}

void SerialColumnElimination(double *pMatrix, double *pVector, int Pivot, int Iter, int Size)
{
    double PivotValue = pMatrix[Pivot * Size + Iter];
    double PivotFactor;

    // loop over all rows except the pivot row
    for (int i = 0; i < Size; i++)
    {
        if (pSerialPivotIter[i] == -1 && i != Pivot)
        {
            PivotFactor = pMatrix[i * Size + Iter] / PivotValue;
            for (int j = Iter; j < Size; j++)
                pMatrix[i * Size + j] -= PivotFactor * pMatrix[Pivot * Size + j];
            pVector[i] -= PivotFactor * pVector[Pivot];
        }
    }
}

// Gaussian elimination
void SerialGaussianElimination(double *pMatrix, double *pVector, int Size)
{
    int Iter;     // num of the iteration of the Gaussian elimination
    int PivotRow; // num of the current pivot row
    for (Iter = 0; Iter < Size; Iter++)
    {
        // finding the pivot row
        PivotRow = FindPivotRow(pMatrix, Size, Iter);
        pSerialPivotPos[Iter] = PivotRow;
        pSerialPivotIter[PivotRow] = Iter;
        SerialColumnElimination(pMatrix, pVector, PivotRow, Iter, Size);
    }
}

// back substitution
void SerialBackSubstitution(double *pMatrix, double *pVector, double *pResult, int Size)
{
    for (int i = Size - 1; i >= 0; i--)
    {
        int Row = pSerialPivotPos[i];
        double sum = 0.0;

        // sum over already calculated x[j]
        for (int j = i + 1; j < Size; j++)
            sum += pMatrix[Row * Size + j] * pResult[j];

        pResult[i] = (pVector[Row] - sum) / pMatrix[Row * Size + i];
    }
}

// execution of the Gauss algorithm
void SerialResultCalculation(double *pMatrix, double *pVector, double *pResult, int Size)
{
    // memory alloc
    pSerialPivotPos = new int[Size];
    pSerialPivotIter = new int[Size];
    for (int i = 0; i < Size; i++)
    {
        pSerialPivotIter[i] = -1;
    }

    // Gaussian elimination
    SerialGaussianElimination(pMatrix, pVector, Size);

    // back substitution
    SerialBackSubstitution(pMatrix, pVector, pResult, Size);

    // memory dealloc
    delete[] pSerialPivotPos;
    delete[] pSerialPivotIter;
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
    double duration;
    clock_t start, finish;

    int sizes[] = {10, 100, 500, 1000, 1500, 2000, 2500, 3000};
    int numTests = sizeof(sizes) / sizeof(sizes[0]);

    printf("Serial Gauss algorithm timing test\n\n");

    for (int test = 0; test < numTests; test++)
    {
        int Size = sizes[test];
        printf("Running test for matrix size = %d\n", Size);

        ProcessInitialization(pMatrix, pVector, pResult, Size);

        start = clock();
        SerialResultCalculation(pMatrix, pVector, pResult, Size);
        finish = clock();

        duration = (finish - start) / double(CLOCKS_PER_SEC);
        printf("Execution time: %e seconds\n\n", duration);

        ProcessTermination(pMatrix, pVector, pResult);
    }

    return 0;
}