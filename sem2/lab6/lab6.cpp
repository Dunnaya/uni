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
};

Point* createPoint(double x, double y, double z) 
{
    Point* newPoint = new Point;
    newPoint->x = x;
    newPoint->y = y;
    newPoint->z = z;
    newPoint->next = nullptr;

    return newPoint;
}

void createEmptyLinkedList(Point*& head) 
{
    head = nullptr;
    cout << "An empty list was successfully created!\n";
}

void addLinkedList(Point*& head, double x, double y, double z) 
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

void removeLinkedList(Point*& head, double x, double y, double z) 
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

bool searchLinkedList(Point* head, double x, double y, double z) 
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

void searchInRangeLinkedList(Point* head, double xMin, double xMax, double yMin, double yMax, double zMin, double zMax) {
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

void printLinkedList(Point* head) 
{
    Point* current = head;
    while (current) 
    {
        cout << "(" << current->x << ", " << current->y << ", " << current->z << ") ";
        current = current->next;
    }
    cout << endl;
}

void deleteElLinkedList(Point*& head) 
{
    while (head) 
    {
        Point* temp = head;
        head = head->next;
        delete temp;
    }
}

void fillRandomLinkedList(Point*& head, int count)
{
    for (int i = 0; i < count; ++i) 
    {
        double x = rand()% 1000 / 100.0;
        double y = rand()% 1000 / 100.0;
        double z = rand()% 1000 / 100.0;
        addLinkedList(head, x, y, z);
    }
}

int main() 
{
    Point* list = nullptr;
    createEmptyLinkedList(list);
    fillRandomLinkedList(list, 10);
    cout << "List: ";
    printLinkedList(list);
    cout << "Removing (0.5, 0.5, 0.5) from the list..." << endl;
    removeLinkedList(list, 0.5, 0.5, 0.5);
    cout << "List after removal: ";
    printLinkedList(list);
    cout << "Searching for (0.3, 0.3, 0.3): " << (searchLinkedList(list, 0.3, 0.3, 0.3) ? "Found" : "Not Found") << endl;
    searchInRangeLinkedList(list, 2, 5, 2, 5, 2, 5);
    deleteElLinkedList(list);
    return 0;
}
