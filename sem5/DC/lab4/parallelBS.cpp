#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <mpi.h>

using namespace std;

int ProcNum = 0;   // numb of available processes
int ProcRank = -1; // rank of current process

void DummyDataInit(double *&pData, int &DataSize)
{
    for (int i = 0; i < DataSize; i++)
        pData[i] = DataSize - i;
}

void RandomDataInit(double *&pData, int &DataSize)
{
    srand((unsigned)time(0));
    for (int i = 0; i < DataSize; i++)
        pData[i] = double(rand()) / RAND_MAX * 1000;
}

void PrintData(double *pData, int DataSize)
{
    for (int i = 0; i < DataSize; i++)
        printf("%7.2f ", pData[i]);
    printf("\n");
}

void ParallelPrintData(double *pProcData, int BlockSize)
{
    for (int i = 0; i < ProcNum; i++)
    {
        if (ProcRank == i)
        {
            printf("ProcRank = %d\n", ProcRank);
            printf("Proc sorted data:\n");
            PrintData(pProcData, BlockSize);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
}

// serial bubble sort algorithm
void SerialBubbleSort(double *pData, int DataSize)
{
    double Tmp;
    for (int i = 1; i < DataSize; i++)
        for (int j = 0; j < DataSize - i; j++)
            if (pData[j] > pData[j + 1])
            {
                Tmp = pData[j];
                pData[j] = pData[j + 1];
                pData[j + 1] = Tmp;
            }
}

// copying the sorted data
void CopyData(double *pData, int DataSize, double *pDataCopy)
{
    copy(pData, pData + DataSize, pDataCopy);
}

// comparing the data
bool CompareData(double *pData1, double *pData2, int DataSize)
{
    return equal(pData1, pData1 + DataSize, pData2);
}

// memory alloc and setting the initial values
void ProcessInit(double *&pData, int &DataSize, double *&pProcData, int &BlockSize)
{
    setvbuf(stdout, 0, _IONBF, 0);
    // if (ProcRank == 0)
    // {
    //     do
    //     {
    //         printf("Enter the size of data to be sorted: ");
    //         scanf("%d", &DataSize);
    //         if (DataSize < ProcNum)
    //             printf("Data size should be greater than number of processes\n");
    //     } while (DataSize < ProcNum);
    //     printf("Sorting %d data items\n", DataSize);
    // }
    // Broadcasting the data size
    MPI_Bcast(&DataSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int RestData = DataSize;
    for (int i = 0; i < ProcRank; i++)
        RestData -= RestData / (ProcNum - i);

    BlockSize = RestData / (ProcNum - ProcRank);
    pProcData = new double[BlockSize];

    if (ProcRank == 0)
    {
        pData = new double[DataSize];
        RandomDataInit(pData, DataSize);
    }
}

// data distribution among the processes
void DataDistribution(double *pData, int DataSize, double *pProcData, int BlockSize)
{
    // alloc memory for temporary objects
    int *pSendInd = new int[ProcNum];
    int *pSendNum = new int[ProcNum];
    int RestData = DataSize;
    int CurrentSize = DataSize / ProcNum;
    pSendNum[0] = CurrentSize;
    pSendInd[0] = 0;
    for (int i = 1; i < ProcNum; i++)
    {
        RestData -= CurrentSize;
        CurrentSize = RestData / (ProcNum - i);
        pSendNum[i] = CurrentSize;
        pSendInd[i] = pSendInd[i - 1] + pSendNum[i - 1];
    }
    MPI_Scatterv(pData, pSendNum, pSendInd, MPI_DOUBLE, pProcData, pSendNum[ProcRank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
    // free memory
    delete[] pSendNum;
    delete[] pSendInd;
}

// testing the data distribution
void TestDistribution(double *pData, int DataSize, double *pProcData, int BlockSize)
{
    MPI_Barrier(MPI_COMM_WORLD);
    if (ProcRank == 0)
    {
        printf("Initial data:\n");
        PrintData(pData, DataSize);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    for (int i = 0; i < ProcNum; i++)
    {
        if (ProcRank == i)
        {
            printf("ProcRank = %d\n", ProcRank);
            printf("Block:\n");
            PrintData(pProcData, BlockSize);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
}

// data exchange between the neighboring processes
void ExchangeData(double *pProcData, int BlockSize, int DualRank, double *pDualData, int DualBlockSize)
{
    MPI_Status status;
    MPI_Sendrecv(pProcData, BlockSize, MPI_DOUBLE, DualRank, 0, pDualData, DualBlockSize, MPI_DOUBLE, DualRank, 0, MPI_COMM_WORLD, &status);
}

// parallel bubble sort algorithm
void ParallelBubble(double *pProcData, int BlockSize)
{
    // local sorting on each process
    SerialBubbleSort(pProcData, BlockSize);
    enum split_mode
    {
        KeepFirstHalf,
        KeepSecondHalf
    };

    int Offset;
    split_mode SplitMode = KeepFirstHalf;

    for (int i = 0; i < ProcNum; i++)
    {
        if ((i % 2) == 1)
        {
            if ((ProcRank % 2) == 1)
            {
                Offset = 1;
                SplitMode = KeepFirstHalf;
            }
            else
            {
                Offset = -1;
                SplitMode = KeepSecondHalf;
            }
        }
        else
        {
            if ((ProcRank % 2) == 1)
            {
                Offset = -1;
                SplitMode = KeepSecondHalf;
            }
            else
            {
                Offset = 1;
                SplitMode = KeepFirstHalf;
            }
        }
        if ((ProcRank == ProcNum - 1) && (Offset == 1))
            continue;
        if ((ProcRank == 0) && (Offset == -1))
            continue;
        MPI_Status status;
        int DualBlockSize;
        MPI_Sendrecv(&BlockSize, 1, MPI_INT, ProcRank + Offset, 0, &DualBlockSize, 1, MPI_INT, ProcRank + Offset, 0, MPI_COMM_WORLD, &status);
        double *pDualData = new double[DualBlockSize];
        double *pMergedData = new double[BlockSize + DualBlockSize];
        ExchangeData(pProcData, BlockSize, ProcRank + Offset, pDualData, DualBlockSize);
        // data merging
        std::merge(pProcData, pProcData + BlockSize, pDualData, pDualData + DualBlockSize, pMergedData);
        if (SplitMode == KeepFirstHalf)
            std::copy(pMergedData, pMergedData + BlockSize, pProcData);
        else
            std::copy(pMergedData + DualBlockSize, pMergedData + BlockSize + DualBlockSize, pProcData);
        delete[] pDualData;
        delete[] pMergedData;
    }
}

void DataCollection(double *pData, int DataSize, double *pProcData, int BlockSize)
{
    // alloc memory for temporary objects
    int *pReceiveNum = new int[ProcNum];
    int *pReceiveInd = new int[ProcNum];
    int RestData = DataSize;
    pReceiveInd[0] = 0;
    pReceiveNum[0] = DataSize / ProcNum;
    for (int i = 1; i < ProcNum; i++)
    {
        RestData -= pReceiveNum[i - 1];
        pReceiveNum[i] = RestData / (ProcNum - i);
        pReceiveInd[i] = pReceiveInd[i - 1] + pReceiveNum[i - 1];
    }
    MPI_Gatherv(pProcData, BlockSize, MPI_DOUBLE, pData, pReceiveNum, pReceiveInd, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    // free memory
    delete[] pReceiveNum;
    delete[] pReceiveInd;
}

// testing the result of parallel bubble sort
void TestResult(double *pData, double *pSerialData, int DataSize)
{
    MPI_Barrier(MPI_COMM_WORLD);
    if (ProcRank == 0)
    {
        SerialBubbleSort(pSerialData, DataSize);
        if (!CompareData(pData, pSerialData, DataSize))
        {
            printf("The results of serial and parallel algorithms are NOT identical. Check your code\n");
        }
        else
        {
            printf("The results of serial and parallel algorithms are identical\n");
        }
    }
}

void ProcessTermination(double *pData, double *pProcData)
{
    if (ProcRank == 0)
        delete[] pData;
    delete[] pProcData;
}

int main(int argc, char *argv[])
{
    double *pData = 0;
    double *pProcData = 0;
    int DataSize = 0;
    int BlockSize = 0;
    double *pSerialData = 0;
    double start, finish, duration;
    int arrSizes[] = {10, 100, 10000, 20000, 30000, 40000, 50000};
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

    if (ProcRank == 0)
        printf("Parallel bubble sort program\n");

    for (int i = 0; i < 7; i++)
    {
        if (ProcRank == 0)
        {
            DataSize = arrSizes[i];
            printf("\nChosen data size: %d\n", DataSize);
        }
        ProcessInit(pData, DataSize, pProcData, BlockSize);

        if (ProcRank == 0)
        {
            // data copying
            pSerialData = new double[DataSize];
            CopyData(pData, DataSize, pSerialData);
        }

        start = MPI_Wtime();
        // data distribution among processes
        DataDistribution(pData, DataSize, pProcData, BlockSize);
        // testing the data distribution
        // TestDistribution(pData, DataSize, pProcData, BlockSize);

        // parallel bubble sort
        ParallelBubble(pProcData, BlockSize);
        // ParallelPrintData(pProcData, BlockSize);
        
        DataCollection(pData, DataSize, pProcData, BlockSize);
        finish = MPI_Wtime();
        // TestResult(pData, pSerialData, DataSize);
        duration = finish - start;
        if (ProcRank == 0)
            printf("Time of execution: %f\n", duration);
            
        ProcessTermination(pData, pProcData);
        if (ProcRank == 0)
            delete[] pSerialData;
    }
    MPI_Finalize();
}