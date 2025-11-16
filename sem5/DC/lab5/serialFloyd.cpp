#include <iostream>

void DummyDataInit(int *pMatrix, int size)
{
    for (int i = 0; i < size; i++)
        for (int j = i; j < size; j++)
        {
            if (i == j)
                pMatrix[i * size + j] = 0;
            else if (i == 0)
                pMatrix[i * size + j] = j;
            else
                pMatrix[i * size + j] = -1;
            pMatrix[j * size + i] = pMatrix[i * size + j];
        }
}

void RandomDataInit(int *pMatrix, int size)
{
    int InfinitiesPercent = 50;
    srand((unsigned)time(0));
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            if (i != j)
            {
                if ((rand() % 100) < InfinitiesPercent)
                    pMatrix[i * size + j] = -1;
                else
                    pMatrix[i * size + j] = rand() % 1000000 + 1;
            }
            else
                pMatrix[i * size + j] = 0;
}

void PrintMatrix(int *pMatrix, int RowCount, int ColCount)
{
    for (int i = 0; i < RowCount; i++)
    {
        for (int j = 0; j < ColCount; j++)
            printf("%9d", pMatrix[i * ColCount + j]);
        printf("\n");
    }
}

// memory alloc and setting initial values
void ProcessInit(int *&pMatrix, int &size)
{
    // allocate memory for the adjacency matrix
    pMatrix = new int[size * size];
    // data init
    RandomDataInit(pMatrix, size);
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

void ProcessTermination(int *pMatrix)
{
    delete[] pMatrix;
}

int main()
{
    int *pMatrix; // adjacency matrix
    int size;     // size of adjacency matrix
    time_t start, finish;
    double duration = 0.0;
    int sizesArr[] = {10, 500, 600, 700, 800, 900, 1000};

    printf("Serial Floyd algorithm\n");

    for (int i = 0; i < 7; i++)
    {
        size = sizesArr[i];
        printf("\nChosen size: %d \n", size);
        // Process initialization
        ProcessInit(pMatrix, size);
        // printf("The matrix before Floyd algorithm\n");
        // PrintMatrix(pMatrix, size, size);

        start = clock();
        // Serial Floyd algorithm
        SerialFloyd(pMatrix, size);
        finish = clock();
        // printf("The matrix after Floyd algorithm\n");
        // PrintMatrix(pMatrix, size, size);

        duration = (finish - start) / double(CLOCKS_PER_SEC);
        printf("Time of execution: %f\n", duration);
        
        ProcessTermination(pMatrix);
    }
    return 0;
}