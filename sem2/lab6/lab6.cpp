/* Реалізувати структуру даних «впорядкований список» на основі:
1. Зв’язного списку (Linked List)
2. Списку на основі масиву (Array List)
3. Бінарного дерева пошуку (без балансування)
4. АВЛ-дерева
5. 2-3 дерева
   Координати точок в тривимірному просторі (дійсні), сортування за x-y-z, за зростанням
   
   Реалізувати операції: 
   створення пустого списку, 
   додавання елементу, вилучення елементу, 
   пошук елементів за значенням та за діапазоном значень, 
   вивід елементів у правильному порядку, 
   виконання певних дій над усіма елементами в правильному порядку. 
   Також реалізувати заповнення списку заданою кількістю випадкових елементів.*/

#include <iostream>
#include <cstdlib>
using namespace std;

struct Point 
{
    double x, y, z;
    Point* next;

    Point(double x, double y, double z) : x(x), y(y), z(z), next(nullptr) {}
};

struct LinkedList 
{
    Point* head;

    LinkedList() : head(nullptr) {}

    void createEmptyList() 
    {
        head = nullptr;
        cout << "An empty list was successfully created!\n";
    }

    void add(double x, double y, double z) 
    {
        Point* newNode = createPoint(x, y, z);
        if (!head || head->x > x || (head->x == x && head->y > y) || (head->x == x && head->y == y && head->z > z)) 
        {
            newNode->next = head;
            head = newNode;
        } 
        else 
        {
            Point* current = head;
            while (current->next && 
                   (current->next->x < x ||
                    (current->next->x == x && current->next->y < y) ||
                    (current->next->x == x && current->next->y == y && current->next->z < z))) 
            {
                current = current->next;
            }
            newNode->next = current->next;
            current->next = newNode;
        }
    }

    void remove(double x, double y, double z) 
    {
        Point* current = head;
        Point* prev = nullptr;
        while (current && !(current->x == x && current->y == y && current->z == z)) 
        {
            prev = current;
            current = current->next;
        }
        if (current) 
        {
            if (prev)
                prev->next = current->next;
            else
                head = current->next;
            delete current;
        }
    }

    bool search(double x, double y, double z) 
    {
        Point* current = head;
        while (current) 
        {
            if (current->x == x && current->y == y && current->z == z)
                return true;
            current = current->next;
        }
        return false;
    }

    void searchInRange(double xMin, double xMax, double yMin, double yMax, double zMin, double zMax) 
    {
        Point* current = head;
        bool found = false;
        while (current) 
        {
            if (current->x >= xMin && current->x <= xMax &&
                current->y >= yMin && current->y <= yMax &&
                current->z >= zMin && current->z <= zMax) 
            {
                cout << "(" << current->x << ", " << current->y << ", " << current->z << ") ";
                found = true;
            }
            current = current->next;
        }
        if (!found)
            cout << "No points found in the range." << endl;
        else cout << endl;
    }

    void print() 
    {
        Point* current = head;
        while (current) 
        {
            cout << "(" << current->x << ", " << current->y << ", " << current->z << ") ";
            current = current->next;
        }
        cout << endl;
    }

    void clear() 
    {
        while (head) 
        {
            Point* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void fillRandom(int count)
    {
        for (int i = 0; i < count; ++i) 
        {
            double x = rand()% 1000 / 100.0;
            double y = rand()% 1000 / 100.0;
            double z = rand()% 1000 / 100.0;
            add(x, y, z);
        }
    }

    Point* createPoint(double x, double y, double z) 
    {
        return new Point(x, y, z);
    }
};

struct ArrayList 
{
    Point* head;

    ArrayList() : head(nullptr) {}

    void createEmptyList() 
    {
        head = nullptr;
        cout << "An empty list was successfully created!\n";
    }

    void add(double x, double y, double z) 
    {
        Point* newPoint = createPoint(x, y, z);
        if (!head || head->x > x || (head->x == x && head->y > y) || (head->x == x && head->y == y && head->z > z)) 
        {
            newPoint->next = head;
            head = newPoint;
        } else {
            Point* current = head;
            while (current->next && 
                   (current->next->x < x ||
                    (current->next->x == x && current->next->y < y) ||
                    (current->next->x == x && current->next->y == y && current->next->z < z))) 
            {
                current = current->next;
            }
            newPoint->next = current->next;
            current->next = newPoint;
        }
    }

    void remove(double x, double y, double z) 
    {
        Point* current = head;
        Point* prev = nullptr;
        while (current && !(current->x == x && current->y == y && current->z == z)) 
        {
            prev = current;
            current = current->next;
        }
        if (current) 
        {
            if (prev)
                prev->next = current->next;
            else
                head = current->next;
            delete current;
        }
    }

    bool search(double x, double y, double z) const 
    {
        Point* current = head;
        while (current) 
        {
            if (current->x == x && current->y == y && current->z == z)
                return true;
            current = current->next;
        }
        return false;
    }

    void searchInRange(double xMin, double xMax, double yMin, double yMax, double zMin, double zMax) const
    {
        Point* current = head;
        bool found = false;
        while (current) 
        {
            if (current->x >= xMin && current->x <= xMax &&
                current->y >= yMin && current->y <= yMax &&
                current->z >= zMin && current->z <= zMax) 
            {
                cout << "(" << current->x << ", " << current->y << ", " << current->z << ") ";
                found = true;
            }
            current = current->next;
        }
        if (!found)
            cout << "No points found in the range." << endl;
        else
            cout << endl;
    }

    void print() const 
    {
        Point* current = head;
        while (current) 
        {
            cout << "(" << current->x << ", " << current->y << ", " << current->z << ") ";
            current = current->next;
        }
        cout << endl;
    }

    void clear() 
    {
        while (head) 
        {
            Point* temp = head;
            head = head->next;
            delete temp;
        }
        head = nullptr;
    }

    void fillRandom(int count)
    {
        for (int i = 0; i < count; ++i) 
        {
            double x = rand() % 1000 / 100.0;
            double y = rand() % 1000 / 100.0;
            double z = rand() % 1000 / 100.0;
            add(x, y, z);
        }
    }

    Point* createPoint(double x, double y, double z) 
    {
        return new Point(x, y, z);
    }
};

int main() 
{
    LinkedList list;
    list.createEmptyList();
    list.fillRandom(10);
    cout << "List: ";
    list.print();
    cout << "Removing (0.5, 0.5, 0.5) from the list..." << endl;
    list.remove(0.5, 0.5, 0.5);
    cout << "List after removal: ";
    list.print();
    cout << "Searching for (0.3, 0.3, 0.3): " << (list.search(0.3, 0.3, 0.3) ? "Found" : "Not Found") << endl;
    list.searchInRange(2, 5, 2, 5, 2, 5);
    list.clear();

    srand(time(nullptr));
    ArrayList arrayList;
    arrayList.createEmptyList();
    arrayList.fillRandom(10);
    cout << "ArrayList: ";
    arrayList.print();
    cout << "Removing (0.5, 0.5, 0.5) from the list..." << endl;
    arrayList.remove(0.5, 0.5, 0.5);
    cout << "List after removal: ";
    arrayList.print();
    cout << "Searching for (0.3, 0.3, 0.3): " << (arrayList.search(0.3, 0.3, 0.3) ? "Found" : "Not Found") << endl;
    arrayList.searchInRange(2, 5, 2, 5, 2, 5);
    arrayList.clear();
    return 0;
}
