/*Побудова оберненої матриці методом Гауса-Жордана*/
#include <iostream>
#include <iomanip>
#include <cmath>
using namespace std;

typedef double T;

void printMatrix(T** matrix, int n) 
{
    const double epsilon = 1e-10;  //value to compare with 0

    for (int i = 0; i < n; ++i) 
    {
        for (int j = 0; j < n; ++j) 
        {
            //if the element is tiny -> output 0
            if (fabs(matrix[i][j]) < epsilon) 
                cout << setw(5) << setprecision(2) << 0 << " ";
            else
                cout << setw(5) << setprecision(2) << matrix[i][j] << " ";
        }

        cout << endl;
    }
}

bool inverseMatrix(T** A, T** invMatrix, int n) 
{
    //creating the expanded matrix [A|I]
    T** augmented = new T*[n];

    for (int i = 0; i < n; ++i) 
    {
        augmented[i] = new T[2*n];

        for (int j = 0; j < n; ++j) 
        {
            augmented[i][j] = A[i][j];     //copying elements from A
            augmented[i][j+n] = (i == j) ? 1.0 : 0.0;  //and forming a unit matrix
        }
    }

    for (int i = 0; i < n; ++i) 
    {
        //if there is 0 on the diagonal, we look for a line to replace
        if (augmented[i][i] == 0) 
        {
            bool found = false;

            for (int k = i + 1; k < n; ++k) 
            {
                if (augmented[k][i] != 0) 
                {
                    swap(augmented[i], augmented[k]);
                    found = true;
                    break;
                }
            }

            if (!found) 
            {
                cout << "The matrix is ​​degenerate and has no inverse." << endl;
                return false;
            }
        }

        //dividing the current line into a diagonal element
        T diagElement = augmented[i][i];
        for (int j = 0; j < 2 * n; ++j) 
        {
            augmented[i][j] /= diagElement;
        }

        //subtracting the current line from the others to get 0's under the diagonal
        for (int k = 0; k < n; ++k) 
        {
            if (k != i) 
            {
                T factor = augmented[k][i];

                for (int j = 0; j < 2 * n; ++j) 
                {
                    augmented[k][j] -= factor * augmented[i][j];
                }
            }
        }
    }

    //copying the right part of the extended matrix (inverse matrix)
    for (int i = 0; i < n; ++i) 
    {
        for (int j = 0; j < n; ++j) 
        {
            invMatrix[i][j] = augmented[i][j + n];
        }
    }

    //clearing the memory
    for (int i = 0; i < n; ++i) 
    {
        delete[] augmented[i];
    }

    delete[] augmented;

    return true;
}

int main() 
{
    int n;

    cout << "Enter the matrix size: ";
    cin >> n;

    //allocation of memory for matrices
    T** A = new T*[n];
    T** invMatrix = new T*[n];
    for (int i = 0; i < n; ++i) 
    {
        A[i] = new T[n];
        invMatrix[i] = new T[n];
    }

    cout << "Enter the elements of matrix A:" << endl;
    for (int i = 0; i < n; ++i) 
    {
        for (int j = 0; j < n; ++j) 
        {
            cin >> A[i][j];
        }
    }

    if (inverseMatrix(A, invMatrix, n)) 
    {
        cout << "Inverse matrix:" << endl;
        printMatrix(invMatrix, n);
    } 
    else
        cout << "Failed to inverse matrix." << endl;

    //clearing the memory
    for (int i = 0; i < n; ++i) 
    {
        delete[] A[i];
        delete[] invMatrix[i];
    }

    delete[] A;
    delete[] invMatrix;

    return 0;
}