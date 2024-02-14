/*Реалізувати структуру даних «стек» з наступними операціями: 
1) create_empty – створення пустого стеку; 
2) push –  додавання елементу до стеку; 
3) pop –  вилучення елементу зі стеку; 
4) peek – значення верхнього елементу стеку (без видалення); 
5) is_empty – перевірка на пустоту. 
В структурі зберігаються точки у тривимірному просторі (дійсні координати)*/

#include <iostream>
#include <vector>
using namespace std;

struct point 
{
    point* next;

    double x, y, z;

    point (double x, double y, double z) 
    {
        this -> x = x, 
        this -> y = y, 
        this -> z = z;
    };

    point (double x, double y, double z, point* next) 
    {
        this -> x = x, 
        this -> y = y, 
        this -> z = z,
        this -> next = next;
    };

    void returnCoordinate ()
    {
        cout << "(" << x << ";" << y << ";" << z << ")";
    }

    void returnCoordinateBenchmark ()
    {
        for (int a = 0; a <= 7; a++) {} // for the same number of commands
    }

    point () {};
};

const int maxsize = 255000;
struct stackarr
{
    private:
    point arr [maxsize];
    int top;

    public:
    stackarr () {top = -1;} //create_empty

    bool is_empty ()
    {
        if (top == -1) 
        return true;
        else
        return false;
    };

    void peek ()
    {
        if (is_empty())
        cout << "STACK IS EMPTY\n";
        else
        arr[top].returnCoordinate();
    }

    void peekBenchmark ()
    {
        if (!is_empty())
        arr[top].returnCoordinateBenchmark();
    }

    void pop ()
    {
        if (is_empty())
        cout << "STACK IS EMPTY\n";
        else
        arr[top--].returnCoordinate();
    }

    void popBenchmark ()
    {
        if (!is_empty())
        arr[top--].returnCoordinateBenchmark();
    }

    void push (point newElem)
    {
        if (top == maxsize)
        cout << "STACK IS FULL\n";
        else
        arr[++top] = newElem;
    }

    void pushBenchmark (point newElem)
    {
        if (!(top == maxsize))
        arr[++top] = newElem;
    }
};

struct stackvect
{
    private:
    vector <point> arr;

    public:
    stackvect () {} //create_empty

    bool is_empty ()
    {
        return arr.empty();
    };

    void peek ()
    {
        if (is_empty())
        cout << "STACK IS EMPTY\n";
        else
        arr.back().returnCoordinate();
    }

    void peekBenchmark ()
    {
        if (!is_empty())
        arr.back().returnCoordinateBenchmark();
    }

    void pop ()
    {
        if (is_empty())
        cout << "STACK IS EMPTY\n";
        else
        {
            arr.back().returnCoordinate();
            arr.pop_back();
        }
    }

    void popBenchmark ()
    {
        if (!is_empty())
        {
            arr.back().returnCoordinateBenchmark();
            arr.pop_back();
        }
    }

    void push (point newElem)
    {
        arr.push_back(newElem);
    }
};

struct stacklist
{
    point* top;

    stacklist () {top = nullptr;} //create_empty

    bool is_empty ()
    {
        if (top == nullptr)
        return true;
        else
        return false;
    };

    void peek ()
    {
        if (is_empty())
        cout << "STACK IS EMPTY\n";
        else
        top -> returnCoordinate();
    }

    void peekBenchmark ()
    {
        if (!is_empty())
        top -> returnCoordinateBenchmark();
    }

    void pop ()
    {
        if (is_empty())
        cout << "STACK IS EMPTY\n";
        else
        top -> returnCoordinate();
        top = top -> next;
    }

    void popBenchmark ()
    {
        if (!is_empty())
        {
            top -> returnCoordinateBenchmark();
            top = top -> next;
        }
    }

    void push (double x, double y, double z)
    {
        point* newElement = new point (x, y, z);
        newElement -> next = top;
        top = newElement;
    }
};

void interactiveMode()
{
    int choice1, choice2;

    cout << "\nSelect stack type:\n";
    cout << "1. Array\n";
    cout << "2. Vector\n";
    cout << "3. List\n";
    cout << "Enter your choice: ";
    cin >> choice1;

    switch (choice1)
    {
        case 1:
        {
            stackarr newStackarr = stackarr();
            cout << "New stack has been created! :)\n";

            do 
            {
                cout << "\nMenu:\n";
                cout << "1. Push\n";
                cout << "2. Pop\n";
                cout << "3. Peek\n";
                cout << "4. Is empty\n";
                cout << "5. Exit\n";
                cout << "Enter your choice: ";
                cin >> choice2;

                switch (choice2)
                {
                    case 1:
                    {
                        double x, y, z;
                        cout << "Enter (x, y, z): ";
                        cin >> x >> y >> z;

                        point newPoint = point (x, y, z);
                        newStackarr.push(newPoint);

                        cout << "Point added successfully!\n";

                        break;
                    }

                    case 2:
                    {
                        cout << "Deleted point: ";
                        newStackarr.pop();

                        break;
                    }

                    case 3:
                    {
                        cout << "Top point is: "; 
                        newStackarr.peek();

                        break;
                    }

                    case 4:
                    {
                        cout << "Is empty: " << newStackarr.is_empty();

                        break;
                    }

                    case 5:
                    {
                        cout << "Exiting...\n";

                        break;
                    }

                    default:
                    cout << "Invalid choice.\n";
                }
            } while (choice2 != 5);

            break;
        }

        case 2:
        {
            stackvect newStackarr = stackvect();
            cout << "\nNew stack has been created! :)\n";

            do 
            {
                cout << "\nMenu:\n";
                cout << "1. Push\n";
                cout << "2. Pop\n";
                cout << "3. Peek\n";
                cout << "4. Is empty\n";
                cout << "5. Exit\n";
                cout << "Enter your choice: ";
                cin >> choice2;

                switch (choice2)
                {
                    case 1:
                    {
                        double x, y, z;
                        cout << "Enter (x, y, z): ";
                        cin >> x >> y >> z;

                        point newPoint = point (x, y, z);
                        newStackarr.push(newPoint);

                        cout << "Point added successfully!\n";

                        break;
                    }

                    case 2:
                    {
                        cout << "Deleted point: ";
                        newStackarr.pop();

                        break;
                    }

                    case 3:
                    {
                        cout << "Top point is: "; 
                        newStackarr.peek();

                        break;
                    }

                    case 4:
                    {
                        cout << "Is empty: " << newStackarr.is_empty();

                        break;
                    }

                    case 5:
                    {
                        cout << "Exiting...\n";

                        break;
                    }

                    default:
                    cout << "Invalid choice.\n";
                }
            } while (choice2 != 5);

            break;
        }

        case 3:
        {
            stacklist newStackarr = stacklist();
            cout << "\nNew stack has been created! :)\n";
            do 
            {
                cout << "\nMenu:\n";
                cout << "1. Push\n";
                cout << "2. Pop\n";
                cout << "3. Peek\n";
                cout << "4. Is empty\n";
                cout << "5. Exit\n";
                cout << "Enter your choice: ";
                cin >> choice2;

                switch (choice2)
                {
                    case 1:
                    {
                        double x, y, z;
                        cout << "Enter (x, y, z): ";
                        cin >> x >> y >> z;

                        newStackarr.push(x, y, z);

                        cout << "Point added successfully!\n";

                        break;
                    }

                    case 2:
                    {
                        cout << "Deleted point: ";
                        newStackarr.pop();

                        break;
                    }

                    case 3:
                    {
                        cout << "Top point is: "; 
                        newStackarr.peek();

                        break;
                    }

                    case 4:
                    {
                        cout << "Is empty: " << newStackarr.is_empty();

                        break;
                    }

                    case 5:
                    {
                        cout << "Exiting...\n";

                        break;
                    }

                    default:
                    cout << "Invalid choice.\n";
                }
            } while (choice2 != 5);

            break;
        }

        default:
        cout << "Invalid choice.\n";
    }
}

void demoMode()
{
    point point1 (1, 2, 3);
    point point2 (4, 5, 6);
    point point3 (7, 8, 9);
    
            cout << "\n1. Array\n";

    stackarr newStackarr = stackarr();
    cout << "New stack has been created! :)\n";

    cout << "Is empty: " << newStackarr.is_empty();

    cout << "\nPush: ";
    newStackarr.push(point1);
    newStackarr.push(point2);
    newStackarr.push(point3);
    cout << "points added successfully!";

    cout << "\nPeek: ";
    newStackarr.peek();

    cout << "\nDeleted element (pop): ";
    newStackarr.pop();

    cout << "\nPeek: ";
    newStackarr.peek();

    cout << "\nIs empty: " << newStackarr.is_empty();

            cout << "\n\n2. Vector\n";

    stackvect newStackvect = stackvect();
    cout << "New stack has been created! :)\n";

    cout << "Is empty: " << newStackvect.is_empty();

    cout << "\nPush: ";
    newStackvect.push(point1);
    newStackvect.push(point2);
    newStackvect.push(point3);
    cout << "points added successfully!";

    cout << "\nPeek: ";
    newStackvect.peek();

    cout << "\nDeleted element (pop): ";
    newStackvect.pop();

    cout << "\nPeek: ";
    newStackvect.peek();

    cout << "\nIs empty: " << newStackvect.is_empty();

            cout << "\n\n3. List\n";

    stacklist newStacklist = stacklist();
    cout << "New stack has been created! :)\n";

    cout << "Is empty: " << newStacklist.is_empty();

    cout << "\nPush: ";
    newStacklist.push(1, 2, 3);
    newStacklist.push(4, 5, 6);
    newStacklist.push(7, 8, 9);
    cout << "points added successfully!";

    cout << "\nPeek: ";
    newStacklist.peek();

    cout << "\nDeleted element (pop): ";
    newStacklist.pop();

    cout << "\nPeek: ";
    newStacklist.peek();

    cout << "\nIs empty: " << newStacklist.is_empty();
}

void benchmark()
{
    int n;
    cout << "\nEnter N: ";
    cin >> n;

        auto start = chrono::high_resolution_clock::now();

    auto startInitializing = chrono::high_resolution_clock::now();
    stackarr stack1 = stackarr();
    auto endInitializing = chrono::high_resolution_clock::now();

    auto firstStartIsEmpty = chrono::high_resolution_clock::now();
    stack1.is_empty();
    auto firstEndIsEmpty = chrono::high_resolution_clock::now();

    auto startAdd = chrono::high_resolution_clock::now();
    for (int j = 0; j < n; j++)
    {
        double x = rand()% 1000 / 100.0;
        double y = rand()% 1000 / 100.0;
        double z = rand()% 1000 / 100.0;

        point pointt(x, y, z);

        stack1.pushBenchmark(pointt);
    }
    auto endAdd = chrono::high_resolution_clock::now();

    auto firstPeekStart = chrono::high_resolution_clock::now();
    stack1.peekBenchmark();
    auto firstPeekEnd = chrono::high_resolution_clock::now();

    auto startPop = chrono::high_resolution_clock::now();
    stack1.popBenchmark();
    auto endPop = chrono::high_resolution_clock::now();

    auto secondPeekStart = chrono::high_resolution_clock::now();
    stack1.peekBenchmark();
    auto secondPeekEnd = chrono::high_resolution_clock::now();

    auto secondStartIsEmpty = chrono::high_resolution_clock::now();
    stack1.is_empty();
    auto secondEndIsEmpty = chrono::high_resolution_clock::now();

    auto end = chrono::high_resolution_clock::now();

    auto durationAll = chrono::duration_cast<chrono::milliseconds>(end - start);
    auto durationAdd = chrono::duration_cast<chrono::milliseconds>(endAdd - startAdd);
    auto durationInitializing = chrono::duration_cast<chrono::milliseconds>(endInitializing - startInitializing);
    auto durationFirstIsEmpty = chrono::duration_cast<chrono::milliseconds>(firstEndIsEmpty - firstStartIsEmpty);
    auto durationSecondIsEmpty = chrono::duration_cast<chrono::milliseconds>(secondEndIsEmpty - secondStartIsEmpty);
    auto durationFirstPeek = chrono::duration_cast<chrono::milliseconds>(firstPeekEnd - firstPeekStart);
    auto durationSecondPeek = chrono::duration_cast<chrono::milliseconds>(secondPeekEnd - secondPeekStart);
    auto durationPop = chrono::duration_cast<chrono::milliseconds>(endPop - startPop);

    cout << "\nTime using ARRAY\n";
    cout << "Time initializing stack: " << durationInitializing.count() << endl;
    cout << "Time is_empty (1st): " << durationFirstIsEmpty.count() << endl;
    cout << "Time adding points: " << durationAdd.count() << endl;
    cout << "Time peek (1st): " << durationFirstPeek.count() << endl;
    cout << "Time pop: " << durationPop.count() << endl;
    cout << "Time peek (2nd): " << durationSecondPeek.count() << endl;
    cout << "Time is_empty (2nd): " << durationSecondIsEmpty.count() << endl;
    cout << "Duration: " << durationAll.count() << endl;
    cout << "Size of stack: " << sizeof(stack1) << endl;

        start = chrono::high_resolution_clock::now();

    startInitializing = chrono::high_resolution_clock::now();
    stackvect stack2 = stackvect();
    endInitializing = chrono::high_resolution_clock::now();

    firstStartIsEmpty = chrono::high_resolution_clock::now();
    stack2.is_empty();
    firstEndIsEmpty = chrono::high_resolution_clock::now();

    startAdd = chrono::high_resolution_clock::now();
    for (int j = 0; j < n; j++)
    {
        double x = rand()% 1000 / 100.0;
        double y = rand()% 1000 / 100.0;
        double z = rand()% 1000 / 100.0;

        point pointt(x, y, z);

        stack2.push(pointt);
    }
    endAdd = chrono::high_resolution_clock::now();

    firstPeekStart = chrono::high_resolution_clock::now();
    stack2.peekBenchmark();
    firstPeekEnd = chrono::high_resolution_clock::now();

    startPop = chrono::high_resolution_clock::now();
    stack2.popBenchmark();
    endPop = chrono::high_resolution_clock::now();

    secondPeekStart = chrono::high_resolution_clock::now();
    stack2.peekBenchmark();
    secondPeekEnd = chrono::high_resolution_clock::now();

    secondStartIsEmpty = chrono::high_resolution_clock::now();
    stack2.is_empty();
    secondEndIsEmpty = chrono::high_resolution_clock::now();

    end = chrono::high_resolution_clock::now();

    durationAll = chrono::duration_cast<chrono::milliseconds>(end - start);
    durationAdd = chrono::duration_cast<chrono::milliseconds>(endAdd - startAdd);
    durationInitializing = chrono::duration_cast<chrono::milliseconds>(endInitializing - startInitializing);
    durationFirstIsEmpty = chrono::duration_cast<chrono::milliseconds>(firstEndIsEmpty - firstStartIsEmpty);
    durationSecondIsEmpty = chrono::duration_cast<chrono::milliseconds>(secondEndIsEmpty - secondStartIsEmpty);
    durationFirstPeek = chrono::duration_cast<chrono::milliseconds>(firstPeekEnd - firstPeekStart);
    durationSecondPeek = chrono::duration_cast<chrono::milliseconds>(secondPeekEnd - secondPeekStart);
    durationPop = chrono::duration_cast<chrono::milliseconds>(endPop - startPop);

        cout << "\nTime using VECTOR\n";
    cout << "Time initializing stack: " << durationInitializing.count() << endl;
    cout << "Time is_empty (1st): " << durationFirstIsEmpty.count() << endl;
    cout << "Time adding points: " << durationAdd.count() << endl;
    cout << "Time peek (1st): " << durationFirstPeek.count() << endl;
    cout << "Time pop: " << durationPop.count() << endl;
    cout << "Time peek (2nd): " << durationSecondPeek.count() << endl;
    cout << "Time is_empty (2nd): " << durationSecondIsEmpty.count() << endl;
    cout << "Duration: " << durationAll.count() << endl;
    cout << "Size of stack: " << sizeof(stack2) << endl;

        start = chrono::high_resolution_clock::now();

    startInitializing = chrono::high_resolution_clock::now();
    stacklist stack3 = stacklist();
    endInitializing = chrono::high_resolution_clock::now();

    firstStartIsEmpty = chrono::high_resolution_clock::now();
    stack3.is_empty();
    firstEndIsEmpty = chrono::high_resolution_clock::now();

    startAdd = chrono::high_resolution_clock::now();
    for (int j = 0; j < n; j++)
    {
        double x = rand()% 1000 / 100.0;
        double y = rand()% 1000 / 100.0;
        double z = rand()% 1000 / 100.0;

        stack3.push(x, y, z);
    }
    endAdd = chrono::high_resolution_clock::now();

    firstPeekStart = chrono::high_resolution_clock::now();
    stack3.peekBenchmark();
    firstPeekEnd = chrono::high_resolution_clock::now();

    startPop = chrono::high_resolution_clock::now();
    stack3.popBenchmark();
    endPop = chrono::high_resolution_clock::now();

    secondPeekStart = chrono::high_resolution_clock::now();
    stack3.peekBenchmark();
    secondPeekEnd = chrono::high_resolution_clock::now();

    secondStartIsEmpty = chrono::high_resolution_clock::now();
    stack3.is_empty();
    secondEndIsEmpty = chrono::high_resolution_clock::now();

    end = chrono::high_resolution_clock::now();

    durationAll = chrono::duration_cast<chrono::milliseconds>(end - start);
    durationAdd = chrono::duration_cast<chrono::milliseconds>(endAdd - startAdd);
    durationInitializing = chrono::duration_cast<chrono::milliseconds>(endInitializing - startInitializing);
    durationFirstIsEmpty = chrono::duration_cast<chrono::milliseconds>(firstEndIsEmpty - firstStartIsEmpty);
    durationSecondIsEmpty = chrono::duration_cast<chrono::milliseconds>(secondEndIsEmpty - secondStartIsEmpty);
    durationFirstPeek = chrono::duration_cast<chrono::milliseconds>(firstPeekEnd - firstPeekStart);
    durationSecondPeek = chrono::duration_cast<chrono::milliseconds>(secondPeekEnd - secondPeekStart);
    durationPop = chrono::duration_cast<chrono::milliseconds>(endPop - startPop);

        cout << "\nTime using LIST\n";
    cout << "Time initializing stack: " << durationInitializing.count() << endl;
    cout << "Time is_empty (1st): " << durationFirstIsEmpty.count() << endl;
    cout << "Time adding points: " << durationAdd.count() << endl;
    cout << "Time peek (1st): " << durationFirstPeek.count() << endl;
    cout << "Time pop: " << durationPop.count() << endl;
    cout << "Time peek (2nd): " << durationSecondPeek.count() << endl;
    cout << "Time is_empty (2nd): " << durationSecondIsEmpty.count() << endl;
    cout << "Duration: " << durationAll.count() << endl;
    cout << "Size of stack: " << sizeof(stack3) << endl;
}

int main () 
{
    int choice;

    cout << "Modes:\n";
    cout << "1. Interactive\n" ;
    cout << "2. Demo\n";
    cout << "3. Benchmark\n";
    cout << "Select mode: ";
    cin >> choice;

    switch(choice)
    {
        case 1:
        {
            interactiveMode();

            break;
        }

        case 2:
        {
            demoMode();

            break;
        }

        case 3:
        {
            benchmark();

            break;
        }
    }
    return 0;
}
