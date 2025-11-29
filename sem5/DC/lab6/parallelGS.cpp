#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <cmath>
#include <algorithm>
#include <mpi.h>

static int procNum = 0;   // number of the available processes
static int procRank = -1; // rank of the current process

void DummyDataInit(double *pMatrix, int size)
{
    double h = 1.0 / (size - 1);
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

void PrintMatrix(double *pMatrix, int rowCount, int colCount)
{
    for (int i = 0; i < rowCount; i++)
    {
        for (int j = 0; j < colCount; j++)
            printf("%7.4f ", pMatrix[i * colCount + j]);
        printf("\n");
    }
}

// memory alloc and and data initialization
void ProcessInit(double *&pMatrix, double *&pProcRows, int &size, int &rowNum, double &eps)
{
    int restRows; // number of rows, that haven’t been distributed yet

    // // setting the grid size
    // if (ProcRank == 0)
    // {
    //     do
    //     {
    //         printf("\nEnter the grid size: ");
    //         fflush(stdout);
    //         scanf("%d", &Size);

    //         if (Size <= 2)
    //         {
    //             printf("\n Size of grid must be greater than 2! \n");
    //         }

    //         if (Size < ProcNum)
    //         {
    //             printf("Size of grid must be greater than"
    //                    "the number of processes! \n ");
    //         }
    //     } while ((Size <= 2) || (Size < ProcNum));

    //     // Setting the required accuracy
    //     do
    //     {
    //         printf("\nEnter the required accuracy: ");
    //         fflush(stdout);
    //         scanf("%lf", &Eps);
    //         if (Eps <= 0)
    //             printf("\nAccuracy must be greater than 0!\n");
    //     } while (Eps <= 0);
    // }

    MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&eps, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // define the number of matrix rows stored on each process
    restRows = size;
    for (int i = 0; i < procRank; i++)
        restRows = restRows - restRows / (procNum - i);
    rowNum = (restRows - 2) / (procNum - procRank) + 2;

    // memory alloc
    pProcRows = new double[rowNum * size];

    // define the values of initial objects’ elements
    if (procRank == 0)
    {
        // initial matrix exists only on the pivot process
        pMatrix = new double[size * size];

        // values of elements are defined only on the pivot process
        RandomDataInit(pMatrix, size);
    }
}

// distribution of the grid rows among the processes
void DataDistribution(double *pMatrix, double *pProcRows, int size, int rowNum)
{
    int *pSendNum; // number of elements sent to the process
    int *pSendInd; // index of the first data element sent to the process
    int restRows = size;

    // alloc memory for temporary objects
    pSendInd = new int[procNum];
    pSendNum = new int[procNum];

    // define the disposition of the matrix rows for current process
    rowNum = (size - 2) / procNum + 2;
    pSendNum[0] = rowNum * size;
    pSendInd[0] = 0;

    for (int i = 1; i < procNum; i++)
    {
        restRows = restRows - rowNum + 1;
        rowNum = (restRows - 2) / (procNum - i) + 2;
        pSendNum[i] = (rowNum)*size;
        pSendInd[i] = pSendInd[i - 1] + pSendNum[i - 1] - size;
    }

    // scatter the rows
    MPI_Scatterv(pMatrix, pSendNum, pSendInd, MPI_DOUBLE, pProcRows, pSendNum[procRank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
    delete[] pSendInd;
    delete[] pSendNum;
}

// testing the data distribution
void TestDistribution(double *pMatrix, double *pProcRows, int size, int rowNum)
{
    if (procRank == 0)
    {
        printf("Initial Matrix: \n");
        PrintMatrix(pMatrix, size, size);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    for (int i = 0; i < procNum; i++)
    {
        if (procRank == i)
        {
            printf("\nProcRank = %d \n", procRank);
            printf(" Matrix Stripe:\n");
            fflush(stdout);
            PrintMatrix(pProcRows, rowNum, size);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
}

// exchanging the boundary rows of the process stripe
void ExchangeData(double *pProcRows, int size, int rowNum)
{
    MPI_Status status;

    int NextProcNum = (procRank == procNum - 1) ? MPI_PROC_NULL : procRank + 1;
    int PrevProcNum = (procRank == 0) ? MPI_PROC_NULL : procRank - 1;

    // Send to NextProcNum and receive from PrevProcNum
    MPI_Sendrecv(pProcRows + size * (rowNum - 2), size, MPI_DOUBLE, NextProcNum, 4, pProcRows, size, MPI_DOUBLE, PrevProcNum, 4, MPI_COMM_WORLD, &status);

    // Send to PrevProcNum and receive from NextProcNum
    MPI_Sendrecv(pProcRows + size, size, MPI_DOUBLE, PrevProcNum, 5, pProcRows + (rowNum - 1) * size, size, MPI_DOUBLE, NextProcNum, 5, MPI_COMM_WORLD, &status);
}

// exec of the Gauss-Seidel method iteration
double IterationCalculation(double *pProcRows, int size, int rowNum)
{
    double dm, dmax, temp;
    dmax = 0;
    for (int i = 1; i < rowNum - 1; i++)
        for (int j = 1; j < size - 1; j++)
        {
            temp = pProcRows[size * i + j];
            pProcRows[size * i + j] = 0.25 * (pProcRows[size * i + j + 1] +
                                              pProcRows[size * i + j - 1] +
                                              pProcRows[size * (i + 1) + j] +
                                              pProcRows[size * (i - 1) + j]);
            dm = fabs(pProcRows[size * i + j] - temp);
            if (dmax < dm)
                dmax = dm;
        }
    return dmax;
}

// parallel Gauss-Seidel method
void ParallelResultCalc(double *pProcRows, int size, int rowNum, double eps, int &iterations)
{
    double procDelta, delta;
    iterations = 0;
    do
    {
        iterations++;
        // exchanging the boundary rows of the process stripe
        ExchangeData(pProcRows, size, rowNum);
        // the Gauss-Seidel method iteration
        procDelta = IterationCalculation(pProcRows, size, rowNum);
        // calculating the maximum value of the deviation
        MPI_Allreduce(&procDelta, &delta, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    } while (delta > eps);
}

// gathering calculation results
void ResultCollection(double *pMatrix, double *pProcRows, int size, int rowNum)
{
    int *pReceiveNum; // number of elements, that current process sends
    int *pReceiveInd; // index of the first element of the received block
    int restRows = size;

    // alloc memory for temporary objects
    pReceiveNum = new int[procNum];
    pReceiveInd = new int[procNum];

    // define the disposition of the result vector block of current processor
    pReceiveInd[0] = 0;
    rowNum = (size - 2) / procNum + 2;
    pReceiveNum[0] = rowNum * size;

    for (int i = 1; i < procNum; i++)
    {
        restRows = restRows - rowNum + 1;
        rowNum = (restRows - 2) / (procNum - i) + 2;
        pReceiveNum[i] = rowNum * size;
        pReceiveInd[i] = pReceiveInd[i - 1] + pReceiveNum[i - 1] - size;
    }

    // gather the whole result vector on every processor
    MPI_Gatherv(pProcRows, pReceiveNum[procRank], MPI_DOUBLE, pMatrix,
                pReceiveNum, pReceiveInd, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // free memory
    delete[] pReceiveNum;
    delete[] pReceiveInd;
}

// copy the initial data
void CopyData(double *pMatrix, int size, double *pSerialMatrix)
{
    std::copy(pMatrix, pMatrix + size * size, pSerialMatrix);
}

void SerialResultCalculation(double *pMatrixCopy, int size, double eps, int &iter)
{
    double dm, dmax, temp;
    iter = 0;

    do
    {
        dmax = 0;
        for (int i = 1; i < size - 1; i++)
            for (int j = 1; j < size - 1; j++)
            {
                temp = pMatrixCopy[size * i + j];
                pMatrixCopy[size * i + j] = 0.25 * (pMatrixCopy[size * i + j + 1] +
                                                    pMatrixCopy[size * i + j - 1] +
                                                    pMatrixCopy[size * (i + 1) + j] +
                                                    pMatrixCopy[size * (i - 1) + j]);
                dm = fabs(pMatrixCopy[size * i + j] - temp);
                if (dmax < dm)
                    dmax = dm;
            }
        iter++;
    } while (dmax > eps);
}

// testing the computation result
void TestResult(double *pMatrix, double *pSerialMatrix, int size, double eps)
{
    int equal = 0; // =1, if the matrices are not equal
    int iter;

    if (procRank == 0)
    {
        SerialResultCalculation(pSerialMatrix, size, eps, iter);
        for (int i = 0; i < size * size; i++)
        {
            if (fabs(pSerialMatrix[i] - pMatrix[i]) >= eps)
                equal = 1;
            break;
        }

        if (equal == 1)
            printf("The results are NOT identical.\n");
        else
            printf("The results are identical.\n");
    }
}

// computational process termination
void ProcessTermination(double *pMatrix, double *pProcRows)
{
    if (procRank == 0)
        delete[] pMatrix;
    delete[] pProcRows;
}

int main(int argc, char *argv[])
{
    double *pMatrix;       // matrix of the grid
    int size;              // matrix size
    double *pProcRows;     // stripe of the matrix on current process
    int rowNum;            // number of rows in matrix stripe
    double eps;            // requied accuracy
    double *pSerialMatrix; // result of the serial method
    int iterations;
    double start, finish, duration;
    int matrixSizes[] = {10, 100, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000};

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

    for (int i = 0; i < 12; i++)
    {
        size = matrixSizes[i];
        eps = 1;
        if (procRank == 0)
        {
            printf("Chosen size: %d\n", size);
        }

        ProcessInit(pMatrix, pProcRows, size, rowNum, eps);
        start = MPI_Wtime();
        DataDistribution(pMatrix, pProcRows, size, rowNum);

        ParallelResultCalc(pProcRows, size, rowNum, eps, iterations);

        ResultCollection(pMatrix, pProcRows, size, rowNum);
        finish = MPI_Wtime();
        duration = finish - start;

        if (procRank == 0)
        {
            printf("Execution time: %f\n", duration);
            printf("Iterations: %d\n", iterations);
        }

        ProcessTermination(pMatrix, pProcRows);
    }
    MPI_Finalize();
}