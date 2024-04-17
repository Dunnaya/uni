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
#include <string>
#include <cmath>
#include <stack>
using namespace std;

struct TreeNode 
{
    int value;
    vector<TreeNode*> children;

    TreeNode(int value): value(value), children() {};
    TreeNode() {};

    void add_child(int value, int min, int max) 
    {
        TreeNode* newTreeNode = new TreeNode(value);

        if (this->children.empty()) 
        {
            this->children.push_back(newTreeNode);
            return;
        }

        TreeNode* min_child = this->children[0]; 
        if (this->children.size() < min)
            this->children.push_back(newTreeNode);
        else 
        { 
            for(int i = 0; i < this->children.size(); i++) 
            {
                if(min_child->children.size() > this->children[i]->children.size()) 
                    min_child = this->children[i];
            }

            if(min_child->children.size() <= max) 
                min_child->children.push_back(newTreeNode);
            else 
            {
                if(this->children.size() <= max)
                    this->children.push_back(newTreeNode);
                else 
                { 
                    TreeNode* min_child = this->children[0];  
                    for(int i = 0; i < this->children.size(); i++) 
                    {
                        if(min_child->children.size() > this->children[i]->children.size()) 
                            min_child = this->children[i];
                    }
                    min_child->children.push_back(newTreeNode);
                } 
            }
        }
    }

    void print_tree() 
    {
        if(this!=nullptr) 
        {
            cout << this->value;
            if (this->children.empty()) {return;}
            cout << "(";
            for(TreeNode* child: this->children) 
            {
                child->print_tree();
                cout << " ";
            }
            cout << ")";
        }
    }

    void delete_tree() 
    {
        for (TreeNode*& child : this->children) 
        {
            child->delete_tree();
            delete child;
            child = nullptr;
        }
    }

    void remove_subtree_by_value(int value) 
    {
        if (this == nullptr) return;

        if (this->value == value) 
        {
            print_tree();
            delete_tree();
            return;
        }

        for(size_t i = 0; i < this->children.size(); ++i)
        {
            this->children[i]->remove_subtree_by_value(value);
        }

        this->children.erase(remove(this->children.begin(), this->children.end(), nullptr), this->children.end());
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

    BinaryTree() {this->root = nullptr;};

    Node* root;

    Node* insertRecursive(Node* root, int value) 
    {
        if (root == nullptr) {
            // Create a new node for the value
            root = new Node(value);
        } else if (value < root->value) {
            root->left = insertRecursive(root->left, value);
        } else if (value > root->value) {
            root->right = insertRecursive(root->right, value);
        }
        return root;
    }

    
    void insert(int value) 
    {
        if (this->root == nullptr) 
        {
            this->root = new Node(value);
            return;
        }
        this->root = insertRecursive(this->root, value);
    }

    void insertThreadedTree(int value) 
    {
        if (root == nullptr) 
        {
            root = new Node(value);
            cout << "Created root node with value: " << root->value << endl;
            return;
        }

        Node* cur = root;
        Node* parent = nullptr;

        while (cur != nullptr) 
        {
            parent = cur;
            if (value < cur->value) 
            {
                if (cur->left == nullptr) 
                {
                    cur->left = new Node(value);
                    cout << "Created left child node with value: " << cur->left->value << endl;
                    cur->left->predecessor = parent->predecessor;
                    cur->left->successor = parent;

                    if (cur->left->predecessor != nullptr) 
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
                        cout << "Created right child node with value: " << cur->right->value << endl;
                        cur->isThreaded = false;
                        cur->right->predecessor = cur;
                        cur->right->successor = temp;

                        if (temp != nullptr) 
                        temp->predecessor = cur->right;

                        break;
                    }
                    cur = cur->right;
                }
        }
    }


    void traversal()
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

struct ExprTree 
{
    char op;
    double val;
    ExprTree *left, *right;

    ExprTree(char c) : op(c), left(nullptr), right(nullptr) {}

    ExprTree(double v) : val(v), left(nullptr), right(nullptr) {}

    static ExprTree* buildExpressionTree(const string& expr) 
    {
        if (expr.empty()) return nullptr;

        stack<ExprTree*> st;

        for (int i = 0; i < expr.length(); ++i) 
        {
            if (isDigit(expr[i])) 
            {
                double num = 0;
                while (i < expr.length() && isDigit(expr[i])) 
                {
                    num = num * 10 + (expr[i] - '0');
                    ++i;
                }
                --i;
                st.push(new ExprTree(num));
            } else if (isOperator(expr[i])) {
                ExprTree* node = new ExprTree(expr[i]);
                node->right = st.top(); st.pop();
                node->left = st.top(); st.pop();
                st.push(node);
            } else {
                st.push(new ExprTree(expr[i]));
            }
        }
        return st.top();
    }

    double evaluate(double x, double y) 
    {
        if (!this) return 0;

        if (op) 
        {
            double left_val = left->evaluate(x, y);
            double right_val = right->evaluate(x, y);
            switch (op) 
            {
                case '+': return left_val + right_val;
                case '-': return left_val - right_val;
                case '*': return left_val * right_val;
                case '/': return right_val == 0 ? throw runtime_error("Division by zero!") : left_val / right_val;
                case '^': return pow(left_val, right_val);
            }
        } else if (val) return val;
        else {
            if (op == 'x') return x;
            if (op == 'y') return y;
            return 0;
        }
        return 0;
    }

    void printExpression() 
    {
        if (!this) return;

        if (op) 
        {
            cout << "(";
            left->printExpression();
            cout << " " << op << " ";
            right->printExpression();
            cout << ")";
        } else if (val) cout << val;
        else cout << op;
    }

    static bool isOperator(char c) 
    {
        return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
    }

    static bool isDigit(char c) 
    {
        return c >= '0' && c <= '9';
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
                cout << "\nCreating a new tree!";
                int root_val;
                cout << "\n\nEnter the root value: ";
                cin >> root_val;
                TreeNode* tree = new TreeNode(root_val);
                cout << "\nTree has been created successfully!\n\n";

                do
                {
                    cout << "\nMenu:\n";
                    cout << "1. add_child\n";
                    cout << "2. print_tree\n";
                    cout << "3. remove_subtree_by_value\n";
                    cout << "4. Exit\n";
                    cout << "Enter your choice: ";
                    cin >> choice;

                    switch(choice) 
                    {
                        case 1: 
                        {
                            int root;
                            
                            int value, min, max;
                            cout << "\nEnter the value, min and max: ";
                            cin >> value >> min >> max;
                            if (min > max) {cout << "3rr0r!"; exit(0);}
                            else tree->add_child(value, min, max);
                            
                            break;
                        }

                        case 2: 
                        {
                            tree->print_tree();
                            break;
                        }
                        
                        case 3:
                        {
                            int value;
                            cout << "\nEnter subtree value: ";
                            cin >> value;
                            tree->remove_subtree_by_value(value);
                            break;
                        }

                        default:
                            cout << "Invalid choice.\n";

                        case 4:
                            cout << "Exiting...\n";
                            break;
                    }
                } while (choice != 4);

                break;
            } 

            case 2:
            {
                BinaryTree* tree = new BinaryTree();

                do
                {
                    cout << "Menu:\n";
                    cout << "1. insert (binary)\n";
                    cout << "2. insertThreadedTree\n";
                    cout << "3. traversal\n";
                    cout << "4. Exit\n";
                    cout << "Enter your choice: ";
                    cin >> choice;

                    switch(choice) 
                    {
                        case 1: 
                        {
                            int value;
                            cout << "\nEnter the value: ";
                            cin >> value;
                            tree->insert(value);
                            break;
                        }

                        case 2: 
                        {
                            int value;
                            cout << "\nEnter the value: ";
                            cin >> value;
                            tree->insertThreadedTree(value);
                            break;
                        }
                        
                        case 3:
                        {
                            tree->traversal();
                            break;
                        }

                        default:
                            cout << "Invalid choice.\n";

                        case 4:
                            cout << "Exiting...\n";
                            break;
                    }
                } while (choice != 4);

                break;
            }

            case 3:
            {
                ExprTree* root = nullptr;

                do
                {
                    cout << "Menu:\n";
                    cout << "1. buildExpressionTree\n";
                    cout << "2. printExpression\n";
                    cout << "3. evaluate\n";
                    cout << "4. Exit\n";
                    cout << "Enter your choice: ";
                    cin >> choice;

                    switch(choice)
                    {
                        case 1: 
                        {
                            string expression;
                            cout << "Enter the expression: ";
                            cin.ignore();
                            getline(cin, expression);
                            root->buildExpressionTree(expression);
                            break;
                        }

                        case 2: 
                        {
                            root->printExpression();
                            break;
                        }
                        
                        case 3:
                        {
                            double x, y;
                            cout <<"\nEnter x and y: ";
                            cin >> x >> y;
                            root->evaluate(x,y);
                            break;
                        }

                        default:
                            cout << "Invalid choice.\n";

                        case 4:
                            cout << "Exiting...\n";
                            break;
                    }
                } while (choice != 4);

                break;
            }

            default:
            cout << "Invalid choice.\n"; 

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
