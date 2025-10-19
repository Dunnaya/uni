#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

int procNum = 0;
int procRank = 0;

void DummyDataInit(double* pMatrix, double* pVector, int size) {
    for (int i = 0; i < size; i++) {
        pVector[i] = 1;
        for (int j = 0; j < size; j++)
            pMatrix[i * size + j] = i;
    }
}

void RandomDataInit(double* pMatrix, double* pVector, int size) {
    srand(unsigned(clock()) + procRank);
    for (int i = 0; i < size; i++) {
        pVector[i] = rand() / double(1000);
        for (int j = 0; j < size; j++)
            pMatrix[i * size + j] = rand() / double(1000);
    }
}

void ProcessInit(double*& pMatrix, double*& pVector, double*& pResult,
                 double*& pProcRows, double*& pProcResult,
                 int size, int& rowNum) {
    int restRows = size;

    setvbuf(stdout, 0, _IONBF, 0); 

    if (size < procNum) {
        if (procRank == 0)
            std::cout << "\n[Warning] Matrix size " << size
                      << " is smaller than number of processes (" << procNum << "). Skipping...\n";
        rowNum = 0;
        return;
    }

    for (int i = 0; i < procRank; i++)
        restRows = restRows - restRows / (procNum - i);
    rowNum = restRows / (procNum - procRank);

    pVector = new double[size];
    pResult = new double[size];
    pProcRows = new double[rowNum * size];
    pProcResult = new double[rowNum];

    if (procRank == 0) {
        pMatrix = new double[size * size];
        RandomDataInit(pMatrix, pVector, size);
    } else {
        pMatrix = nullptr;
    }

    MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

void DataDistribution(double* pMatrix, double* pVector, double* pProcRows, int size, int rowNum) {
    int* pSendNum = new int[procNum];
    int* pSendInd = new int[procNum];
    int restRows = size;
    int tmpRows = size / procNum;

    MPI_Bcast(pVector, size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    pSendNum[0] = tmpRows * size;
    pSendInd[0] = 0;
    for (int i = 1; i < procNum; i++) {
        restRows -= tmpRows;
        tmpRows = restRows / (procNum - i);
        pSendNum[i] = tmpRows * size;
        pSendInd[i] = pSendInd[i - 1] + pSendNum[i - 1];
    }

    MPI_Scatterv(pMatrix, pSendNum, pSendInd, MPI_DOUBLE,
                 pProcRows, pSendNum[procRank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

    delete[] pSendNum;
    delete[] pSendInd;
}

void ResultReplication(double* pResult, double* pProcResult, int size, int rowNum) {
    int* pReceiveNum = new int[procNum];
    int* pReceiveInd = new int[procNum];
    int restRows = size;

    pReceiveInd[0] = 0;
    pReceiveNum[0] = size / procNum;
    for (int i = 1; i < procNum; i++) {
        restRows -= pReceiveNum[i - 1];
        pReceiveNum[i] = restRows / (procNum - i);
        pReceiveInd[i] = pReceiveInd[i - 1] + pReceiveNum[i - 1];
    }

    MPI_Allgatherv(pProcResult, pReceiveNum[procRank], MPI_DOUBLE,
                   pResult, pReceiveNum, pReceiveInd, MPI_DOUBLE, MPI_COMM_WORLD);

    delete[] pReceiveNum;
    delete[] pReceiveInd;
}

void SerialResultCalc(double* pMatrix, double* pVector, double* pResult, int size) {
    for (int i = 0; i < size; i++) {
        pResult[i] = 0;
        for (int j = 0; j < size; j++)
            pResult[i] += pMatrix[i * size + j] * pVector[j];
    }
}

void ParallelResultCalc(double* pProcRows, double* pVector, double* pProcResult, int size, int rowNum) {
    for (int i = 0; i < rowNum; i++) {
        pProcResult[i] = 0;
        for (int j = 0; j < size; j++)
            pProcResult[i] += pProcRows[i * size + j] * pVector[j];
    }
}

void TestResult(double* pMatrix, double* pVector, double* pResult, int size) {
    if (procRank == 0 && pMatrix != nullptr) {
        double* pSerialResult = new double[size];
        SerialResultCalc(pMatrix, pVector, pSerialResult, size);

        bool equal = true;
        for (int i = 0; i < size; i++) {
            if (fabs(pResult[i] - pSerialResult[i]) > 1e-6) {
                equal = false;
                break;
            }
        }

        if (equal)
            std::cout << "Results are equal.\n";
        else
            std::cout << "Results differ!\n";

        delete[] pSerialResult;
    }
}

void ProcessTermination(double* pMatrix, double* pVector, double* pResult,
                        double* pProcRows, double* pProcResult) {
    if (procRank == 0 && pMatrix)
        delete[] pMatrix;
    delete[] pVector;
    delete[] pResult;
    delete[] pProcRows;
    delete[] pProcResult;
}

int main(int argc, char* argv[]) {
    double* pMatrix;
    double* pVector;
    double* pResult;
    double* pProcRows;
    double* pProcResult;
    int rowNum;
    double start, finish, duration;

    int sizes[] = {10, 100, 1000, 2000, 3000, 4000, 5000,
                   6000, 7000, 8000, 9000, 10000};
    int numTests = sizeof(sizes) / sizeof(sizes[0]);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

    if (procRank == 0) {
        std::cout << "\nParallel matrix-vector multiplication program\n";
    }

    for (int t = 0; t < numTests; t++) {
        int size = sizes[t];
        if (procRank == 0)
            std::cout << "\nRunning test for matrix size = " << size << std::endl;

        ProcessInit(pMatrix, pVector, pResult, pProcRows, pProcResult, size, rowNum);
        if (rowNum == 0) continue; // if size < procNum, skip this test

        MPI_Barrier(MPI_COMM_WORLD);
        start = MPI_Wtime();

        DataDistribution(pMatrix, pVector, pProcRows, size, rowNum);
        ParallelResultCalc(pProcRows, pVector, pProcResult, size, rowNum);
        ResultReplication(pResult, pProcResult, size, rowNum);

        MPI_Barrier(MPI_COMM_WORLD);
        finish = MPI_Wtime();
        duration = finish - start;

        TestResult(pMatrix, pVector, pResult, size);
        if (procRank == 0)
            std::cout << "Execution time: " << duration << std::endl;

        ProcessTermination(pMatrix, pVector, pResult, pProcRows, pProcResult);
    }

    if (procRank == 0)
        std::cout << "\nAll tests completed.\n";

    MPI_Finalize();
    return 0;
}