#include <iostream>
#include <algorithm>

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
        printf("%7.4f ", pData[i]);
    printf("\n");
}

// memory alloc and setting the initial values
void ProcessInit(double *&pData, int &DataSize)
{
    // do
    // {
    //     printf("Enter the size of data to be sorted: ");
    //     scanf("%d", &DataSize);
    //     if (DataSize <= 0)
    //         printf("Data size should be greater than zero\n");
    // } while (DataSize <= 0);
    // printf("Sorting %d data items\n", DataSize);

    pData = new double[DataSize];
    RandomDataInit(pData, DataSize);
}

// serial bubble sort algorithm
void SerialBubble(double *pData, int DataSize)
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

// sorting by the standard library algorithm
void SerialStdSort(double *pData, int DataSize)
{
    std::sort(pData, pData + DataSize);
}

void ProcessTermination(double *pData, double *testpdata)
{
    delete[] pData;
    delete[] testpdata;
}

int main()
{
    double *pData; // data to be sorted
    int DataSize;  // size of data to be sorted
    clock_t start, finish;
    double duration = 0.0;
    int arrSizes[] = {10, 100, 10000, 20000, 30000, 40000, 50000};
    printf("Serial bubble sort program\n");

    for (int i = 0; i < 7; i++)
    {
        DataSize = arrSizes[i];
        printf("\nChosen data size: %d\n", DataSize);
        
        ProcessInit(pData, DataSize);
        double *testpdata = new double[DataSize];
        std::copy(pData, pData + DataSize, testpdata);

        printf("\nSerial bubble sort:\n");
        start = clock();
        SerialBubble(pData, DataSize);
        finish = clock();
        duration = (finish - start) / double(CLOCKS_PER_SEC);
        printf("Time of execution: %f\n", duration);

        printf("\nSTD sort:\n");
        start = clock();
        SerialStdSort(testpdata, DataSize);
        finish = clock();
        duration = (finish - start) / double(CLOCKS_PER_SEC);
        printf("Time of execution lib alg: %f\n", duration);

        ProcessTermination(pData, testpdata);
    }
    return 0;
}