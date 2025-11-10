#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

int procNum = 0; // number of processes
int procRank = 0; // rank of current process
int gridSize = 0; // size of virtual processor grid
int gridCoords[2]; // coordinates of current process in grid
MPI_Comm gridComm; // communicator for virtual processor grid
MPI_Comm rowComm; // communicator for current process row
MPI_Comm colComm; // communicator for current process column

void DummyDataInit(double *pAMatrix, double *pBMatrix, int Size)
{
    for (int i = 0; i < Size; i++)
    {
        for (int j = 0; j < Size; j++)
        {
            pAMatrix[i * Size + j] = 1;
            pBMatrix[i * Size + j] = 1;
        }
    }
}

void RandomDataInit(double *pAMatrix, double *pBMatrix, int Size)
{
    srand(unsigned(clock()));
    for (int i = 0; i < Size; i++)
        for (int j = 0; j < Size; j++)
        {
            pAMatrix[i * Size + j] = rand() / double(1000000);
            pBMatrix[i * Size + j] = rand() / double(1000000);
        }
}

void SerialResultCalculation(double *pAMatrix, double *pBMatrix, double *pCMatrix, int Size)
{

    for (int i = 0; i < Size; i++)
    {
        for (int j = 0; j < Size; j++)
        {
            for (int k = 0; k < Size; k++)
            {
                pCMatrix[i * Size + j] += pAMatrix[i * Size + k] * pBMatrix[k * Size + j];
            }
        }
    }
}

void PrintMatrix(double *pMatrix, int RowCount, int ColCount)
{
    for (int i = 0; i < RowCount; i++)
    {
        for (int j = 0; j < ColCount; j++)
            printf("%9.4f ", pMatrix[i * ColCount + j]);
        printf("\n");
    }
}

// memory alloc and data init
void ProcessInit(double* &pAMatrix, double* &pBMatrix, double* &pCMatrix,
                 double* &pAblock, double* &pBblock, double* &pCblock, double* &pMatrixAblock,
                 int size, int &blockSize)
{
    // blockSize must be size / gridSize and size assumed already broadcasted
    blockSize = size / gridSize;

    // allocate local blocks
    pAblock = new double[blockSize * blockSize];
    pBblock = new double[blockSize * blockSize];
    pCblock = new double[blockSize * blockSize];
    pMatrixAblock = new double[blockSize * blockSize];

    for (int i = 0; i < blockSize * blockSize; ++i)
        pCblock[i] = 0.0;

    if (procRank == 0)
    {
        pAMatrix = new double[size * size];
        pBMatrix = new double[size * size];
        pCMatrix = new double[size * size];
        
        RandomDataInit(pAMatrix, pBMatrix, size);
        for (int i = 0; i < size * size; ++i)
            pCMatrix[i] = 0.0;
    }
    else
    {
        pAMatrix = nullptr;
        pBMatrix = nullptr;
        pCMatrix = nullptr;
    }
}

// creating two-dimensional grid communicator and communicators for each row and each column of the grid
void CreateGridCommunicators()
{
    int DimSize[2];  // num of processes in each dimension of the grid
    int Periodic[2];
    DimSize[0] = gridSize;
    DimSize[1] = gridSize;
    Periodic[0] = 1;
    Periodic[1] = 1;
    int Subdims[2];
    // creation of the Cartesian communicator
    MPI_Cart_create(MPI_COMM_WORLD, 2, DimSize, Periodic, 1, &gridComm);
    // determination of the cartesian coordinates for every process
    MPI_Cart_coords(gridComm, procRank, 2, gridCoords);

    // creating communicators for rows
    Subdims[0] = 0; // dimension is fixed
    Subdims[1] = 1; // dimension belong to the subgrid
    MPI_Cart_sub(gridComm, Subdims, &rowComm);
    // creating communicators for columns
    Subdims[0] = 1; // dimension belong to the subgrid
    Subdims[1] = 0; // dimension is fixed
    MPI_Cart_sub(gridComm, Subdims, &colComm);
}

// checkerboard matrix decomposition
void CheckerboardMatrixScatter(double *pMatrix, double *pMatrixBlock, int Size, int BlockSize)
{
    double *pMatrixRow = new double[BlockSize * Size];
    if (gridCoords[1] == 0)
    {
        MPI_Scatter(pMatrix, BlockSize * Size, MPI_DOUBLE, pMatrixRow, BlockSize * Size, MPI_DOUBLE, 0, colComm);
    }
    for (int i = 0; i < BlockSize; i++)
    {
        MPI_Scatter(&pMatrixRow[i * Size], BlockSize, MPI_DOUBLE, &(pMatrixBlock[i * BlockSize]), BlockSize, MPI_DOUBLE, 0, rowComm);
    }
    delete[] pMatrixRow;
}

// data distribution among the processes
void DataDistribution(double *pAMatrix, double *pBMatrix,
                      double *pMatrixAblock, double *pBblock, int Size, int BlockSize)
{
    CheckerboardMatrixScatter(pAMatrix, pMatrixAblock, Size, BlockSize);
    CheckerboardMatrixScatter(pBMatrix, pBblock, Size, BlockSize);
}

void TestBlocks(double *pBlock, int BlockSize, const char *str)
{
    MPI_Barrier(MPI_COMM_WORLD);
    if (procRank == 0)
    {
        printf("%s \n", str);
        fflush(stdout);
    }
    for (int i = 0; i < procNum; i++)
    {
        if (procRank == i)
        {
            printf("ProcRank = %d \n", procRank);
            PrintMatrix(pBlock, BlockSize, BlockSize);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
}

// broadcasting blocks of the matrix A to process grid rows
void ABlockCommunication(int iter, double *pAblock, double *pMatrixAblock, int BlockSize)
{
    // defining the leading process of the process grid row
    int Pivot = (gridCoords[0] + iter) % gridSize;
    // copying the transmitted block in a separate memory buffer
    if (gridCoords[1] == Pivot)
    {
        for (int i = 0; i < BlockSize * BlockSize; i++)
            pAblock[i] = pMatrixAblock[i];
    }

    MPI_Bcast(pAblock, BlockSize * BlockSize, MPI_DOUBLE, Pivot, rowComm);
}

// cyclic shifting the blocks of the matrix B
void BblockCommunication(double *pBblock, int BlockSize, MPI_Comm ColumnComm)
{
    MPI_Status Status;
    int NextProc = gridCoords[0] + 1;
    if (gridCoords[0] == gridSize - 1)
        NextProc = 0;
    int PrevProc = gridCoords[0] - 1;
    if (gridCoords[0] == 0)
        PrevProc = gridSize - 1;
    MPI_Sendrecv_replace(pBblock, BlockSize * BlockSize, MPI_DOUBLE, NextProc, 0, PrevProc, 0, ColumnComm, &Status);
}

void BlockMultiplication(double *pAblock, double *pBblock, double *pCblock, int Size)
{
    SerialResultCalculation(pAblock, pBblock, pCblock, Size);
}

// parallel exec of the Fox method
void ParallelResultCalc(double *pAblock, double *pMatrixAblock, double *pBblock, double *pCblock, int BlockSize)
{
    for (int iter = 0; iter < gridSize; iter++)
    {
        // sending blocks of matrix A to the process grid rows
        ABlockCommunication(iter, pAblock, pMatrixAblock, BlockSize);

        BlockMultiplication(pAblock, pBblock, pCblock, BlockSize);
        // cyclic shift of blocks of matrix B in process grid columns
        BblockCommunication(pBblock, BlockSize, colComm);
    }
}

void ResultCollection(double *pCMatrix, double *pCblock, int Size, int BlockSize)
{
    double *pResultRow = new double[Size * BlockSize];
    for (int i = 0; i < BlockSize; i++)
    {
        MPI_Gather(&pCblock[i * BlockSize], BlockSize, MPI_DOUBLE, &pResultRow[i * Size], BlockSize, MPI_DOUBLE, 0, rowComm);
    }
    if (gridCoords[1] == 0)
    {
        MPI_Gather(pResultRow, BlockSize * Size, MPI_DOUBLE, pCMatrix, BlockSize * Size, MPI_DOUBLE, 0, colComm);
    }
    delete[] pResultRow;
}

void TestResult(double *pAMatrix, double *pBMatrix, double *pCMatrix, int Size)
{
    double *pSerialResult;
    double Accuracy = 1.e-6;
    int equal = 0; // =1, if the matrices are not equal
    if (procRank == 0)
    {
        pSerialResult = new double[Size * Size];
        for (int i = 0; i < Size * Size; i++)
        {
            pSerialResult[i] = 0;
        }
        SerialResultCalculation(pAMatrix, pBMatrix, pSerialResult, Size);
        for (int i = 0; i < Size * Size; i++)
        {
            if (fabs(pSerialResult[i] - pCMatrix[i]) >= Accuracy)
                equal = 1;
        }
        if (equal == 1)
            printf("The results are NOT identical.");
        else
            printf("The results are identical.");
        delete[] pSerialResult;
    }
}

void ProcessTermination(double *pAMatrix, double *pBMatrix, double *pCMatrix, double *pAblock, double *pBblock, double *pCblock, double *pMatrixAblock)
{
    if (procRank == 0)
    {
        delete[] pAMatrix;
        delete[] pBMatrix;
        delete[] pCMatrix;
    }
    delete[] pAblock;
    delete[] pBblock;
    delete[] pCblock;
    delete[] pMatrixAblock;
}

int main(int argc, char* argv[])
{
    double* pAMatrix = nullptr;
    double* pBMatrix = nullptr;
    double* pCMatrix = nullptr;
    int size;
    int blockSize;
    double* pAblock = nullptr;
    double* pBblock = nullptr;
    double* pCblock = nullptr;
    double* pMatrixAblock = nullptr;
    double start, finish, duration;

    setvbuf(stdout, NULL, _IONBF, 0);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

    gridSize = (int)sqrt((double)procNum);
    if (procNum != gridSize * gridSize)
    {
        if (procRank == 0)
            std::cout << "\nThe number of processes must be a perfect square." << std::endl;
        MPI_Finalize();
        return 0;
    }

    if (procRank == 0)
        std::cout << "Parallel Matrix Multiplication Program\n";

    CreateGridCommunicators();

    // choose test sizes depending on procNum (user requested specific arrays for 4 and 9 procs)
    int *sizes = nullptr;
    int numTests = 0;
    if (procNum == 4) {
        static int s4[] = {12, 100, 500, 1000, 1500, 2000, 2500, 3000};
        sizes = s4; numTests = sizeof(s4)/sizeof(s4[0]);
    } else if (procNum == 9) {
        static int s9[] = {9, 99, 504, 999, 1503, 1998, 2502, 2997};
        sizes = s9; numTests = sizeof(s9)/sizeof(s9[0]);
    } else {
        // default sizes (can be adjusted)
        static int sd[] = {10, 100, 500, 1000, 1500, 2000, 2500, 3000};
        sizes = sd; numTests = sizeof(sd)/sizeof(sd[0]);
    }

    for (int t = 0; t < numTests; ++t)
    {
        size = sizes[t];
        if (procRank == 0)
            std::cout << "\nRunning test for matrix size = " << size << std::endl;

        // check divisibility
        if (size % gridSize != 0)
        {
            if (procRank == 0)
                std::cout << "Warning size " << size << " is not multiple of gridSize " << gridSize << ". Skipping this test.\n";
            continue;
        }

        // initialize data structures (allocated per test)
        ProcessInit(pAMatrix, pBMatrix, pCMatrix, pAblock, pBblock, pCblock, pMatrixAblock, size, blockSize);

        MPI_Barrier(MPI_COMM_WORLD);
        start = MPI_Wtime();

        // distribute data to blocks
        DataDistribution(pAMatrix, pBMatrix, pMatrixAblock, pBblock, size, blockSize);

        // run Fox parallel calc
        ParallelResultCalc(pAblock, pMatrixAblock, pBblock, pCblock, blockSize);

        // gather result into pCMatrix
        ResultCollection(pCMatrix, pCblock, size, blockSize);

        MPI_Barrier(MPI_COMM_WORLD);
        finish = MPI_Wtime();
        duration = finish - start;

        // verify
        TestResult(pAMatrix, pBMatrix, pCMatrix, size);

        if (procRank == 0)
            std::cout << "Execution time: " << duration << std::endl;

        // free memory from this test
        ProcessTermination(pAMatrix, pBMatrix, pCMatrix, pAblock, pBblock, pCblock, pMatrixAblock);

        // reset pointers for next iteration
        pAMatrix = pBMatrix = pCMatrix = nullptr;
        pAblock = pBblock = pCblock = pMatrixAblock = nullptr;
    }

    if (procRank == 0)
        std::cout << "\nAll tests completed.\n";

    MPI_Finalize();
    return 0;
}