#include <iostream>
#include <cmath>
#include <mpi.h>

int procNum = 0;        // number of the available processes
int procRank = 0;       // rank of the current process
int *pProcInd;          // number of the first row located on the processes
int *pProcNum;          // number of the linear system rows located on the processes
int *pParallelPivotPos; // number of rows selected as the pivot ones
int *pProcPivotIter;    // number of iterations, at which the process
// rows were used as the pivot ones

void DummyDataInit(double *pMatrix, double *pVector, int size)
{
    for (int i = 0; i < size; i++)
    {
        pVector[i] = i + 1;
        for (int j = 0; j < size; j++)
        {
            if (j <= i)
                pMatrix[i * size + j] = 1;
            else
                pMatrix[i * size + j] = 0;
        }
    }
}

void RandomDataInit(double *pMatrix, double *pVector, int size)
{
    srand(unsigned(clock()));
    for (int i = 0; i < size; i++)
    {
        pVector[i] = rand() / double(1000000);
        for (int j = 0; j < size; j++)
        {
            if (j <= i)
                pMatrix[i * size + j] = rand() / double(1000000);
            else
                pMatrix[i * size + j] = 0;
        }
    }
}

void PrintMatrix(double *pMatrix, int rowCount, int colCount)
{
    for (int i = 0; i < rowCount; i++)
    {
        for (int j = 0; j < colCount; j++)
            printf("%9.4f ", pMatrix[i * colCount + j]);
        printf("\n");
    }
}

void PrintVector(double *pVector, int size)
{
    for (int i = 0; i < size; i++)
        printf("%7.4f ", pVector[i]);
    printf("\n");
}

void PrintResultVector(double *pResult, int size)
{
    for (int i = 0; i < size; i++)
        printf("%7.4f ", pResult[pParallelPivotPos[i]]);
}

// memory alloc and data init
void ProcessInitialization(double *&pMatrix, double *&pVector, double *&pResult, double *&pProcRows, double *&pProcVector, double *&pProcResult, int &size, int &rowNum)
{
    // setting the size of the matrix and the vector
    // if (procRank == 0)
    // {
    //     do
    //     {
    //         printf("\nEnter the size of the matrix and the vector: ");
    //         scanf("%d", &size);
    //         if (size < procNum)
    //         {
    //             printf("Size must be greater than number of processes! \n");
    //         }
    //     } while (size < procNum);
    // }

    MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int restRows = size;
    for (int i = 0; i < procRank; i++)
        restRows = restRows - restRows / (procNum - i);
    rowNum = restRows / (procNum - procRank);

    pProcRows = new double[rowNum * size];
    pProcVector = new double[rowNum];
    pProcResult = new double[rowNum];

    pProcInd = new int[procNum];
    pProcNum = new int[procNum];
    if (procRank == 0)
    {
        pMatrix = new double[size * size];
        pVector = new double[size];
        pResult = new double[size];
        RandomDataInit(pMatrix, pVector, size);
    }
}

// data distribution among the processes
void DataDistribution(double *pMatrix, double *pProcRows, double *pVector, double *pProcVector, int size, int rowNum)
{
    int *pSendNum;       // number of the elements sent to the process
    int *pSendInd;       // index of the first data element sent to the process
    int restRows = size; // number of rows, that have not been distributed yet

    pSendInd = new int[procNum];
    pSendNum = new int[procNum];

    // define the disposition of the matrix rows for the current process
    rowNum = (size / procNum);
    pSendNum[0] = rowNum * size;
    pSendInd[0] = 0;
    for (int i = 1; i < procNum; i++)
    {
        restRows -= rowNum;
        rowNum = restRows / (procNum - i);
        pSendNum[i] = rowNum * size;
        pSendInd[i] = pSendInd[i - 1] + pSendNum[i - 1];
    }

    // scatter the rows
    MPI_Scatterv(pMatrix, pSendNum, pSendInd, MPI_DOUBLE, pProcRows, pSendNum[procRank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // define the disposition of the matrix rows for current process
    restRows = size;
    pProcInd[0] = 0;
    pProcNum[0] = size / procNum;
    for (int i = 1; i < procNum; i++)
    {
        restRows -= pProcNum[i - 1];
        pProcNum[i] = restRows / (procNum - i);
        pProcInd[i] = pProcInd[i - 1] + pProcNum[i - 1];
    }

    MPI_Scatterv(pVector, pProcNum, pProcInd, MPI_DOUBLE, pProcVector, pProcNum[procRank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // free the memory
    delete[] pSendNum;
    delete[] pSendInd;
}

// testing the data distribution
void TestDistribution(double *pMatrix, double *pVector, double *pProcRows, double *pProcVector, int size, int rowNum)
{
    if (procRank == 0)
    {
        printf("Initial Matrix: \n");
        PrintMatrix(pMatrix, size, size);
        printf("Initial Vector: \n");
        PrintVector(pVector, size);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    for (int i = 0; i < procNum; i++)
    {
        if (procRank == i)
        {
            printf("\nProcRank = %d \n", procRank);
            printf(" Matrix Stripe:\n");
            PrintMatrix(pProcRows, rowNum, size);
            printf(" Vector: \n");
            PrintVector(pProcVector, rowNum);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
}

// the column elimination
void ParallelEliminateColumns(double *pProcRows, double *pProcVector, double *pPivotRow, int size, int rowNum, int iter)
{
    double pivotFactor;
    for (int i = 0; i < rowNum; i++)
    {
        if (pProcPivotIter[i] == -1)
        {
            pivotFactor = pProcRows[i * size + iter] / pPivotRow[iter];
            for (int j = iter; j < size; j++)
            {
                pProcRows[i * size + j] -= pivotFactor * pPivotRow[j];
            }
            pProcVector[i] -= pivotFactor * pPivotRow[size];
        }
    }
}

// Gaussian elimination
void ParallelGaussianElimination(double *pProcRows, double *pProcVector, int size, int rowNum)
{
    double maxValue; // value of the pivot element of thе process
    int pivotPos;    // position of the pivot row in the process stripe
    struct
    {
        double maxValue;
        int procRank;
    } procPivot, pivot;
    double *pPivotRow; // pivot row of the current iteration
    pPivotRow = new double[size + 1];

    // the iterations of the Gaussian elimination
    for (int i = 0; i < size; i++)
    {
        // calc the local pivot row
        maxValue = 0;
        for (int j = 0; j < rowNum; j++)
        {
            if ((pProcPivotIter[j] == -1) &&
                (maxValue < fabs(pProcRows[j * size + i])))
            {
                maxValue = fabs(pProcRows[j * size + i]);
                pivotPos = j;
            }
        }

        // finding the global pivot row
        procPivot.maxValue = maxValue;
        procPivot.procRank = procRank;
        
        // finding the pivot process
        MPI_Allreduce(&procPivot, &pivot, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD);

        // storing the number of the pivot row
        if (procRank == pivot.procRank)
        {
            pProcPivotIter[pivotPos] = i;
            pParallelPivotPos[i] = pProcInd[procRank] + pivotPos;
        }
        MPI_Bcast(&pParallelPivotPos[i], 1, MPI_INT, pivot.procRank, MPI_COMM_WORLD);

        // broadcasting the pivot row
        if (procRank == pivot.procRank)
        {
            // fill the pivot row
            for (int j = 0; j < size; j++)
            {
                pPivotRow[j] = pProcRows[pivotPos * size + j];
            }
            pPivotRow[size] = pProcVector[pivotPos];
        }
        MPI_Bcast(pPivotRow, size + 1, MPI_DOUBLE, pivot.procRank, MPI_COMM_WORLD);

        // column elimination
        ParallelEliminateColumns(pProcRows, pProcVector, pPivotRow, size, rowNum, i);
    }
    delete[] pPivotRow;
}

// finding the pivot row of the back substitution
void FindBackPivotRow(int rowIndex, int &iterProcRank, int &iterPivotPos)
{
    for (int i = 0; i < procNum - 1; i++)
    {
        if ((pProcInd[i] <= rowIndex) && (rowIndex < pProcInd[i + 1]))
            iterProcRank = i;
    }
    if (rowIndex >= pProcInd[procNum - 1])
        iterProcRank = procNum - 1;
    iterPivotPos = rowIndex - pProcInd[iterProcRank];
}

// back substitution
void ParallelBackSubstitution(double *pProcRows, double *pProcVector, double *pProcResult, int size, int rowNum)
{
    int iterProcRank;  // rank of the process with the current pivot row
    int iterPivotPos;  // position of the pivot row of the process
    double iterResult; // calculated value of the current unknown
    double val;
    // the iterations of the back substitution

    for (int i = size - 1; i >= 0; i--)
    {
        // calculating the rank of the process, which holds the pivot row
        FindBackPivotRow(pParallelPivotPos[i], iterProcRank, iterPivotPos);

        // calculating the unknown
        if (procRank == iterProcRank)
        {
            iterResult = pProcVector[iterPivotPos] /
                         pProcRows[iterPivotPos * size + i];
            pProcResult[iterPivotPos] = iterResult;
        }

        // broadcasting the value of the current unknown
        MPI_Bcast(&iterResult, 1, MPI_DOUBLE, iterProcRank, MPI_COMM_WORLD);

        // updating the values of the vector
        for (int j = 0; j < rowNum; j++)
            if (pProcPivotIter[j] < i)
            {
                val = pProcRows[j * size + i] * iterResult;
                pProcVector[j] = pProcVector[j] - val;
            }
    }
}

// execution of the parallel Gauss algorithm
void ParallelResultCalculation(double *pProcRows, double *pProcVector, double *pProcResult, int size, int rowNum)
{
    // memory alloc
    pParallelPivotPos = new int[size];
    pProcPivotIter = new int[rowNum];
    for (int i = 0; i < rowNum; i++)
        pProcPivotIter[i] = -1;

    // gaussian elimination
    ParallelGaussianElimination(pProcRows, pProcVector, size, rowNum);

    // back substitution
    ParallelBackSubstitution(pProcRows, pProcVector, pProcResult, size, rowNum);
}

// gathering the result vector
void ResultCollection(double *pProcResult, double *pResult)
{
    MPI_Gatherv(pProcResult, pProcNum[procRank], MPI_DOUBLE, pResult, pProcNum, pProcInd, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

// testing the result
void TestResult(double *pMatrix, double *pVector, double *pResult, int size)
{
    /* buffer for storing the vector, that is a result of multiplication
    of the linear system matrix by the vector of unknowns */
    double *pRightPartVector;

    // flag, that shows wheather the right parts vectors are identical or not
    int equal = 0;
    double Accuracy = 1.e-6; // comparison accuracy

    if (procRank == 0)
    {
        pRightPartVector = new double[size];
        for (int i = 0; i < size; i++)
        {
            pRightPartVector[i] = 0;
            for (int j = 0; j < size; j++)
            {
                pRightPartVector[i] +=
                    pMatrix[i * size + j] * pResult[pParallelPivotPos[j]];
            }
        }
        for (int i = 0; i < size; i++)
        {
            if (fabs(pRightPartVector[i] - pVector[i]) > Accuracy)
                equal = 1;
        }

        if (equal == 1)
            printf("The results are NOT identical.");
        else
            printf("The results are identical.");
        delete[] pRightPartVector;
    }
}

// computational process termination
void ProcessTermination(double *pMatrix, double *pVector, double *pResult, double *pProcRows, double *pProcVector, double *pProcResult)
{
    if (procRank == 0)
    {
        delete[] pMatrix;
        delete[] pVector;
        delete[] pResult;
    }
    delete[] pProcRows;
    delete[] pProcVector;
    delete[] pProcResult;
    delete[] pParallelPivotPos;
    delete[] pProcPivotIter;
    delete[] pProcInd;
    delete[] pProcNum;
}

int main(int argc, char *argv[])
{
    double *pMatrix;      // matrix of the linear system
    double *pVector;      // right parts of the linear system
    double *pResult;      // result vector
    double *pProcRows;    // rows of the matrix A
    double *pProcVector;  // block of the vector b
    double *pProcResult;  // block of the vector x
    int size;             // size of the matrix and vectors
    int rowNum;           // number of the matrix rows
    double start, finish, duration;

    int arrSizes[] = {10, 100, 500, 1000, 1500, 2000, 2500, 3000};
    int numTests = sizeof(arrSizes) / sizeof(arrSizes[0]);

    setvbuf(stdout, 0, _IONBF, 0);
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

    if (procRank == 0)
    {
        printf("\nParallel Gauss algorithm for solving linear systems\n");
    }

    for (int t = 0; t < numTests; t++)
    {
        size = arrSizes[t];
        if (procRank == 0)
            printf("\nRunning test for matrix size = %d\n", size);

        ProcessInitialization(pMatrix, pVector, pResult, pProcRows, pProcVector, pProcResult, size, rowNum);

        DataDistribution(pMatrix, pProcRows, pVector, pProcVector, size, rowNum);

        MPI_Barrier(MPI_COMM_WORLD);
        start = MPI_Wtime();

        // execute the parallel Gauss algorithm
        ParallelResultCalculation(pProcRows, pProcVector, pProcResult, size, rowNum);

        // gather result
        ResultCollection(pProcResult, pResult);

        MPI_Barrier(MPI_COMM_WORLD);
        finish = MPI_Wtime();
        duration = finish - start;

        if (procRank == 0)
        {
            // TestResult(pMatrix, pVector, pResult, size);
            printf("\nExecution time: %f\n", duration);
        }

        ProcessTermination(pMatrix, pVector, pResult, pProcRows, pProcVector, pProcResult);
    }

    if (procRank == 0)
        printf("\nAll tests completed.\n");

    MPI_Finalize();
    return 0;
}