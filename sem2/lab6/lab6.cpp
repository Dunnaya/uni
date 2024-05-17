/* Реалізувати структуру даних «впорядкований список» на основі:
1. doneЗв’язного списку (Linked List)
2. doneСписку на основі масиву (Array List)
3. doneБінарного дерева пошуку (без балансування)
4. doneАВЛ-дерева
5. 2-3 дерева
   Координати точок в тривимірному просторі (дійсні), сортування за x-y-z, за зростанням
   
   Реалізувати операції: 
   створення пустого списку, 
   додавання елементу, вилучення елементу, 
   пошук елементів за значенням та за діапазоном значень, 
   вивід елементів у правильному порядку, 
   ?виконання певних дій над усіма елементами в правильному порядку. 
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

struct PointTree
{
    double x, y, z;
    PointTree* left;
    PointTree* right;

    PointTree(double x, double y, double z) : x(x), y(y), z(z), left(nullptr), right(nullptr) {}
};

struct PointAVL 
{
    double x, y, z;
    PointAVL* left;
    PointAVL* right;
    int height;

    PointAVL(double x, double y, double z) : x(x), y(y), z(z), left(nullptr), right(nullptr), height(1) {}
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

struct BST 
{
    PointTree* root;

    BST() : root(nullptr) {}

    void createEmptyTree() 
    {
        root = nullptr;
        cout << "An empty tree was successfully created!\n";
    }

    void add(double x, double y, double z) 
    {
        root = addRecursively(root, x, y, z);
    }

    PointTree* addRecursively(PointTree* node, double x, double y, double z) 
    {
        if (!node) 
            return new PointTree(x, y, z);
        if (x < node->x || (x == node->x && y < node->y) || (x == node->x && y == node->y && z < node->z)) 
        {
            node->left = addRecursively(node->left, x, y, z);
        } 
        else 
        {
            node->right = addRecursively(node->right, x, y, z);
        }
        return node;
    }

    void remove(double x, double y, double z) 
    {
        root = removeRecursively(root, x, y, z);
    }

    PointTree* removeRecursively(PointTree* node, double x, double y, double z) 
    {
        if (!node) 
            return nullptr;
        if (x < node->x || (x == node->x && y < node->y) || (x == node->x && y == node->y && z < node->z)) 
        {
            node->left = removeRecursively(node->left, x, y, z);
        } 
        else if (x > node->x || (x == node->x && y > node->y) || (x == node->x && y == node->y && z > node->z)) 
        {
            node->right = removeRecursively(node->right, x, y, z);
        } 
        else 
        {
            if (!node->left) 
            {
                PointTree* temp = node->right;
                delete node;
                return temp;
            } 
            else if (!node->right) 
            {
                PointTree* temp = node->left;
                delete node;
                return temp;
            }

            PointTree* temp = findMin(node->right);
            node->x = temp->x;
            node->y = temp->y;
            node->z = temp->z;
            node->right = removeRecursively(node->right, temp->x, temp->y, temp->z);
        }
        return node;
    }

    PointTree* findMin(PointTree* node) 
    {
        while (node && node->left) node = node->left;
        return node;
    }

    bool search(double x, double y, double z) const 
    {
        return searchRecursively(root, x, y, z);
    }

    bool searchRecursively(PointTree* node, double x, double y, double z) const 
    {
        if (!node) return false;
        if (node->x == x && node->y == y && node->z == z) return true;
        if (x < node->x || (x == node->x && y < node->y) || (x == node->x && y == node->y && z < node->z))
            return searchRecursively(node->left, x, y, z);
        return searchRecursively(node->right, x, y, z);
    }

    void searchInRange(double xMin, double xMax, double yMin, double yMax, double zMin, double zMax) const 
    {
        searchInRangeRecursively(root, xMin, xMax, yMin, yMax, zMin, zMax);
    }

    void searchInRangeRecursively(PointTree* node, double xMin, double xMax, double yMin, double yMax, double zMin, double zMax) const 
    {
        if (!node) return;
        if (node->x >= xMin && node->x <= xMax && node->y >= yMin && node->y <= yMax && node->z >= zMin && node->z <= zMax) 
        {
            cout << "(" << node->x << ", " << node->y << ", " << node->z << ") ";
        }
        if (xMin < node->x || (xMin == node->x && yMin < node->y) || (xMin == node->x && yMin == node->y && zMin < node->z))
            searchInRangeRecursively(node->left, xMin, xMax, yMin, yMax, zMin, zMax);
        if (xMax > node->x || (xMax == node->x && yMax > node->y) || (xMax == node->x && yMax == node->y && zMax > node->z))
            searchInRangeRecursively(node->right, xMin, xMax, yMin, yMax, zMin, zMax);
    }

    void print() const 
    {
        printRecursively(root);
        cout << endl;
    }

    void printRecursively(PointTree* node) const 
    {
        if (!node) return;
        printRecursively(node->left);
        cout << "(" << node->x << ", " << node->y << ", " << node->z << ") ";
        printRecursively(node->right);
    }

    void clear() 
    {
        clearRecursively(root);
        root = nullptr;
    }

    void clearRecursively(PointTree* node) 
    {
        if (!node) return;
        clearRecursively(node->left);
        clearRecursively(node->right);
        delete node;
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
};

struct AVLTree 
{
    PointAVL* root;

    AVLTree() : root(nullptr) {}

    int getHeight(PointAVL* node) 
    {
        return node ? node->height : 0;
    }

    int getBalance(PointAVL* node) 
    {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    void updateHeight(PointAVL* node) 
    {
        if (node) 
        {
            node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        }
    }

    PointAVL* rotateRight(PointAVL* y) 
    {
        PointAVL* x = y->left;
        PointAVL* T2 = x->right;

        x->right = y;
        y->left = T2;

        updateHeight(y);
        updateHeight(x);

        return x;
    }

    PointAVL* rotateLeft(PointAVL* x) 
    {
        PointAVL* y = x->right;
        PointAVL* T2 = y->left;

        y->left = x;
        x->right = T2;

        updateHeight(x);
        updateHeight(y);

        return y;
    }

    PointAVL* balance(PointAVL* node) 
    {
        int balance = getBalance(node);

        if (balance > 1) 
        {
            if (getBalance(node->left) < 0)
                node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        if (balance < -1) 
        {
            if (getBalance(node->right) > 0)
                node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    void createEmptyTree() 
    {
        root = nullptr;
        cout << "An empty tree was successfully created!\n";
    }

    void add(double x, double y, double z) 
    {
        root = addRecursively(root, x, y, z);
    }

    PointAVL* addRecursively(PointAVL* node, double x, double y, double z) 
    {
        if (!node) 
            return new PointAVL(x, y, z);

        if (x < node->x || (x == node->x && y < node->y) || (x == node->x && y == node->y && z < node->z)) 
        {
            node->left = addRecursively(node->left, x, y, z);
        } 
        else 
        {
            node->right = addRecursively(node->right, x, y, z);
        }

        updateHeight(node);
        return balance(node);
    }

    void remove(double x, double y, double z) 
    {
        root = removeRecursively(root, x, y, z);
    }

    PointAVL* removeRecursively(PointAVL* node, double x, double y, double z) 
    {
        if (!node) 
            return nullptr;

        if (x < node->x || (x == node->x && y < node->y) || (x == node->x && y == node->y && z < node->z)) 
        {
            node->left = removeRecursively(node->left, x, y, z);
        } 
        else if (x > node->x || (x == node->x && y > node->y) || (x == node->x && y == node->y && z > node->z)) 
        {
            node->right = removeRecursively(node->right, x, y, z);
        } 
        else 
        {
            if (!node->left) 
            {
                PointAVL* temp = node->right;
                delete node;
                return temp;
            } 
            else if (!node->right) 
            {
                PointAVL* temp = node->left;
                delete node;
                return temp;
            }

            PointAVL* temp = findMin(node->right);
            node->x = temp->x;
            node->y = temp->y;
            node->z = temp->z;
            node->right = removeRecursively(node->right, temp->x, temp->y, temp->z);
        }

        updateHeight(node);
        return balance(node);
    }

    PointAVL* findMin(PointAVL* node) 
    {
        while (node && node->left) 
            node = node->left;
        return node;
    }

    bool search(double x, double y, double z) const 
    {
        return searchRecursively(root, x, y, z);
    }

    bool searchRecursively(PointAVL* node, double x, double y, double z) const 
    {
        if (!node) 
            return false;

        if (node->x == x && node->y == y && node->z == z) 
            return true;

        if (x < node->x || (x == node->x && y < node->y) || (x == node->x && y == node->y && z < node->z)) 
            return searchRecursively(node->left, x, y, z);

        return searchRecursively(node->right, x, y, z);
    }

    void searchInRange(double xMin, double xMax, double yMin, double yMax, double zMin, double zMax) const 
    {
        searchInRangeRecursively(root, xMin, xMax, yMin, yMax, zMin, zMax);
    }

    void searchInRangeRecursively(PointAVL* node, double xMin, double xMax, double yMin, double yMax, double zMin, double zMax) const 
    {
        if (!node) 
            return;

        if (node->x >= xMin && node->x <= xMax && node->y >= yMin && node->y <= yMax && node->z >= zMin && node->z <= zMax) 
        {
            cout << "(" << node->x << ", " << node->y << ", " << node->z << ") ";
        }

        if (xMin < node->x || (xMin == node->x && yMin < node->y) || (xMin == node->x && yMin == node->y && zMin < node->z))
            searchInRangeRecursively(node->left, xMin, xMax, yMin, yMax, zMin, zMax);

        if (xMax > node->x || (xMax == node->x && yMax > node->y) || (xMax == node->x && yMax == node->y && zMax > node->z))
            searchInRangeRecursively(node->right, xMin, xMax, yMin, yMax, zMin, zMax);
    }

    void print() const 
    {
        printRecursively(root);
        cout << endl;
    }

    void printRecursively(PointAVL* node) const 
    {
        if (!node) 
            return;

        printRecursively(node->left);
        cout << "(" << node->x << ", " << node->y << ", " << node->z << ") ";
        printRecursively(node->right);
    }

    void clear() 
    {
        clearRecursively(root);
        root = nullptr;
    }

    void clearRecursively(PointAVL* node) 
    {
        if (!node) 
            return;

        clearRecursively(node->left);
        clearRecursively(node->right);
        delete node;
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

    srand(time(nullptr));
    BST BSTtree;
    BSTtree.createEmptyTree();
    BSTtree.fillRandom(10);
    cout << "BST: ";
    BSTtree.print();
    cout << "Removing (0.5, 0.5, 0.5) from the tree..." << endl;
    BSTtree.remove(0.5, 0.5, 0.5);
    cout << "Tree after removal: ";
    BSTtree.print();
    cout << "Searching for (0.3, 0.3, 0.3): " << (BSTtree.search(0.3, 0.3, 0.3) ? "Found" : "Not Found") << endl;
    cout << "Points in range (2, 5, 2, 5, 2, 5): ";
    BSTtree.searchInRange(2, 5, 2, 5, 2, 5);
    cout << endl;
    BSTtree.clear();

    srand(time(nullptr));
    AVLTree AVLtree;
    AVLtree.createEmptyTree();
    AVLtree.fillRandom(10);
    cout << "AVLTree: ";
    AVLtree.print();
    cout << "Removing (0.5, 0.5, 0.5) from the tree..." << endl;
    AVLtree.remove(0.5, 0.5, 0.5);
    cout << "Tree after removal: ";
    AVLtree.print();
    cout << "Searching for (0.3, 0.3, 0.3): " << (AVLtree.search(0.3, 0.3, 0.3) ? "Found" : "Not Found") << endl;
    cout << "Points in range (2, 5, 2, 5, 2, 5): ";
    AVLtree.searchInRange(2, 5, 2, 5, 2, 5);
    cout << endl;
    AVLtree.clear();

    return 0;
}
