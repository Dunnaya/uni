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

void DummyDataInit(double *pAMatrix, double *pBMatrix, int size)
{
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
        {
            pAMatrix[i * size + j] = 1.0;
            pBMatrix[i * size + j] = 1.0;
        }
}

void RandomDataInit(double *pAMatrix, double *pBMatrix, int size)
{
    srand(unsigned(clock()) + procRank);
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
        {
            pAMatrix[i * size + j] = rand() / double(RAND_MAX) * 10.0;
            pBMatrix[i * size + j] = rand() / double(RAND_MAX) * 10.0;
        }
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

void serialResultCalc(double *pAMatrix, double *pBMatrix, double *pCMatrix, int size)
{
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
        {
            double s = 0.0;
            for (int k = 0; k < size; k++)
                s += pAMatrix[i * size + k] * pBMatrix[k * size + j];
            pCMatrix[i * size + j] = s;
        }
}

void BlockMultiplication(double *pAblock, double *pBblock, double *pCblock, int size)
{
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
        {
            double s = 0.0;
            for (int k = 0; k < size; ++k)
                s += pAblock[i * size + k] * pBblock[k * size + j];
            pCblock[i * size + j] += s;
        }
}

void createGridCommunicators()
{
    // create grid communicator
    int dimSize[2]; // num of process in each dimension of grid
    int periodic[2]; // =1 if grid is periodic in corresponding dimension
    int subdims[2];

    dimSize[0] = gridSize;
    dimSize[1] = gridSize;
    periodic[0] = 0;
    periodic[1] = 0;

    // creation of the Cartesian communicator
    MPI_Cart_create(MPI_COMM_WORLD, 2, dimSize, periodic, 1, &gridComm);

    // determination of current process coordinates in grid
    MPI_Cart_coords(gridComm, procRank, 2, gridCoords);

    // create row communicator (varying columns)
    subdims[0] = 0;
    subdims[1] = 1;
    MPI_Cart_sub(gridComm, subdims, &rowComm);

    // create column communicator (varying rows)
    subdims[0] = 1;
    subdims[1] = 0;
    MPI_Cart_sub(gridComm, subdims, &colComm);
}

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

// Scatter matrix in checkerboard to blocks
void CheckerboardMatrixScatter(double* pMatrix, double* pMatrixBlock, int size, int blockSize)
{
    // temporary buffer for a group of rows that belong to processes in a column
    double* matrixRow = new double [blockSize * size];

    // first, each process in column 0 receives its block of rows
    if (gridCoords[1] == 0)
        MPI_Scatter(pMatrix, blockSize * size, MPI_DOUBLE, matrixRow, blockSize * size, MPI_DOUBLE, 0, colComm);

    // then, each row communicator scatters subblocks into final block
    for (int i = 0; i < blockSize; i++)
        MPI_Scatter(&matrixRow[i * size], blockSize, MPI_DOUBLE, &pMatrixBlock[i * blockSize], blockSize, MPI_DOUBLE, 0, rowComm);

    delete[] matrixRow;
}

void DataDistribution(double* pAMatrix, double* pBMatrix, double* pAblock, double* pBblock, double* pMatrixAblock, int size, int blockSize)
{
    // scatter A and B into pAblock and pBblock (each process gets its block)
    CheckerboardMatrixScatter(pAMatrix, pAblock, size, blockSize);
    CheckerboardMatrixScatter(pBMatrix, pBblock, size, blockSize);

    // Save a copy of the originally assigned A-block to use as "matrixAblock" in the cyclic broadcasts
    for (int i = 0; i < blockSize * blockSize; ++i)
        pMatrixAblock[i] = pAblock[i];
}

void ResultCollection(double* pCMatrix, double* pCblock, int size, int blockSize)
{
    double* pResultRow = new double [size * blockSize];
    for (int i = 0; i < blockSize; i++)
        MPI_Gather(&pCblock[i * blockSize], blockSize, MPI_DOUBLE, &pResultRow[i * size], blockSize, MPI_DOUBLE, 0, rowComm);

    if (gridCoords[1] == 0)
        MPI_Gather(pResultRow, blockSize * size, MPI_DOUBLE, pCMatrix, blockSize * size, MPI_DOUBLE, 0, colComm);

    delete[] pResultRow;
}

// broadcasting blocks of the matrix A to process grid rows
void ABlockCommunication(int iter, double* pAblock, double* pMatrixAblock, int blockSize)
{
    int pivot = (gridCoords[0] + iter) % gridSize;
    if (gridCoords[1] == pivot)
    {
        // the process that owns the pivot's block sets pAblock = its local A block (the copy from original matrix)
        for (int i = 0; i < blockSize * blockSize; i++)
            pAblock[i] = pMatrixAblock[i];
    }

    // block broadcasting along the row communicator from pivot
    MPI_Bcast(pAblock, blockSize * blockSize, MPI_DOUBLE, pivot, rowComm);
}

void BBlockCommunication(double* &pBblock, int blockSize)
{
    MPI_Status status;
    int nextProc = gridCoords[0] + 1;
    if (gridCoords[0] == gridSize - 1) nextProc = 0;

    int prevProc = gridCoords[0] - 1;
    if (gridCoords[0] == 0) prevProc = gridSize - 1;

    MPI_Sendrecv_replace(pBblock, blockSize * blockSize, MPI_DOUBLE, nextProc, 0, prevProc, 0, colComm, &status);
}

void ParallelResultCalc(double* pAblock, double* pMatrixAblock, double* pBblock, double* pCblock, int blockSize)
{
    for (int iter = 0; iter < gridSize; iter++)
    {
        ABlockCommunication(iter, pAblock, pMatrixAblock, blockSize);
        BlockMultiplication(pAblock, pBblock, pCblock, blockSize);
        BBlockCommunication(pBblock, blockSize);
    }
}

void TestResult(double* pAMatrix, double* pBMatrix, double* pCMatrix, int size)
{
    if (procRank == 0)
    {
        double* pSerialResult = new double[size * size];
        serialResultCalc(pAMatrix, pBMatrix, pSerialResult, size);

        double accuracy = 1.e-6;
        bool equal = true;
        for (int i = 0; i < size * size; i++)
        {
            if (fabs(pSerialResult[i] - pCMatrix[i]) >= accuracy)
            {
                equal = false;
                break;
            }
        }

        if (equal)
            std::cout << "The results are identical." << std::endl;
        else
            std::cout << "The results are NOT identical." << std::endl;

        delete[] pSerialResult;
    }
}

void ProcessTermination(double* pAMatrix, double* pBMatrix, double* pCMatrix,
                        double* pAblock, double* pBblock, double* pCblock, double* pMatrixAblock)
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

    createGridCommunicators();

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
        DataDistribution(pAMatrix, pBMatrix, pAblock, pBblock, pMatrixAblock, size, blockSize);

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