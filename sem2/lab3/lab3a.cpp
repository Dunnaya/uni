/* Координати точок в тривимірному просторі (дійсні), сортування за x-y-z, за спаданням
Selection sort
Hoare partition scheme, rightmost pivot
bottom-up */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct point 
{
    double x, y, z;

    point (double x, double y, double z) 
    {
        this -> x = x, 
        this -> y = y, 
        this -> z = z;
    };

    void returnCoordinate ()
    {
        cout << "(" << x << ";" << y << ";" << z << ") \n";
    }

    void returnCoordinateBenchmark ()
    {
        for (int a = 0; a <= 7; a++) {}
    }

    point () {};
};

bool comparePointsX (const point& p1, const point& p2) 
{
    if (p1.x != p2.x) 
    {
        return p1.x > p2.x;
    }
    else return false;
}

bool comparePointsY (const point& p1, const point& p2) 
{
    if (p1.y != p2.y) 
    {
        return p1.y > p2.y;
    }
    else return false;
}

bool comparePointsZ (const point& p1, const point& p2) 
{
    if (p1.z != p2.z) 
    {
        return p1.z > p2.z;
    }
    else return false;
}

int xPartition (vector <point>& points, int low, int high)
{
    point pivot = points[high];
    int i = low - 1;

    for (int j = low; j < high; j++) 
    {
        if (points[j].x >= pivot.x) 
        {
            i++;
            swap (points[i], points[j]);
        }
    }
    swap (points[i + 1], points[high]);
    return i + 1;
}

int yPartition (vector <point>& points, int low, int high)
{
    point pivot = points[high];
    int i = low - 1;

    for (int j = low; j < high; j++) 
    {
        if (points[j].y >= pivot.y) 
        {
            i++;
            swap (points[i], points[j]);
        }
    }
    swap (points[i + 1], points[high]);
    return i + 1;
}

int zPartition (vector <point>& points, int low, int high)
{
    point pivot = points[high];
    int i = low - 1;

    for (int j = low; j < high; j++) 
    {
        if (points[j].z >= pivot.z) 
        {
            i++;
            swap (points[i], points[j]);
        }
    }
    swap (points[i + 1], points[high]);
    return i + 1;
}

void xMerge (vector <point>& points, int l, int m, int r) // l - left, m - mid index, r - right
{
    int n1 = m - l +1; //arr size
    int n2 = r - m;

    vector <point> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = points[l+i];
    
    for (int i = 0; i < n2; i++)
        R[i] = points[m + 1 + i];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) 
    {
        if (L[i].x >= R[j].x) 
        {
            points[k] = L[i];
            i++;
        } 
        else 
        {
            points[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1)
        points[k++] = L[i++];
    while (j < n2)
        points[k++] = R[j++];
}

void yMerge (vector <point>& points, int l, int m, int r)
{
    int n1 = m - l +1;
    int n2 = r - m;

    vector <point> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = points[l+i];
    
    for (int i = 0; i < n2; i++)
        R[i] = points[m + 1 + i];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) 
    {
        if (L[i].y >= R[j].y) 
        {
            points[k] = L[i];
            i++;
        } 
        else 
        {
            points[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1)
        points[k++] = L[i++];
    while (j < n2)
        points[k++] = R[j++];
}

void zMerge (vector <point>& points, int l, int m, int r)
{
    int n1 = m - l +1;
    int n2 = r - m;

    vector <point> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = points[l+i];
    
    for (int i = 0; i < n2; i++)
        R[i] = points[m + 1 + i];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) 
    {
        if (L[i].z >= R[j].z) 
        {
            points[k] = L[i];
            i++;
        } 
        else 
        {
            points[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1)
        points[k++] = L[i++];
    while (j < n2)
        points[k++] = R[j++];
}

void xSelectionSort (vector <point>& points)
{
    for (int i = 0; i < (points.size() - 1); i++)
    {
        int indexmax = i;

        for (int j = i + 1; j < (points.size()); j++)
        {
            if (points[j].x > points[indexmax].x)
            {
                indexmax = j;
            }
        }

        if (indexmax != i)
        {
            swap (points[i], points[indexmax]);
        }
    }
}

void xQuicksort (vector <point>& points, int low, int high)
{
    if (low < high)
    {
        int p = xPartition (points, low, high);
        xQuicksort (points, low, p - 1);
        xQuicksort (points, p + 1, high);
    }
}

void xMergesort (vector <point>& points)
{
    int n = points.size();

    for (int current_size = 1; current_size <= n - 1; current_size *= 2)
    {
        for (int left_start = 0; left_start < n - 1; left_start += 2 * current_size)
        {
            int mid = min (left_start + current_size - 1, n - 1);
            int right_end = min (left_start + 2 * current_size - 1, n - 1);
            xMerge (points, left_start, mid, right_end);
        }
    }
}

void xCombination (vector <point>& points)
{
    int n = 10000; // selection sort for x,y,z - 1/2 sec
    if (points.size() > n)
    {
        xQuicksort(points, 0, points.size() - 1);
    }
    else xSelectionSort(points);
}

void ySelectionSort (vector <point>& points)
{
    for (int i = 0; i < (points.size() - 1); i++)
    {
        int indexmax = i;

        for (int j = i + 1; j < (points.size()); j++)
        {
            if (points[j].y > points[indexmax].y)
            {
                indexmax = j;
            }
        }

        if (indexmax != i)
        {
            swap (points[i], points[indexmax]);
        }
    }
}

void yQuicksort (vector <point>& points, int low, int high)
{
    if (low < high)
    {
        int p = yPartition (points, low, high);
        yQuicksort (points, low, p - 1);
        yQuicksort (points, p + 1, high);
    }
}

void yMergesort (vector <point>& points)
{
    int n = points.size();

    for (int current_size = 1; current_size <= n - 1; current_size *= 2)
    {
        for (int left_start = 0; left_start < n - 1; left_start += 2 * current_size)
        {
            int mid = min (left_start + current_size - 1, n - 1);
            int right_end = min (left_start + 2 * current_size - 1, n - 1);
            yMerge (points, left_start, mid, right_end);
        }
    }
}

void yCombination (vector <point>& points)
{
    int n = 10000; // selection sort for x,y,z - 1/2 sec
    if (points.size() > n)
    {
        yQuicksort(points, 0, points.size() - 1);
    }
    else ySelectionSort(points);
}

void zSelectionSort (vector <point>& points)
{
    for (int i = 0; i < (points.size() - 1); i++)
    {
        int indexmax = i;

        for (int j = i + 1; j < (points.size()); j++)
        {
            if (points[j].z > points[indexmax].z)
            {
                indexmax = j;
            }
        }

        if (indexmax != i)
        {
            swap (points[i], points[indexmax]);
        }
    }
}

void zQuicksort (vector <point>& points, int low, int high)
{
    if (low < high)
    {
        int p = zPartition (points, low, high);
        zQuicksort (points, low, p - 1);
        zQuicksort (points, p + 1, high);
    }
}

void zMergesort (vector <point>& points)
{
    int n = points.size();

    for (int current_size = 1; current_size <= n - 1; current_size *= 2)
    {
        for (int left_start = 0; left_start < n - 1; left_start += 2 * current_size)
        {
            int mid = min (left_start + current_size - 1, n - 1);
            int right_end = min (left_start + 2 * current_size - 1, n - 1);
            zMerge (points, left_start, mid, right_end);
        }
    }
}

void zCombination (vector <point>& points)
{
    int n = 10000; // selection sort for x,y,z - 1/2 sec
    if (points.size() > n)
    {
        zQuicksort(points, 0, points.size() - 1);
    }
    else zSelectionSort(points);
}

void demoMode() 
{
    vector <point> points;
    for (int j = 0; j < 10; j++)
    {
        double x = rand()% 1000 / 100.0;
        double y = rand()% 1000 / 100.0;
        double z = rand()% 1000 / 100.0;

        point point (x, y, z);
        points.push_back(point);
    }

    cout << "Random coordinates:\n";
    for (int i = 0; i < points.size(); i++)
    {
        points[i].returnCoordinate();
    }

    cout << "\nSelection sort by x:\n";
    xSelectionSort(points);
    for (int i = 0; i < points.size(); i++)
    {
        points[i].returnCoordinate();
    }

    cout << "\nSelection sort by y:\n";
    ySelectionSort(points);
    for (int i = 0; i < points.size(); i++)
    {
        points[i].returnCoordinate();
    }

    cout << "\nSelection sort by z:\n";
    zSelectionSort(points);
    for (int i = 0; i < points.size(); i++)
    {
        points[i].returnCoordinate();
    }

    cout << "\nQuick sort by x:\n";
    xQuicksort(points, 0, points.size() - 1);
    for (int i = 0; i < points.size(); i++)
    {
        points[i].returnCoordinate();
    }

    cout << "\nQuick sort by y:\n";
    yQuicksort(points, 0, points.size() - 1);
    for (int i = 0; i < points.size(); i++)
    {
        points[i].returnCoordinate();
    }

    cout << "\nQuick sort by z:\n";
    zQuicksort(points, 0, points.size() - 1);
    for (int i = 0; i < points.size(); i++)
    {
        points[i].returnCoordinate();
    }

    cout << "\nMerge sort by x:\n";
    xMergesort(points);
    for (int i = 0; i < points.size(); i++)
    {
        points[i].returnCoordinate();
    }

    cout << "\nMerge sort by y:\n";
    yMergesort(points);
    for (int i = 0; i < points.size(); i++)
    {
        points[i].returnCoordinate();
    }

    cout << "\nMerge sort by z:\n";
    zMergesort(points);
    for (int i = 0; i < points.size(); i++)
    {
        points[i].returnCoordinate();
    }

    cout << "\nCombined algorithm by x:\n";
    xCombination(points);
    for (int i = 0; i < points.size(); i++)
    {
        points[i].returnCoordinate();
    }

    cout << "\nCombined algorithm by y:\n";
    yCombination(points);
    for (int i = 0; i < points.size(); i++)
    {
        points[i].returnCoordinate();
    }

    cout << "\nCombined algorithm by z:\n";
    zCombination(points);
    for (int i = 0; i < points.size(); i++)
    {
        points[i].returnCoordinate();
    }

    cout << "\nstd::sort by x:\n";
    std::sort(points.begin(), points.end(), comparePointsX);
    for (int i = 0; i < points.size(); i++)
    {
        points[i].returnCoordinate();
    }

    cout << "\nstd::sort by y:\n";
    std::sort(points.begin(), points.end(), comparePointsY);
    for (int i = 0; i < points.size(); i++)
    {
        points[i].returnCoordinate();
    }

    cout << "\nstd::sort by z:\n";
    std::sort(points.begin(), points.end(), comparePointsZ);
    for (int i = 0; i < points.size(); i++)
    {
        points[i].returnCoordinate();
    }
}

void benchmark() 
{   
    int n;
    vector <point> points;
    cout << "Enter N: ";
    cin >> n;

    for (int j = 0; j < n; j++)
    {
        double x = rand()% 1000 / 100.0;
        double y = rand()% 1000 / 100.0;
        double z = rand()% 1000 / 100.0;

        point point (x, y, z);
        points.push_back(point);
    }

    auto startSelectionX = chrono::high_resolution_clock::now();
    xSelectionSort(points);
    auto endSelectionX = chrono::high_resolution_clock::now();
    auto durationSelectionX = chrono::duration_cast<chrono::milliseconds>(endSelectionX - startSelectionX);
    cout << "Selection sort by x: " << durationSelectionX.count() << endl;

    auto startSelectionY = chrono::high_resolution_clock::now();
    ySelectionSort(points);
    auto endSelectionY = chrono::high_resolution_clock::now();
    auto durationSelectionY = chrono::duration_cast<chrono::milliseconds>(endSelectionY - startSelectionY);
    cout << "Selection sort by y: " << durationSelectionY.count() << endl;

    auto startSelectionZ = chrono::high_resolution_clock::now();
    zSelectionSort(points);
    auto endSelectionZ = chrono::high_resolution_clock::now();
    auto durationSelectionZ = chrono::duration_cast<chrono::milliseconds>(endSelectionZ - startSelectionZ);
    cout << "Selection sort by z: " << durationSelectionZ.count() << endl;

    auto startQuickX = chrono::high_resolution_clock::now();
    xQuicksort(points, 0, points.size() - 1);
    auto endQuickX = chrono::high_resolution_clock::now();
    auto durationQuickX = chrono::duration_cast<chrono::milliseconds>(endQuickX - startQuickX);
    cout << "Quicksort by x: " << durationQuickX.count() << endl;

    auto startQuickY = chrono::high_resolution_clock::now();
    yQuicksort(points, 0, points.size() - 1);
    auto endQuickY = chrono::high_resolution_clock::now();
    auto durationQuickY = chrono::duration_cast<chrono::milliseconds>(endQuickY - startQuickY);
    cout << "Quicksort by y: " << durationQuickY.count() << endl;

    auto startQuickZ = chrono::high_resolution_clock::now();
    zQuicksort(points, 0, points.size() - 1);
    auto endQuickZ = chrono::high_resolution_clock::now();
    auto durationQuickZ = chrono::duration_cast<chrono::milliseconds>(endQuickZ - startQuickZ);
    cout << "Quicksort by z: " << durationQuickZ.count() << endl;

    auto startMergeX = chrono::high_resolution_clock::now();
    xMergesort(points);
    auto endMergeX = chrono::high_resolution_clock::now();
    auto durationMergeX = chrono::duration_cast<chrono::milliseconds>(endMergeX - startMergeX);
    cout << "Mergesort by x: " << durationMergeX.count() << endl;

    auto startMergeY = chrono::high_resolution_clock::now();
    yMergesort(points);
    auto endMergeY = chrono::high_resolution_clock::now();
    auto durationMergeY = chrono::duration_cast<chrono::milliseconds>(endMergeY - startMergeY);
    cout << "Mergesort by y: " << durationMergeY.count() << endl;

    auto startMergeZ = chrono::high_resolution_clock::now();
    zMergesort(points);
    auto endMergeZ = chrono::high_resolution_clock::now();
    auto durationMergeZ = chrono::duration_cast<chrono::milliseconds>(endMergeZ - startMergeZ);
    cout << "Mergesort by z: " << durationMergeZ.count() << endl;

    auto startCombX = chrono::high_resolution_clock::now();
    xCombination(points);
    auto endCombX = chrono::high_resolution_clock::now();
    auto durationCombX = chrono::duration_cast<chrono::milliseconds>(endCombX - startCombX);
    cout << "Combined algorithm by x: " << durationCombX.count() << endl;

    auto startCombY = chrono::high_resolution_clock::now();
    yCombination(points);
    auto endCombY = chrono::high_resolution_clock::now();
    auto durationCombY = chrono::duration_cast<chrono::milliseconds>(endCombY - startCombY);
    cout << "Combined algorithm by y: " << durationCombY.count() << endl;

    auto startCombZ = chrono::high_resolution_clock::now();
    zCombination(points);
    auto endCombZ = chrono::high_resolution_clock::now();
    auto durationCombZ = chrono::duration_cast<chrono::milliseconds>(endCombZ - startCombZ);
    cout << "Combined algorithm by z: " << durationCombZ.count() << endl;

    auto startSortX = chrono::high_resolution_clock::now();
    std::sort(points.begin(), points.end(), comparePointsX);
    auto endSortX = chrono::high_resolution_clock::now();
    auto durationSortX = chrono::duration_cast<chrono::milliseconds>(endSortX - startSortX);
    cout << "std::sort by x: " << durationSortX.count() << endl;

    auto startSortY = chrono::high_resolution_clock::now();
    std::sort(points.begin(), points.end(), comparePointsY);
    auto endSortY = chrono::high_resolution_clock::now();
    auto durationSortY = chrono::duration_cast<chrono::milliseconds>(endSortY - startSortY);
    cout << "std::sort by y: " << durationSortY.count() << endl;

    auto startSortZ = chrono::high_resolution_clock::now();
    std::sort(points.begin(), points.end(), comparePointsZ);
    auto endSortZ = chrono::high_resolution_clock::now();
    auto durationSortZ = chrono::duration_cast<chrono::milliseconds>(endSortZ - startSortZ);
    cout << "std::sort by z: " << durationSortZ.count() << endl;
}

int main () 
{
    int choice;

    cout << "Modes:\n";
    cout << "1. Demo\n";
    cout << "2. Benchmark\n";
    cout << "Select mode: ";
    cin >> choice;

    switch(choice)
    {
        case 1:
        {
            demoMode();

            break;
        }

        case 2:
        {
            benchmark();

            break;
        }

        default:
        cout << "Invalid choice.\n";
        break;
    }
    return 0;
}
