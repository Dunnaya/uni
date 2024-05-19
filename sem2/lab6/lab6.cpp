/* Реалізувати структуру даних «впорядкований список» на основі:
1. Зв’язного списку (Linked List)
2. Списку на основі масиву (Array List)
3. Бінарного дерева пошуку (без балансування)
4. АВЛ-дерева
5. 2-3 дерева
   
    Координати точок в тривимірному просторі (дійсні), сортування за x-y-z, за зростанням*/

#include <iostream>
#include <cstdlib>
#include <vector>
#include <cassert>
#include <chrono>
#include <thread>
using namespace std;

void interactiveMode();
void demoMode();
void benchmark();

struct PointList 
{
    double x, y, z;
    PointList* next;

    PointList(double x, double y, double z) : x(x), y(y), z(z), next(nullptr) {}
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

struct Point //2-3 tree
{
    double x, y, z;
    Point* next;

    Point(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z), next(nullptr) {}

    // Operator <
    bool operator<(const Point& other) const 
    {
        if (x != other.x) return x < other.x;
        if (y != other.y) return y < other.y;
        return z < other.z;
    }

    // Operator >
    bool operator>(const Point& other) const 
    {
        if (x != other.x) return x > other.x;
        if (y != other.y) return y > other.y;
        return z > other.z;
    }

    // Operator <=
    bool operator<=(const Point& other) const 
    {
        return !(*this > other);
    }

    // Operator >=
    bool operator>=(const Point& other) const 
    {
        return !(*this < other);
    }

    // Operator ==
    bool operator==(const Point& other) const 
    {
        return x == other.x && y == other.y && z == other.z;
    }

    // Operator !=
    bool operator!=(const Point& other) const 
    {
        return !(*this == other);
    }

    void print() const
    {
        cout << "(" << x << ", " << y << ", " << z << ") ";
    }
};

struct LinkedList 
{
    PointList* head;

    LinkedList() : head(nullptr) {}

    void createEmptyList() 
    {
        head = nullptr;
        cout << "An empty list was successfully created!\n";
    }

    void add(double x, double y, double z) 
    {
        PointList* newNode = createPoint(x, y, z);
        if (!head || head->x > x || (head->x == x && head->y > y) || (head->x == x && head->y == y && head->z > z)) 
        {
            newNode->next = head;
            head = newNode;
        } 
        else 
        {
            PointList* current = head;
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
        PointList* current = head;
        PointList* prev = nullptr;
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
        PointList* current = head;
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
        PointList* current = head;
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
        PointList* current = head;
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
            PointList* temp = head;
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

    PointList* createPoint(double x, double y, double z) 
    {
        return new PointList(x, y, z);
    }
};

struct ArrayList 
{
    PointList* head;

    ArrayList() : head(nullptr) {}

    void createEmptyList() 
    {
        head = nullptr;
        cout << "An empty list was successfully created!\n";
    }

    void add(double x, double y, double z) 
    {
        PointList* newPoint = createPoint(x, y, z);
        if (!head || head->x > x || (head->x == x && head->y > y) || (head->x == x && head->y == y && head->z > z)) 
        {
            newPoint->next = head;
            head = newPoint;
        } else {
            PointList* current = head;
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
        PointList* current = head;
        PointList* prev = nullptr;
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
        PointList* current = head;
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
        PointList* current = head;
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
        PointList* current = head;
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
            PointList* temp = head;
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

    PointList* createPoint(double x, double y, double z) 
    {
        return new PointList(x, y, z);
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

struct two3Tree 
{
    struct TreeNode 
    {
        Point data[2];
        int size;
        TreeNode* children[3];
        TreeNode* parent;

        TreeNode(Point data1, TreeNode* parent = nullptr) 
        {
            this->data[0] = data1;
            this->size = 1;
            children[0] = children[1] = children[2] = nullptr;
            this->parent = parent;
        }

        TreeNode(Point data1, Point data2, TreeNode* parent = nullptr) 
        {
            assert(data1 <= data2);
            this->data[0] = data1;
            this->data[1] = data2;
            this->size = 2;
            children[0] = children[1] = children[2] = nullptr;
            this->parent = parent;
        }

        TreeNode(Point data1, Point data2, Point data3, TreeNode* parent = nullptr) 
        {
            assert(data1 <= data2);
            assert(data2 <= data3);

            this->data[0] = data2;
            this->size = 1;
            this->children[0] = new TreeNode(data1, this);
            this->children[1] = new TreeNode(data3, this);
            this->children[2] = nullptr;

            this->parent = parent;

        }

        void add_single_data(Point data) 
        {
            assert(size == 1);

            if (data > this->data[0]) 
            {
                this->data[1] = data;
                size = 2;
            }
            else 
            {
                this->data[1] = this->data[0];
                this->data[0] = data;
                size = 2;
            }
        }

        TreeNode* add_and_split(Point new_data) 
        {
            if (children[0] == nullptr) 
            {
                if (size == 1) 
                {
                    add_single_data(new_data);
                    return nullptr;
                }
                else 
                {
                    if (new_data < data[0])
                        return new TreeNode(new_data, data[0], data[1]);
                    else if (new_data < data[1])
                        return new TreeNode(data[0], new_data, data[1]);
                    else
                        return new TreeNode(data[0], data[1], new_data);
                }
            }

            TreeNode* extra = nullptr;

            if (size == 1) 
            {
                if (new_data < data[0]) 
                {
                    extra = children[0]->add_and_split(new_data);
                    if (!extra) return nullptr;
                    data[1] = data[0];
                    data[0] = extra->data[0];
                    children[2] = children[1];
                    children[0] = extra->children[0];
                    children[1] = extra->children[1];
                    size = 2;
                    return nullptr;
                }
                else 
                {
                    extra = children[1]->add_and_split(new_data);
                    if (!extra) return nullptr;
                    data[1] = extra->data[0];
                    children[1] = extra->children[0];
                    children[2] = extra->children[1];
                    size = 2;
                    return nullptr;
                }
            }
            else 
            {
                if (new_data < data[0]) 
                {
                    extra = children[0]->add_and_split(new_data);
                    if (!extra) return nullptr;
                    TreeNode* result = new TreeNode(extra->data[0], data[0], data[1]);
                    result->children[0]->children[0] = extra->children[0];
                    result->children[0]->children[1] = extra->children[1];
                    result->children[1]->children[0] = children[1];
                    result->children[1]->children[1] = children[2];
                    return result;
                }
                else if (new_data < data[1]) 
                {
                    extra = children[1]->add_and_split(new_data);
                    if (!extra) return nullptr;
                    TreeNode* result = new TreeNode(data[0], extra->data[0], data[1]);
                    result->children[0]->children[0] = children[0];
                    result->children[0]->children[1] = extra->children[0];
                    result->children[1]->children[0] = extra->children[1];
                    result->children[1]->children[1] = children[2];
                    return result;
                }
                else 
                {
                    extra = children[2]->add_and_split(new_data);
                    if (!extra) return nullptr;
                    TreeNode* result = new TreeNode(data[0], data[1], extra->data[0]);
                    result->children[0]->children[0] = children[0];
                    result->children[0]->children[1] = children[1];
                    result->children[1]->children[0] = extra->children[0];
                    result->children[1]->children[1] = extra->children[1];
                    return result;
                }
            }
        }

        TreeNode* find_max_subtree() 
        {
            if (children[2]) 
            {
                assert(size == 2);
                return children[2]->find_max_subtree();
            }

            if (children[1]) 
            {
                assert(size == 1);
                return children[1]->find_max_subtree();
            }

            assert(children[0] == nullptr);

            return this;
        }

        Point get_max_data() 
        {
            if (size == 2) 
                return data[1];
            else
                return data[0];
        }

        enum RemoveResult { Removed, NotFound, NeedParentRemove };

        bool rebalance(int index_current_child) 
        {
            assert(index_current_child < size + 1);
            TreeNode* current_child = children[index_current_child];
            assert(current_child);
            assert(current_child->size == 0);
            TreeNode* left_child = nullptr;

            if (index_current_child > 0) 
            { 
                left_child = children[index_current_child - 1]; 
            }

            TreeNode* right_child = (index_current_child < size) ?
                children[index_current_child + 1] :
                nullptr;

            assert(left_child != nullptr || right_child != nullptr);

            if (left_child && left_child->size == 2) 
            {
                assert(index_current_child - 1 >= 0);
                current_child->data[0] = this->data[index_current_child - 1];
                this->data[index_current_child - 1] = left_child->data[1];

                current_child->children[1] = current_child->children[0];
                current_child->children[0] = left_child->children[2];
                left_child->children[2] = nullptr;

                current_child->size = 1;
                left_child->size = 1;
                return true;
            }

            if (right_child && right_child->size == 2) 
            {
                assert(index_current_child < this->size);
                current_child->data[0] = this->data[index_current_child];
                this->data[index_current_child] = right_child->data[0];
                right_child->data[0] = right_child->data[1];

                current_child->children[1] = right_child->children[0];
                right_child->children[0] = right_child->children[1];
                right_child->children[1] = right_child->children[2];
                right_child->children[2] = nullptr;

                current_child->size = 1;
                right_child->size = 1;
                return true;
            }

            if (left_child) 
            {
                assert(left_child->size == 1);
                assert(index_current_child - 1 >= 0);

                left_child->data[1] = this->data[index_current_child - 1];

                left_child->children[2] = current_child->children[0];

                left_child->size = 2;
                this->size--;

                if (this->size == 0) 
                {
                    assert(this->children[0] == left_child);
                    assert(this->children[1] == current_child);
                    assert(this->children[2] == nullptr);
                    delete current_child;
                    this->children[1] = nullptr;
                    return false;
                }

                assert(this->size == 1);

                if (index_current_child == 1) 
                {
                    this->data[0] = this->data[1];
                    assert(this->children[0] == left_child);
                    assert(this->children[1] == current_child);
                    delete current_child;
                    this->children[1] = this->children[2];
                    this->children[2] = nullptr;
                    return true;
                }

                assert(index_current_child == 2); 
                {
                    assert(this->children[1] == left_child);
                    assert(this->children[2] == current_child);
                    delete current_child;
                    this->children[2] = nullptr;
                    return true;
                }
            }

            assert(right_child != nullptr);
            assert(right_child->size == 1);

            right_child->data[1] = right_child->data[0];
            right_child->data[0] = this->data[index_current_child];
            assert(right_child->data[0] < right_child->data[1]);

            if (current_child->children[0] != nullptr) 
            {
                assert(right_child->children[0] != nullptr);
                assert(right_child->children[1] != nullptr);
                
                right_child->children[2] = right_child->children[1];
                right_child->children[1] = right_child->children[0];
                right_child->children[0] = current_child->children[0];

            }
            else 
            {
                assert(current_child->children[0] == nullptr);
                assert(right_child->children[0] == nullptr);
                assert(right_child->children[1] == nullptr);
            }

            right_child->size = 2;
            this->size--;

            if (this->size == 0) 
            {
                assert(this->children[0] == current_child);
                assert(this->children[1] == right_child);
                assert(this->children[2] == nullptr);
                delete current_child;
                this->children[0] = this->children[1];
                this->children[1] = nullptr;
                return false;
            }
            assert(this->size == 1);

            if (index_current_child == 0) 
            {
                assert(this->children[0] == current_child);
                assert(this->children[1] == right_child);
                this->data[0] = this->data[1];
                delete current_child;
                this->children[0] = this->children[1];
                this->children[1] = this->children[2];
                this->children[2] = nullptr;
                return true;
            }

            assert(index_current_child == 1); 
            {
                assert(this->children[1] == current_child);
                assert(this->children[2] == right_child);
                delete current_child;
                this->children[1] = this->children[2];
                this->children[2] = nullptr;
                return true;
            }
        }

        RemoveResult remove(Point data_to_remove) 
        {
            if (children[0] == nullptr) 
            {
                if (size == 1) 
                {
                    if (data[0] == data_to_remove) 
                    {
                        size = 0;
                        return NeedParentRemove;
                    }
                    else
                        return NotFound;
                }
                else 
                {
                    if (data[0] == data_to_remove) 
                    {
                        data[0] = data[1];
                        size = 1;
                        return Removed;
                    }
                    else if (data[1] == data_to_remove) 
                    {
                        size = 1;
                        return Removed;
                    }
                    else
                        return NotFound;
                }
            }

            if (size == 1) 
            {
                if (data_to_remove < data[0]) 
                {
                    RemoveResult result = children[0]->remove(data_to_remove);

                    if (result == NeedParentRemove) 
                    {
                        rebalance(0);
                        if (this->size == 0) 
                            return NeedParentRemove;
                        else 
                            return Removed;
                    }
                    else
                        return result;
                }
                else if (data_to_remove > data[0]) 
                {
                    RemoveResult result = children[1]->remove(data_to_remove);

                    if (result == NeedParentRemove) 
                    {
                        rebalance(1);
                        if (this->size == 0)
                            return NeedParentRemove;
                        else
                            return Removed;
                    }
                    else
                        return result;
                }
                else 
                {
                    TreeNode* prev = children[0]->find_max_subtree();
                    assert(prev->children[0] == nullptr);
                    Point prev_data = prev->get_max_data();
                    data[0] = prev_data;

                    RemoveResult result = this->children[0]->remove(prev_data);
                    assert(result != NotFound);

                    if (result == Removed)
                        return Removed;
                    rebalance(0);

                    if (this->size == 0)
                        return NeedParentRemove;
                    else 
                        return Removed;
                }
            }

            if (size == 2) 
            {
                if (data_to_remove < data[0]) 
                {
                    RemoveResult result = children[0]->remove(data_to_remove);

                    if (result == NeedParentRemove) 
                    {
                        rebalance(0);
                        assert(this->size > 0);
                        return Removed;
                    }
                    else
                        return result;
                }
                else if (data_to_remove == data[0]) 
                {
                    TreeNode* prev = children[0]->find_max_subtree();
                    assert(prev->children[0] == nullptr);
                    Point prev_data = prev->get_max_data();
                    data[0] = prev_data;
                    RemoveResult result = this->children[0]->remove(prev_data);
                    assert(result != NotFound);

                    if (result == Removed)
                        return Removed;
                    rebalance(0);

                    assert(this->size > 0);
                    return Removed;
                }
                else if (data_to_remove < data[1]) 
                {
                    RemoveResult result = children[1]->remove(data_to_remove);
                    if (result == NeedParentRemove) 
                    {
                        rebalance(1);
                        assert(this->size > 0);
                        return Removed;
                    }
                    else
                        return result;
                }
                else if (data_to_remove == data[1]) 
                {
                    TreeNode* prev = children[1]->find_max_subtree();
                    assert(prev->children[0] == nullptr);
                    Point prev_data = prev->get_max_data();
                    data[1] = prev_data;
                    RemoveResult result = this->children[1]->remove(prev_data);
                    assert(result != NotFound);

                    if (result == Removed)
                        return Removed;
                    rebalance(1);

                    assert(this->size > 0);
                    return Removed;
                }
                else 
                {
                    RemoveResult result = children[2]->remove(data_to_remove);

                    if (result == NeedParentRemove) 
                    {
                        rebalance(2);
                        assert(this->size > 0);
                        return Removed;
                    }
                    else
                        return result;
                }
            }
        }

        void print_as_tree() 
        {
            data[0].print();

            if (size == 2)
            {
                cout << ";"; 
                data[1].print();
            }

            cout << "(";
            if (children[0])
                children[0]->print_as_tree();
            
            cout << ",";
            if (children[1])
                children[1]->print_as_tree();
            
            cout << ",";
            if (children[2])
                children[2]->print_as_tree();
            
            cout << ")";
        }

        void print_in_order() 
        {
            if (children[0])
                children[0]->print_in_order();
            
            data[0].print();
            cout << " ";

            if (children[1])
                children[1]->print_in_order();

            if (size == 2) 
            {
                data[1].print();
                cout << " ";

                if (children[2])
                    children[2]->print_in_order();
            }
        }
    };

    TreeNode* root;

    void find_elements_by_range_recursive(TreeNode* node, Point& min_modulus, Point& max_modulus, vector<Point>& result) 
    {
        if (!node) return;

        if (node->data[0] >= min_modulus && node->data[0] <= max_modulus)
            result.push_back(node->data[0]);
        
        if (node->size == 2 && node->data[1] >= min_modulus && node->data[1] <= max_modulus)
            result.push_back(node->data[1]);
        
        if (node->children[0] && node->data[0] >= min_modulus)
            find_elements_by_range_recursive(node->children[0], min_modulus, max_modulus, result);

        if (node->children[1] && node->data[0] <= max_modulus)
            find_elements_by_range_recursive(node->children[1], min_modulus, max_modulus, result);
        
        if (node->children[2] && node->size == 2 && node->data[1] <= max_modulus)
            find_elements_by_range_recursive(node->children[2], min_modulus, max_modulus, result);
    }

    two3Tree() 
    {
        root = nullptr;
    }

    void add(Point data) 
    {
        if (!root)
            root = new TreeNode(data);
        else 
        {
            TreeNode* extra = root->add_and_split(data);
            if (extra)
                root = extra;
        }

    }

    bool remove(Point data) 
    {
        TreeNode::RemoveResult result = root->remove(data);
        if (result == TreeNode::NotFound)
            return false;
        if (result == TreeNode::Removed)
            return true;
        if (result == TreeNode::NeedParentRemove) 
        {
            root = root->children[0];
            return true;
        }
    }

    void fillRandom(int count)
    {
        Point point;

        for (int i = 0; i < count; ++i) 
        {
            double x = rand() % 1000 / 100.0;
            double y = rand() % 1000 / 100.0;
            double z = rand() % 1000 / 100.0;
            point = Point(x, y, z);
            add(point);
        }
    }

    void print_as_tree() 
    {
        if (root)
            root->print_as_tree();
        else
            cout << "The tree is empty.";
        cout << endl;
    }

    void print_in_order() 
    {
        if (root) 
            root->print_in_order();
        else 
            cout << "The tree is empty.";
        cout << endl;
    }

    void print_all() 
    {
        print_as_tree();
        print_in_order();
    }

    bool find_element_by_val(Point z) 
    {
        TreeNode* current = root;

        while (current) 
        {
            if (current->data[0] == z || (current->size == 2 && current->data[1] == z))
                return current;
            else if (current->size == 1 && z < current->data[0]) 
                current = current->children[0];
            else if (current->size == 2 && z < current->data[0]) 
                current = current->children[0];
            else if (current->size == 2 && z < current->data[1]) 
                current = current->children[1];
            else 
                current = current->children[2];
        }
        return false;
    }

    vector<Point> find_elements_by_range(Point min_modulus, Point max_modulus) 
    {
        vector<Point> result;
        find_elements_by_range_recursive(root, min_modulus, max_modulus, result);
        return result;
    }
};

void linked_list_menu()
{
    LinkedList list;
    int choice;
    do
    {
        cout << "\n  Linked list menu:\n";
        cout << "1. Create an empty list\n";
        cout << "2. Add an element\n";
        cout << "3. Delete the element\n";
        cout << "4. Searh for the element\n";
        cout << "5. Search elements in range\n";
        cout << "6. Print the list\n";
        cout << "7. Generate random elements\n";
        cout << "8. Clear the list\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice)
        {
            case 1:
            {
                list.createEmptyList();
                break;
            }

            case 2:
            {
                double x, y, z;
                cout << "Enter the point you want to add (x,y,z): ";
                cin >> x >> y >> z;
                list.add(x, y, z);
                break;
            }

            case 3:
            {
                double x, y, z;
                cout << "Enter the point you want to remove (x,y,z): ";
                cin >> x >> y >> z;
                list.remove(x, y, z);
                break;
            }

            case 4:
            {
                double x, y, z;
                cout << "Enter the point you want to find (x,y,z): ";
                cin >> x >> y >> z;
                list.search(x, y, z);
                break;
            }

            case 5:
            {
                double xmin, ymin, zmin;
                cout << "Enter the 1st point in a range (x,y,z): ";
                cin >> xmin >> ymin >> zmin;

                double xmax,ymax,zmax;
                cout << "Enter the 2nd point in a range (x,y,z): ";
                cin >> xmax >> ymax >> zmax;

                list.searchInRange(xmin, xmax, ymin, ymax, zmin, zmax);
                break;
            }

            case 6:
            {
                list.print();
                break;
            }

            case 7:
            {
                int n;
                cout << "How many elements you want to generate?\n";
                cin >> n;
                list.fillRandom(n);
                break;
            }

            case 8:
            {
                list.clear();
                break;
            }

            case 9:
            {
                cout << "Exiting...\n";
                break;
            }
            
            default:
            {
                cout << "Invalid input.";
                break;
            }
        }
    } while (choice != 9);
}

void array_list_menu()
{
    ArrayList list;
    int choice;
    do
    {
        cout << "\n  Array list menu:\n";
        cout << "1. Create an empty list\n";
        cout << "2. Add an element\n";
        cout << "3. Delete the element\n";
        cout << "4. Searh for the element\n";
        cout << "5. Search elements in range\n";
        cout << "6. Print the list\n";
        cout << "7. Generate random elements\n";
        cout << "8. Clear the list\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice)
        {
            case 1:
            {
                list.createEmptyList();
                break;
            }

            case 2:
            {
                double x, y, z;
                cout << "Enter the point you want to add (x,y,z): ";
                cin >> x >> y >> z;
                list.add(x, y, z);
                break;
            }

            case 3:
            {
                double x, y, z;
                cout << "Enter the point you want to remove (x,y,z): ";
                cin >> x >> y >> z;
                list.remove(x, y, z);
                break;
            }

            case 4:
            {
                double x, y, z;
                cout << "Enter the point you want to find (x,y,z): ";
                cin >> x >> y >> z;
                list.search(x, y, z);
                break;
            }

            case 5:
            {
                double xmin, ymin, zmin;
                cout << "Enter the 1st point in a range (x,y,z): ";
                cin >> xmin >> ymin >> zmin;

                double xmax,ymax,zmax;
                cout << "Enter the 2nd point in a range (x,y,z): ";
                cin >> xmax >> ymax >> zmax;

                list.searchInRange(xmin, xmax, ymin, ymax, zmin, zmax);
                break;
            }

            case 6:
            {
                list.print();
                break;
            }

            case 7:
            {
                int n;
                cout << "How many elements you want to generate?\n";
                cin >> n;
                list.fillRandom(n);
                break;
            }

            case 8:
            {
                list.clear();
                break;
            }

            case 9:
            {
                cout << "Exiting...\n";
                break;
            }
            
            default:
            {
                cout << "Invalid input.";
                break;
            }
        }
    } while (choice != 9);
}

void binary_tree_menu()
{
    BST tree;
    int choice;
    do
    {
        cout << "\n  Binary tree menu:\n";
        cout << "1. Create an empty tree\n";
        cout << "2. Add an element\n";
        cout << "3. Delete the element\n";
        cout << "4. Searh for the element\n";
        cout << "5. Search elements in range\n";
        cout << "6. Print the list\n";
        cout << "7. Generate random elements\n";
        cout << "8. Clear the list\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice)
        {
            case 1:
            {
                tree.createEmptyTree();
                break;
            }

            case 2:
            {
                double x, y, z;
                cout << "Enter the point you want to add (x,y,z): ";
                cin >> x >> y >> z;
                tree.add(x, y, z);
                break;
            }

            case 3:
            {
                double x, y, z;
                cout << "Enter the point you want to remove (x,y,z): ";
                cin >> x >> y >> z;
                tree.remove(x, y, z);
                break;
            }

            case 4:
            {
                double x, y, z;
                cout << "Enter the point you want to find (x,y,z): ";
                cin >> x >> y >> z;
                tree.search(x, y, z);
                break;
            }

            case 5:
            {
                double xmin, ymin, zmin;
                cout << "Enter the 1st point in a range (x,y,z): ";
                cin >> xmin >> ymin >> zmin;

                double xmax,ymax,zmax;
                cout << "Enter the 2nd point in a range (x,y,z): ";
                cin >> xmax >> ymax >> zmax;

                tree.searchInRange(xmin, xmax, ymin, ymax, zmin, zmax);
                break;
            }

            case 6:
            {
                tree.print();
                break;
            }

            case 7:
            {
                int n;
                cout << "How many elements you want to generate?\n";
                cin >> n;
                tree.fillRandom(n);
                break;
            }

            case 8:
            {
                tree.clear();
                break;
            }

            case 9:
            {
                cout << "Exiting...\n";
                break;
            }
            
            default:
            {
                cout << "Invalid input.";
                break;
            }
        }
    } while (choice != 9);
}

void avl_tree_menu()
{
    AVLTree tree;
    int choice;
    do
    {
        cout << "\n  AVL tree menu:\n";
        cout << "1. Create an empty tree\n";
        cout << "2. Add an element\n";
        cout << "3. Delete the element\n";
        cout << "4. Searh for the element\n";
        cout << "5. Search elements in range\n";
        cout << "6. Print the list\n";
        cout << "7. Generate random elements\n";
        cout << "8. Clear the list\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice)
        {
            case 1:
            {
                tree.createEmptyTree();
                break;
            }

            case 2:
            {
                double x, y, z;
                cout << "Enter the point you want to add (x,y,z): ";
                cin >> x >> y >> z;
                tree.add(x, y, z);
                break;
            }

            case 3:
            {
                double x, y, z;
                cout << "Enter the point you want to remove (x,y,z): ";
                cin >> x >> y >> z;
                tree.remove(x, y, z);
                break;
            }

            case 4:
            {
                double x, y, z;
                cout << "Enter the point you want to find (x,y,z): ";
                cin >> x >> y >> z;
                tree.search(x, y, z);
                break;
            }

            case 5:
            {
                double xmin, ymin, zmin;
                cout << "Enter the 1st point in a range (x,y,z): ";
                cin >> xmin >> ymin >> zmin;

                double xmax,ymax,zmax;
                cout << "Enter the 2nd point in a range (x,y,z): ";
                cin >> xmax >> ymax >> zmax;

                tree.searchInRange(xmin, xmax, ymin, ymax, zmin, zmax);
                break;
            }

            case 6:
            {
                tree.print();
                break;
            }

            case 7:
            {
                int n;
                cout << "How many elements you want to generate?\n";
                cin >> n;
                tree.fillRandom(n);
                break;
            }

            case 8:
            {
                tree.clear();
                break;
            }

            case 9:
            {
                cout << "Exiting...\n";
                break;
            }
            
            default:
            {
                cout << "Invalid input.";
                break;
            }
        }
    } while (choice != 9);
}

void twth_tree_menu()
{
    two3Tree tree;
    Point point;
    int choice;
    do
    {
        cout << "\n  2-3 tree menu:\n";
        cout << "1. Add an element\n";
        cout << "2. Delete the element\n";
        cout << "3. Searh for the element\n";
        cout << "4. Search elements in range\n";
        cout << "5. Print tree as tree\n";
        cout << "6. Print tree in order\n";
        cout << "7. Generate random elements\n";
        cout << "8. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice)
        {
            case 1:
            {
                double x, y, z;
                cout << "Enter the point you want to add (x,y,z): ";
                cin >> x >> y >> z;
                point = Point(x, y, z);
                tree.add(point);
                break;
            }

            case 2:
            {
                double x, y, z;
                cout << "Enter the point you want to remove (x,y,z): ";
                cin >> x >> y >> z;
                point = Point(x, y, z);
                if (tree.remove(point))
                    cout << "Point removed.\n";
                else
                    cout << "Point not found.\n";
                break;
            }

            case 3:
            {
                double x, y, z;
                cout << "Enter the point you want to find (x,y,z): ";
                cin >> x >> y >> z;
                point = Point(x, y, z);
                if (tree.find_element_by_val(point))
                    cout << "Point found.\n";
                else
                    cout << "Point not found.\n";
                break;
            }

            case 4:
            {
                double xmin, xmax, ymin, ymax, zmin, zmax;
                Point min_modulus, max_modulus;
                cout << "Enter minimum modulus point (x y z): ";
                cin >> xmin >> ymin >> zmin;
                min_modulus = Point(xmin, ymin, zmin);
                cout << "Enter maximum modulus point (x y z): ";
                cin >> xmax >> ymax >> zmax;
                max_modulus = Point(xmax, ymax, zmax);
                {
                    vector<Point> points = tree.find_elements_by_range(min_modulus, max_modulus);
                    cout << "Points in range:\n";
                    for (const Point& p : points) 
                    {
                        p.print();
                        cout << " ";
                    }
                    cout << endl;
                }
                break;
            }

            case 5:
            {
                cout << "Tree structure:\n";
                tree.print_as_tree();
                break;
            }

            case 6:
            {
                cout << "Tree in order:\n";
                tree.print_in_order();
                break;
            }

            case 7:
            {
                int n;
                cout << "How many elements you want to generate?\n";
                cin >> n;
                tree.fillRandom(n);
                break;
            }

            case 8:
            {
                cout << "Exiting...\n";
                break;
            }
            
            default:
            {
                cout << "Invalid input.";
                break;
            }
        }
    } while (choice != 8);
}

void interactiveMode()
{
    int choice;

    cout << "\n   Main menu:\n1. Linked List\n2. Array list\n3. Binary tree\n4. AVL tree\n5. 2-3 tree\n6. Exit\n";
    cout << "Enter your choice: ";
    cin >> choice;

    switch (choice)
    {
        case 1:
        {
            linked_list_menu();
            break;
        }

        case 2:
        {
            array_list_menu();
            break;
        }

        case 3:
        {
            binary_tree_menu();
            break;
        }

        case 4:
        {
            avl_tree_menu();
            break;
        }

        case 5:
        {
            twth_tree_menu();
            break;
        }

        case 6:
        {
            cout << "Exiting...\n";
            break;
        }
        
        default:
        {
            cout << "Invalid input.";
            break;
        }
    }
}

void demoMode()
{
    cout << "\n\tLinked list:\n\n";
    LinkedList list;
    cout << "Creating an empty list:\n";
    list.createEmptyList();
    cout << "\nAdding 9 random elements and the (1.1, 1.1, 1.1) point:\n";
    list.add(1.1, 1.1, 1.1);
    list.fillRandom(9);
    cout << "List: ";
    list.print();
    cout << "\nSearching for (1.1, 1.1, 1.1): " << (list.search(1.1, 1.1, 1.1) ? "Found" : "Not Found") << endl;
    cout << "\nSearching for points in range (0, 0, 0) - (7, 6, 5):\n";
    list.searchInRange(0, 7, 0, 6, 0, 5);
    cout << "\nRemoving (1.1, 1.1, 1.1) from the list...\n";
    list.remove(1.1, 1.1, 1.1);
    cout << "List after removal: ";
    list.print();
    list.clear();

        this_thread::sleep_for(chrono::seconds(1));

    cout << "\n\tArray list:\n\n";
    srand(time(nullptr));
    ArrayList arrayList;
    cout << "Creating an empty list:\n";
    arrayList.createEmptyList();
    cout << "\nAdding 9 random elements and the (2.2, 2.2, 2.2) point:\n";
    arrayList.add(2.2, 2.2, 2.2);
    arrayList.fillRandom(9);
    cout << "List: ";
    arrayList.print();
    cout << "\nSearching for (1.1, 1.1, 1.1): " << (arrayList.search(1.1, 1.1, 1.1) ? "Found" : "Not Found") << endl;
    cout << "\nSearching for points in range (0, 0, 0) - (7, 6, 5):\n";
    arrayList.searchInRange(0, 7, 0, 6, 0, 5);
    cout << "\nRemoving (2.2, 2.2, 2.2) from the list...\n";
    arrayList.remove(2.2, 2.2, 2.2);
    cout << "List after removal: ";
    arrayList.print();
    arrayList.clear();

        this_thread::sleep_for(chrono::seconds(1));

    cout << "\n\tBinary tree:\n\n";
    srand(time(nullptr));
    BST BSTtree;
    cout << "Creating an empty list:\n";
    BSTtree.createEmptyTree();
    cout << "\nAdding 9 random elements and the (3.3, 3.3, 3.3) point:\n";
    BSTtree.add(3.3, 3.3, 3.3);
    BSTtree.fillRandom(9);
    cout << "List: ";
    BSTtree.print();
    cout << "\nSearching for (3.3, 3.3, 3.3): " << (BSTtree.search(3.3, 3.3, 3.3) ? "Found" : "Not Found") << endl;
    cout << "\nSearching for points in range (0, 0, 0) - (7, 6, 5):\n";
    BSTtree.searchInRange(0, 7, 0, 6, 0, 5);
    cout << "\n\nRemoving (3.3, 3.3, 3.3) from the list...\n";
    BSTtree.remove(3.3, 3.3, 3.3);
    cout << "List after removal: ";
    BSTtree.print();
    BSTtree.clear();

        this_thread::sleep_for(chrono::seconds(1));

    cout << "\n\tAVL tree:\n\n";
    srand(time(nullptr));
    AVLTree AVLtree;
    cout << "Creating an empty list:\n";
    AVLtree.createEmptyTree();
    cout << "\nAdding 9 random elements and the (4.4, 4.4, 4.4) point:\n";
    AVLtree.add(4.4, 4.4, 4.4);
    AVLtree.fillRandom(9);
    cout << "List: ";
    AVLtree.print();
    cout << "\nSearching for (3.3, 3.3, 3.3): " << (AVLtree.search(3.3, 3.3, 3.3) ? "Found" : "Not Found") << endl;
    cout << "\nSearching for points in range (0, 0, 0) - (7, 6, 5):\n";
    AVLtree.searchInRange(0, 7, 0, 6, 0, 5);
    cout << "\n\nRemoving (4.4, 4.4, 4.4) from the list...\n";
    AVLtree.remove(4.4, 4.4, 4.4);
    cout << "List after removal: ";
    AVLtree.print();
    AVLtree.clear();

        this_thread::sleep_for(chrono::seconds(1));

    cout << "\n\t2-3 tree:\n\n";
    two3Tree tree23;
    cout << "Creating an empty tree...\n";
    cout << "\nAdding 8 random elements and the (1.1, 1.1, 1.1) and (5.5, 5.5, 5.5) points:\n";
    Point point1 = Point(1.1, 1.1, 1.1);
    Point point2 = Point(5.5, 5.5, 5.5);
    Point point3 = Point(0, 0, 0);
    Point point4 = Point(7, 6, 5);
    tree23.add(point1);
    tree23.add(point2);
    tree23.fillRandom(8);
    cout << "\nTree in order: ";
    tree23.print_in_order();
    cout << "\nTree as a tree: ";
    tree23.print_as_tree();
    cout << "\nSearching for (1.1, 1.1, 1.1): " << (tree23.find_element_by_val(point1) ? "Found" : "Not Found") << endl;
    cout << "\nSearching for points in range (0, 0, 0) - (7, 6, 5):\n";
    tree23.find_elements_by_range(point3, point4);
    cout << "\nRemoving (1.1, 1.1, 1.1) from the tree...\n";
    tree23.remove(point1);
    cout << "Tree after removal: ";
    tree23.print_in_order();
}

void benchmark()
{}

int main() 
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
