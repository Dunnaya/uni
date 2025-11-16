#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <mpi.h>

int procNum;  // number of available processes
int procRank; // rank of current process

void RandomDataInit(int *pMatrix, int size)
{
    int infinitiesPercent = 50;
    srand((unsigned)time(0));
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            if (i != j)
            {
                if ((rand() % 100) < infinitiesPercent)
                    pMatrix[i * size + j] = -1;
                else
                    pMatrix[i * size + j] = rand() % 1000000 + 1;
            }
            else
                pMatrix[i * size + j] = 0;
}

void PrintMatrix(int *pMatrix, int rowCount, int colCount)
{
    for (int i = 0; i < rowCount; i++)
    {
        for (int j = 0; j < colCount; j++)
            printf("%9d", pMatrix[i * colCount + j]);
        printf("\n");
    }
}

void ParallelPrintMatrix(int *pProcRows, int size, int rowNum)
{
    for (int i = 0; i < procNum; i++)
    {
        if (procRank == i)
        {
            printf("ProcRank = %d\n", procRank);
            printf("Proc rows:\n");
            PrintMatrix(pProcRows, rowNum, size);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
}

int Min(int A, int B)
{
    int Result = (A < B) ? A : B;
    if ((A < 0) && (B >= 0))
        Result = B;
    if ((B < 0) && (A >= 0))
        Result = A;
    if ((A < 0) && (B < 0))
        Result = -1;
    return Result;
}

void SerialFloyd(int *pMatrix, int size)
{
    int t1, t2;
    for (int k = 0; k < size; k++)
        for (int i = 0; i < size; i++)
            for (int j = 0; j < size; j++)
                if ((pMatrix[i * size + k] != -1) &&
                    (pMatrix[k * size + j] != -1))
                {
                    t1 = pMatrix[i * size + j];
                    t2 = pMatrix[i * size + k] + pMatrix[k * size + j];
                    pMatrix[i * size + j] = Min(t1, t2);
                }
}

void CopyMatrix(int *pMatrix, int size, int *pMatrixCopy)
{
    std::copy(pMatrix, pMatrix + size * size, pMatrixCopy);
}

bool CompareMatrices(int *pMatrix1, int *pMatrix2, int size)
{
    return std::equal(pMatrix1, pMatrix1 + size * size, pMatrix2);
}

// testing the data distribution
void TestDistribution(int *pMatrix, int *pProcRows, int size, int rowNum)
{
    MPI_Barrier(MPI_COMM_WORLD);
    if (procRank == 0)
    {
        printf("Initial adjacency matrix:\n");
        PrintMatrix(pMatrix, size, size);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    ParallelPrintMatrix(pProcRows, size, rowNum);
}

// memory alloc and setting initial values
void ProcessInit(int *&pMatrix, int *&pProcRows, int &size, int &rowNum)
{
    // setvbuf(stdout, 0, _IONBF, 0);
    // if (procRank == 0)
    // {
    //     do
    //     {
    //         printf("Enter the number of vertices: ");
    //         scanf("%d", &size);
    //         if (size < procNum)
    //             printf("The number of vertices should be greater than the number of processes\n");
    //     } while (size < procNum);
    //     printf("Using the graph with %d vertices\n", size);
    // }

    // broadcast the number of vertices
    MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // number of rows for each process
    int restRows = size;
    for (int i = 0; i < procRank; i++)
        restRows = restRows - restRows / (procNum - i);
    rowNum = restRows / (procNum - procRank);
    // allocate memory for the current process rows
    pProcRows = new int[size * rowNum];
    if (procRank == 0)
    {
        // allocate memory for the adjacency matrix
        pMatrix = new int[size * size];
        // data initialization
        RandomDataInit(pMatrix, size);
    }
}

// data distribution among the processes
void DataDistribution(int *pMatrix, int *pProcRows, int size, int rowNum)
{
    int *pSendNum;       // number of elements sent to the process
    int *pSendInd;       // index of the first data element sent to the process
    int restRows = size; // number of rows, that haven’t been distributed yet
    // allocate memory for temporary objects
    pSendInd = new int[procNum];
    pSendNum = new int[procNum];
    // define the disposition of the matrix rows for current process
    rowNum = size / procNum;
    pSendNum[0] = rowNum * size;
    pSendInd[0] = 0;
    for (int i = 1; i < procNum; i++)
    {
        restRows -= rowNum;
        rowNum = restRows / (procNum - i);
        pSendNum[i] = rowNum * size;
        pSendInd[i] = pSendInd[i - 1] + pSendNum[i - 1];
    }
    
    MPI_Scatterv(pMatrix, pSendNum, pSendInd, MPI_INT,
                 pProcRows, pSendNum[procRank], MPI_INT, 0, MPI_COMM_WORLD);
    // free allocated memory
    delete[] pSendNum;
    delete[] pSendInd;
}

// row broadcasting among all processes
void RowDistribution(int *pProcRows, int size, int rowNum, int k, int *pRow)
{
    int procRowRank; // process rank with the row k
    int procRowNum;  // process row number
    // finding the process rank with the row k
    int restRows = size;
    int ind = 0;
    int num = size / procNum;
    for (procRowRank = 1; procRowRank < procNum + 1; procRowRank++)
    {
        if (k < ind + num)
            break;
        restRows -= num;
        ind += num;
        num = restRows / (procNum - procRowRank);
    }
    procRowRank = procRowRank - 1;
    procRowNum = k - ind;
    if (procRowRank == procRank)
        // copy the row to pRow array
        std::copy(&pProcRows[procRowNum * size], &pProcRows[(procRowNum + 1) * size], pRow);
    // broadcast row to all processes
    MPI_Bcast(pRow, size, MPI_INT, procRowRank, MPI_COMM_WORLD);
}

void ParallelFloyd(int *pProcRows, int size, int rowNum)
{
    int *pRow = new int[size];
    int t1, t2;
    for (int k = 0; k < size; k++)
    {
        RowDistribution(pProcRows, size, rowNum, k, pRow);
        // update adjacency matrix elements
        for (int i = 0; i < rowNum; i++)
            for (int j = 0; j < size; j++)
                if ((pProcRows[i * size + k] != -1) &&
                    (pRow[j] != -1))
                {
                    t1 = pProcRows[i * size + j];
                    t2 = pProcRows[i * size + k] + pRow[j];
                    pProcRows[i * size + j] = Min(t1, t2);
                }
    }
    delete[] pRow;
}

void ResultCollection(int *pMatrix, int *pProcRows, int size, int rowNum)
{
    int *pReceiveNum;    // number of elements, that current process sends
    int *pReceiveInd;    // offset for storing the data from current process
    int restRows = size; // number of rows, that haven’t been gathered yet
    // allocate memory for temporary objects
    pReceiveNum = new int[procNum];
    pReceiveInd = new int[procNum];
    // determine the disposition of the result data block of current process
    rowNum = size / procNum;
    pReceiveInd[0] = 0;
    pReceiveNum[0] = rowNum * size;
    for (int i = 1; i < procNum; i++)
    {
        restRows -= rowNum;
        rowNum = restRows / (procNum - i);
        pReceiveNum[i] = rowNum * size;
        pReceiveInd[i] = pReceiveInd[i - 1] + pReceiveNum[i - 1];
    }
    // gather the whole matrix on the process 0
    MPI_Gatherv(pProcRows, pReceiveNum[procRank], MPI_INT,
                pMatrix, pReceiveNum, pReceiveInd, MPI_INT, 0, MPI_COMM_WORLD);
    // free allocated memory
    delete[] pReceiveNum;
    delete[] pReceiveInd;
}

void TestResult(int *pMatrix, int *pSerialMatrix, int size)
{
    MPI_Barrier(MPI_COMM_WORLD);
    if (procRank == 0)
    {
        SerialFloyd(pSerialMatrix, size);
        if (!CompareMatrices(pMatrix, pSerialMatrix, size))
        {
            printf("The results are "
                   "NOT identical.\n");
        }
        else
        {
            printf("The results are "
                   "identical\n");
        }
    }
}

void ProcessTermination(int *pMatrix, int *pProcRows)
{
    if (procRank == 0)
        delete[] pMatrix;
    delete[] pProcRows;
}

int main(int argc, char *argv[])
{
    int *pMatrix;   // adjacency matrix
    int size;       // size of adjacency matrix
    int *pProcRows; // process rows
    int rowNum;     // number of process rows
    int *pSerialMatrix = 0;
    double start, finish;
    double duration = 0.0;
    int sizesArr[] = {10, 500, 600, 700, 800, 900, 1000};

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
    if (procRank == 0)
        printf("Parallel Floyd algorithm \n");

    for (int i = 0; i < 7; i++)
    {
        if (procRank == 0)
        {
            size = sizesArr[i];
            printf("\nChosen size: %d \n", size);
        }
        
        ProcessInit(pMatrix, pProcRows, size, rowNum);
        if (procRank == 0)
        {
            // matrix copying
            pSerialMatrix = new int[size * size];
            CopyMatrix(pMatrix, size, pSerialMatrix);
        }
        start = MPI_Wtime();
        
        DataDistribution(pMatrix, pProcRows, size, rowNum);
        ParallelFloyd(pProcRows, size, rowNum);

        finish = MPI_Wtime();
        ResultCollection(pMatrix, pProcRows, size, rowNum);
        duration = finish - start;

        if (procRank == 0)
            printf("Time of execution: %f\n", duration);
        TestResult(pMatrix, pSerialMatrix, size);

        ProcessTermination(pMatrix, pProcRows);
    }
    MPI_Finalize();
}