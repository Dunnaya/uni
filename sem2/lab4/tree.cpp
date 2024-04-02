/*0.Реалізувати структуру даних для опису дерева з довільною кількістю «дітей». У вузлах дерева зберігаються цілі числа. Core.

  1.Реалізувати функцію для додавання елемента до довільного дерева. Передається значення нового елемента, а також мінімальна 
  та максимальна кількість дітей. Елемент додається за такими правилами: 1) якщо кількість дітей поточного елемента менше 
  мінімальної – додаємо до поточного елемента; 2) інакше вибираємо дитину поточного елемента, у якої кількість дітей мінімальна; 
  3) якщо кількість дітей у цієї дитини не більше максимальної – додаємо елемент за тою ж процедурою до цієї дитини; 4) інакше, 
  якщо кількість дітей поточного вузла не більше максимальної – додаємо до поточного вузла; 5) інакше шукаємо дитину з найменшим 
  розміром під-дерева і додаємо до цієї дитини за тою ж процедурою. Core. Provides: ADD_PROC

  2.Реалізувати функцію для виводу дерева з використанням дужок для дітей. Core.
  
  3.Реалізувати функцію для видалення елементу з довільного дерева. Передається значення елементу (якщо є кілька елементів 
  з таким значенням – видаляється будь-який один вузол). Повертається видалене під-дерево. Core.
  
  4.Реалізувати структуру даних для опису бінарного дерева. Реалізувати функцію для додавання елементів відповідно до значення 
  (менші значення наліво, всі інші направо). Core.
  
  5.Побудувати з бінарного дерева прошите дерево на основі симетричного порядку.
  
  6.Реалізувати дерево для подання арифметичних виразів. Підтримуються константи та змінні, а також операції +, -, *, /, 
  піднесення до степеню. Реалізувати спрощення виразів, обчислення константних підвиразів, пошук помилок (наприклад, ділення на 0). 
  Реалізувати обчислення значення виразу для заданих значень змінних. Реалізувати вивід виразу у звичайній математичній нотації з 
  мінімальною кількістю дужок.*/

#include <iostream>
#include <vector>
#include <cassert>
using namespace std;

struct TreeNode {
    int value;

    vector<TreeNode*> children;

    TreeNode(int value): value(value), children() {};

    void add_child(int value, int min, int max) {
        TreeNode* newTreeNode = new TreeNode(value);
        TreeNode* min_child = this->children[0]; 
        if (this->children.size() < min)
        this->children.push_back(newTreeNode);
        else { 
            for(int i = 0; i < this->children.size(); i++) {
                if(min_child->children.size() > this->children[i]->children.size()) 
                min_child = this->children[i];
            }

            if(min_child->children.size() <= max) 
            min_child->children.push_back(newTreeNode);
            else {
                if(this->children.size() <= max)
                this->children.push_back(newTreeNode);
                else { 
                    TreeNode* min_child = this->children[0];  
                    for(int i = 0; i < this->children.size(); i++) {
                        if(min_child->children.size() > this->children[i]->children.size()) 
                        min_child = this->children[i];
                        min_child->children.push_back(newTreeNode);
                    }
                } 
            }
        }
    }

    void print_tree(TreeNode* root) {
        if(root!=nullptr) {
            cout << root->value;
            if (root->children.empty()) {return;}
            cout << "(";
            for(TreeNode* child: root->children) {
                print_tree(child);
                cout << " ";
            }
            cout << ")";
        }
    }

    void delete_tree(TreeNode*& root) {
        for(TreeNode*& child: root->children) {
            delete_tree(child);
        }
        delete root;
        root = nullptr;
    }

    void remove_subtree_by_value(TreeNode*& root, int value) {

        if (root->value == value) {
            print_tree(root);
            delete_tree(root);
            assert(root==nullptr);
            return;
        }

        for(TreeNode*& child: root->children){
            remove_subtree_by_value(child, value);
        }

        root->children.erase(remove(root->children.begin(), root->children.end(), nullptr), root->children.end());
    }
};

struct BinaryTree 
{
    struct Node
    {
        int value;
        Node* left;
        Node* right;
        bool isThreaded;
        Node* predecessor;
        Node* successor;

        Node(int value) : value(value), left(nullptr), right(nullptr), predecessor(nullptr), successor(nullptr) {}

        char op;
        double val;

        Node(char ch) : op(ch), left(nullptr), right(nullptr) {}

        Node(double v) : val(v), left(nullptr), right(nullptr) {}
    };

    bool isOperator (char ch) 
    {
        return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^';
    }

    bool isDigit (char ch) 
    {
        return ch >= '0' && ch <= '9';
    }

    Node* root;

    Node* insertRecursive(Node* root, int value) 
    {
        if (root == nullptr) {
            return new Node(value);
        }

        if (value < root->value) {
            root->left = insertRecursive(root->left, value);
        } else if (value > root->value) {
            root->right = insertRecursive(root->right, value);
        }

        return root;
    }
    
    void insert(int value) 
    {
        root = insertRecursive(root, value);
    }

    void insertThreadedTree(Node*& root, int value)
    {
        if (root == nullptr)
        {
            root = new Node(value);
            return;
        }

        Node* cur = root;
        Node* parent = nullptr;

        while(cur != nullptr)
        {
            parent = cur;
            if (value < cur->value)
            {
                if (cur->left == nullptr)
                {
                    cur->left = new Node(value);
                    cur->left->predecessor = parent->predecessor;
                    cur->left->successor = parent;

                    if(cur->left->predecessor != nullptr)
                    {
                        cur->left->predecessor->successor = cur->left;
                    }
                    parent->predecessor = cur->left;
                    break;
                }
                cur = cur->left;
            } else {
                if (cur->isThreaded || cur->right == nullptr)
                {
                    Node* temp = cur->right;
                    cur->right = new Node(value);
                    cur->isThreaded = false;
                    cur->right->predecessor = cur;
                    cur->right->successor = temp;
                    
                    if(temp != nullptr)
                    temp->predecessor = cur->right;

                    break;
                }
                cur = cur->right;
            }
        }
    }

    void traversal(Node* root)
    {
        if (root == nullptr) return;

        Node* cur = root;
        while(cur->left != nullptr)
        {
            cur = cur->left;
        }

        while(cur != nullptr)
        {
            cout << cur->value << " ";

            if(!cur->isThreaded)
            {
                cur = cur->right;
                while(cur != nullptr && cur->left != nullptr)
                {
                    cur = cur->left;
                }
            } else {
                cur = cur->successor;
            }
        }
    }
};

void interactiveMode() 
{
    int choice;

    do 
    {
        cout << "Menu:\n";
        cout << "1. 1,2,3\n";
        cout << "2. 4,5\n";
        cout << "3. 6\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice)
        {
            case 1:
            {
                do
                {
                    switch(choice) 
                    {
                        cout << "Menu:\n";
                        cout << "1. add_child\n";
                        cout << "2. print_tree\n";
                        cout << "3. remove_subtree_by_value\n";
                        cout << "4. Exit\n";
                        cout << "Enter your choice: ";
                        cin >> choice;

                        case 1: 
                        {
                            
                        }

                        case 2: 
                        {
                        
                            break;
                        }
                        
                        case 3:
                        {

                            break;
                        }

                        default:
                            cout << "Invalid choice.\n";

                        case 4:
                            cout << "Exiting...\n";
                            break;
                    }
                } while (choice != 4);
            } 

            case 2:
            {
                do
                {
                    switch(choice) 
                    {
                        cout << "Menu:\n";
                        cout << "1. insert (binary)\n";
                        cout << "2. insertThreadedTree\n";
                        cout << "3. traversal\n";
                        cout << "4. Exit\n";
                        cout << "Enter your choice: ";
                        cin >> choice;

                        case 1: 
                        {
                            
                        }

                        case 2: 
                        {
                        
                            break;
                        }
                        
                        case 3:
                        {

                            break;
                        }

                        default:
                            cout << "Invalid choice.\n";

                        case 4:
                            cout << "Exiting...\n";
                            break;
                    }
                } while (choice != 4);
            }

            case 3:
            {
                do
                {
                    switch(choice) 
                    {
                        cout << "Menu:\n";
                        cout << "1. \n";
                        cout << "2. \n";
                        cout << "3. \n";
                        cout << "4. Exit\n";
                        cout << "Enter your choice: ";
                        cin >> choice;

                        case 1: 
                        {
                            
                        }

                        case 2: 
                        {
                        
                            break;
                        }
                        
                        case 3:
                        {

                            break;
                        }

                        default:
                            cout << "Invalid choice.\n";

                        case 4:
                            cout << "Exiting...\n";
                            break;
                    }
                } while (choice != 4);
            } 
        }         
    } while (choice != 4);
}

void demoMode() 
{}

int main()
{  
    int choice;

    cout << "Modes:\n";
    cout << "1. Interactive\n" ;
    cout << "2. Demo\n";
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
    }
    return 0;

}
