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

    void vozvrat ()
    {
        cout << "(" << x << ";" << y << ";" << z << ")";
    }

    point () {};
};

const int maxsize = 99;
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
        arr[top].vozvrat();
    }

    void pop ()
    {
        if (is_empty())
        cout << "STACK IS EMPTY\n";
        else
        arr[top--].vozvrat();
    }

    void push (point newElem)
    {
        //ya loh
        if (top == maxsize)
        cout << "STACK IS FULL\n";
        else
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
        arr.back().vozvrat();
    }

    void pop ()
    {
        if (is_empty())
        cout << "STACK IS EMPTY\n";
        else
        {
            arr.back().vozvrat();
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
        top -> vozvrat();
    }

    void pop ()
    {
        if (is_empty())
        cout << "STACK IS EMPTY\n";
        else
        top -> vozvrat();
        top = top -> next; // :)
    }

    void push (double x, double y, double z)
    {
        point* newElement = new point (x, y, z); // :(
        newElement -> next = top;
        top = newElement;
    }
};

void interactiveMode()
{
    int choice;

    cout << "Select stack type:\n";
    cout << "1. Array\n";
    cout << "2. Vector\n";
    cout << "3. List\n";
    cout << "Enter your choice: ";
    cin >> choice;

    switch (choice)
    {
        case 1:
        {
            do 
            {
                stackarr newStackarr = stackarr();
                cout << "New stack has been created! :)\n";

                cout << "Menu:\n";
                cout << "1. Push\n";
                cout << "2. Pop\n";
                cout << "3. Peek\n";
                cout << "4. Is empty\n";
                cout << "5. Exit\n";
                cout << "Enter your choice: ";
                cin >> choice;

                switch (choice)
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

                    default:
                    cout << "Invalid choice.\n";
                }
            } while (choice != 5);
        }

        case 2:
        {
            do 
            {
                stackvect newStackarr = stackvect();
                cout << "New stack has been created! :)\n";

                cout << "Menu:\n";
                cout << "1. Push\n";
                cout << "2. Pop\n";
                cout << "3. Peek\n";
                cout << "4. Is empty\n";
                cout << "5. Exit\n";
                cout << "Enter your choice: ";
                cin >> choice;

                switch (choice)
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

                    default:
                    cout << "Invalid choice.\n";
                }
            } while (choice != 5);

        }

        case 3:
        {
            do 
            {
                stacklist newStackarr = stacklist();
                cout << "New stack has been created! :)\n";
                
                cout << "Menu:\n";
                cout << "1. Push\n";
                cout << "2. Pop\n";
                cout << "3. Peek\n";
                cout << "4. Is empty\n";
                cout << "5. Exit\n";
                cout << "Enter your choice: ";
                cin >> choice;

                switch (choice)
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

                    default:
                    cout << "Invalid choice.\n";
                }
            } while (choice != 5);
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
    point point4 (10, 11, 12);
    point point5 (13, 14, 15);
    
            cout << "\n1. Array\n";

    stackarr newStackarr = stackarr();
    cout << "New stack has been created! :)\n";

    cout << "Is empty: " << newStackarr.is_empty();

    cout << "\nPush: ";
    newStackarr.push(point1);
    newStackarr.push(point2);
    newStackarr.push(point3);
    newStackarr.push(point4);
    newStackarr.push(point5);
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
    newStackvect.push(point4);
    newStackvect.push(point5);
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
    newStacklist.push(10, 11, 12);
    newStacklist.push(13, 14, 15);
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
