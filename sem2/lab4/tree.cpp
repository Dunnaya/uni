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
#include <algorithm>
#include <unordered_map>
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
        bool rthread;
        bool lthread;

        Node(int value) : value(value), left(nullptr), right(nullptr), lthread(true), rthread(true) {}
        Node(){}

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
            // Searching for a Node with given value 
        Node *ptr = root; 
        Node *par = NULL; // Parent of key to be inserted 
        while (ptr != NULL) 
        { 
            par = ptr; // Update parent pointer 

            // Moving on left subtree. 
            if (value < ptr->value) 
            { 
                if (ptr -> lthread == false) 
                    ptr = ptr -> left; 
                else
                    break; 
            } 

            // Moving on right subtree. 
            else
            { 
                if (ptr->rthread == false) 
                    ptr = ptr -> right; 
                else
                    break; 
            } 
        } 

        // Create a new node 
        Node *tmp = new Node; 
        tmp -> value = value; 
        tmp -> lthread = true; 
        tmp -> rthread = true; 

        if (par == NULL) 
        { 
            root = tmp; 
            tmp -> left = NULL; 
            tmp -> right = NULL; 
        } 
        else if (value < (par -> value)) 
        { 
            tmp -> left = par -> left; 
            tmp -> right = par; 
            par -> lthread = false; 
            par -> left = tmp; 
        } 
        else
        { 
            tmp -> left = par; 
            tmp -> right = par -> right; 
            par -> rthread = false; 
            par -> right = tmp; 
        } 
  
    }

    Node *inorderSuccessor(struct Node *ptr) 
    { 
        // If rthread is set, we can quickly find 
        if (ptr -> rthread == true) 
            return ptr->right; 
    
        // Else return leftmost child of right subtree 
        ptr = ptr -> right; 
        while (ptr -> lthread == false) 
            ptr = ptr -> left; 
        return ptr; 
    } 
  

    void threadedTraversal()
    {
        if (root == NULL) 
        printf("Tree is empty"); 
    
        // Reach leftmost node 
        struct Node *ptr = root; 
        while (ptr -> lthread == false) 
            ptr = ptr -> left; 
    
        // One by one print successors 
        while (ptr != NULL) 
        { 
            cout << ptr -> value << " "; 
            ptr = inorderSuccessor(ptr); 
        } 
    }
    
    void traversal(Node* cur)
    {   
        if (cur != nullptr) 
        {
            if(cur -> left != nullptr) {traversal(cur->left);};

            cout << cur -> value << " ";

            if(cur -> right != nullptr) {traversal(cur->right);};
        }
    }
};

struct ExprTree 
{

    struct TreeNode 
    {
       string value;
        TreeNode* left;
        TreeNode* right;

        TreeNode(string val) : value(val), left(nullptr), right(nullptr) {}
    };

    string expression;
    TreeNode* root;

    ExprTree(string expr) : expression(expr), root(nullptr) {}

    int precedence(char op) 
    {
        unordered_map <char, int> prec;
        prec['+'] = 1;
        prec['-'] = 1;
        prec['*'] = 2;
        prec['/'] = 2;
        prec['^'] = 3;
        return prec[op];
    }

    string infixToPostfix(const string& expression) 
    {
        string postfix;
        stack<char> operatorStack;

        for (size_t i = 0; i < expression.size(); ++i) 
        {
            if (expression[i] == ' ')
                continue;

            if (isdigit(expression[i]) || expression[i] == '.' || isalpha(expression[i])) 
            {
                string operand;
                while (isdigit(expression[i]) || expression[i] == '.' || isalpha(expression[i])) 
                {
                    operand += expression[i++];
                }
                postfix += operand + ' ';
                --i;
            } else if (expression[i] == '(') {
                operatorStack.push(expression[i]);
            } else if (expression[i] == ')') {
                while (!operatorStack.empty() && operatorStack.top() != '(') {
                    postfix += operatorStack.top();
                    postfix += ' ';
                    operatorStack.pop();
                }
                operatorStack.pop();
            } else {
                while (!operatorStack.empty() && precedence(operatorStack.top()) >= precedence(expression[i])) {
                    postfix += operatorStack.top();
                    postfix += ' ';
                    operatorStack.pop();
                }
                operatorStack.push(expression[i]);
            }
        }

        while (!operatorStack.empty()) 
        {
            postfix += operatorStack.top();
            postfix += ' ';
            operatorStack.pop();
        }

        return postfix;
    }

    TreeNode* constructExpressionTree(const string& postfix) 
    {
        stack<TreeNode*> expressionStack;

        for (size_t i = 0; i < postfix.size(); ++i) 
        {
            if (isdigit(postfix[i]) || postfix[i] == '.' || isalpha(postfix[i])) 
            {
                string operand;
                while (isdigit(postfix[i]) || postfix[i] == '.' || isalpha(postfix[i])) 
                {
                    operand += postfix[i++];
                }
                expressionStack.push(new TreeNode(operand));
                --i;
            } else if (postfix[i] != ' ') {
                TreeNode* opNode = new TreeNode(string(1, postfix[i]));
                opNode->right = expressionStack.top();
                expressionStack.pop();
                opNode->left = expressionStack.top();
                expressionStack.pop();
                expressionStack.push(opNode);
            }
        }

        return expressionStack.top();
    }

    void simplifyExpression(TreeNode* &root) 
    {
        if (!root || (isdigit(root->value[0]) && !isalpha(root->value[0])))
            return;

        bool simplified = true;
        while (simplified) 
        {
            simplified = false;

            if (root->left)
                simplifyExpression(root->left);

            if (root->right)
                simplifyExpression(root->right);

            if (root->value == "+") 
            {
                if (root->right && root->right->value == "0") 
                {
                    delete root->right;
                    root->right = nullptr;
                    TreeNode *temp = root;
                    root = root -> left;
                    delete temp;
                    simplified = true;
                } else if (root->left && root->left->value == "0") {
                    delete root->left;
                    root->left = nullptr;
                    TreeNode *temp = root;
                    root = root->right;
                    delete temp;
                    simplified = true;
                } else if (root->left && root->left->value == "0" && root->right && root->right->value == "0") {
                    delete root->left;
                    delete root->right;
                    root->left = nullptr;
                    root->right = nullptr;
                    root->value = "0";
                    simplified = true;
                }
            } else if (root->value == "-") {
                if (root->right && root->right->value == "0") {
                    delete root->right;
                    root->right = nullptr;
                    simplified = true;
                }
            } else if (root->value == "*") {
                if (root->right && (root->right->value == "0" || (root->left && root->left->value == "0"))) {
                    delete root->left;
                    delete root->right;
                    root->left = nullptr;
                    root->right = nullptr;
                    root->value = "0";
                    simplified = true;
                } else if (root->right && root->right->value == "1") {
                    root = root -> left;
                    simplified = true;
                } else if (root->left && root->left->value == "1") {
                    root = root -> right;
                    simplified = true;
                }
            } else if (root->value == "/") {
                if (root->right && root->right->value == "1") {
                    delete root->right;
                    root->right = nullptr;
                    simplified = true;
                } else if (root->left && root->left->value == "0") {
                    delete root->left;
                    delete root->right;
                    root->left = nullptr;
                    root->right = nullptr;
                    root->value = "0";
                    simplified = true;
                }
            }
        }

        if ((root->value == "+" || root->value == "-") && !root->left && !root->right) 
        {
            delete root;
            root = nullptr;
        }
    }

    double evaluateExpression(TreeNode* root, unordered_map<char, double>& variables) 
    {
        if (!root)
            return 0.0;

        if (isdigit(root->value[0]))
            return stod(root->value);
        else if (isalpha(root->value[0])) {
            char variable = root->value[0];
            if (variables.find(variable) != variables.end())
            {
                return variables[variable];
            } else {
                double value;
                cout << "Enter the value for " << variable << ": ";
                cin >> value;
                variables[variable] = value;
                return value;
            }
        }

        double leftValue = evaluateExpression(root->left, variables);
        double rightValue = evaluateExpression(root->right, variables);

        switch (root->value[0]) {
            case '+': return leftValue + rightValue;
            case '-': return leftValue - rightValue;
            case '*': return leftValue * rightValue;
            case '/': {
                if (rightValue == 0) 
                {
                    throw runtime_error("ERROR: Division by 0");
                }
                return leftValue / rightValue;
            }
            case '^': 
            {
                if (rightValue < 0 && leftValue == 0) 
                {
                    throw runtime_error("ERROR: Division by 0");
                } 
                return pow(leftValue, rightValue);
            }
            default: return 0.0;
        }
    }

    void printExpressionTree(TreeNode* root) 
    {
        if (root) 
        {
            bool isOperator = root->value.size() == 1 && !isdigit(root->value[0]);

            if (isOperator && (root->value == "/" || root->value == "^")) 
            {
                bool leftIsOperator = root->left && (root->left->value.size() == 1 && !isdigit(root->left->value[0]));
                bool rightIsOperator = root->right && (root->right->value.size() == 1 && !isdigit(root->right->value[0]));

                if (leftIsOperator && root->left->value != "*" && !isalpha(root->left->value[0])) 
                {
                    cout << "(";
                    printExpressionTree(root->left);
                    cout << ")";
                } else {
                    printExpressionTree(root->left);
                }

                cout << root->value << " ";

                if (rightIsOperator) 
                {
                    cout << "(";
                    printExpressionTree(root->right);
                    cout << ")";
                } else {
                    printExpressionTree(root->right);
                }
            } else {
                printExpressionTree(root->left);
                cout << root->value << " ";
                printExpressionTree(root->right);
            }
        }
    }
    //Decorative(?) methods for easier calling
    void build() 
    {
        string postfixExpression = infixToPostfix(expression);
        root = constructExpressionTree(postfixExpression);
    }

    void simplify() 
    {
        simplifyExpression(this->root);
    }

    void evaluate(unordered_map<char, double>& variables) 
    {
        double result = evaluateExpression(this->root, variables);
        cout << "Evaluation result: " << result << endl;
    }
    
    void print() 
    {
        printExpressionTree(this->root);
    }
};


void interactiveMode() 
{
    int choice;

    do 
    {
        cout << "\nMenu:\n";
        cout << "1. Tree\n";
        cout << "2. Binary tree\n";
        cout << "3. Expression tree\n";
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
                        case 4:
                        {    
                        cout << "Exiting...\n";
                        break;
                        }
                        
                        default:
                            cout << "Invalid choice.\n";
                    }
                } while (choice != 4);

                break;
            } 

            case 2:
            {
                BinaryTree* tree = new BinaryTree();

                do
                {
                    cout << "\nMenu:\n";
                    cout << "1. insert (binary)\n";
                    cout << "2. traversal\n";
                    cout << "3. insertThreadedTree\n";
                    cout << "4. Threaded Tree Traversal\n";
                    cout << "5. Exit\n";
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
                            tree->traversal(tree->root);
                            break; 
                        }

                        case 3: 
                        {
                            int value;
                            cout << "\nEnter the value: ";
                            cin >> value;
                            tree->insertThreadedTree(value);
                            break;
                        }

                        case 4:
                        {
                            tree->threadedTraversal();
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
                } while (choice != 5);

                break;
            }

            case 3:
            {   
                string expr;
                cout << "Enter the expression: ";
                cin.ignore();
                getline(cin, expr);
                ExprTree tree(expr); 
                tree.build();
                cout << "Expression tree built successfully.";
                do
                {
                    int choice2;
                    cout << "\nMenu:\n";
                    cout << "1. Print expression\n";
                    cout << "2. Simplify expression\n";
                    cout << "3. Evaluate expression\n";
                    cout << "4. Exit\n";
                    cout << "Enter your choice: ";
                    cin >> choice;

                    switch(choice)
                    {
                        case 1: 
                        {
                            tree.print();
                            break;
                        }

                        case 2: 
                        {
                            tree.simplify();
                            cout << "Simplified tree: \n";
                            tree.print();
                            break;
                        }
                        
                        case 3:
                        {
                            unordered_map<char, double> variables;
                            tree.evaluate(variables);
                            break;
                        }

                        case 4:
                           {
                             cout << "Exiting...\n";
                             break;
                           }
                        default:
                            cout << "Invalid choice.\n";
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
{

    cout << "\n ----------------------------------DEMO MODE----------------------------------";
    cout << endl << "Simple tree (TASKS 1-3)";
    cout << "\nROOT = 6 \nAdded children: 4, 8, 1, 2, 3, 5, 9. \nThis is how it looks: \n\t";
    TreeNode* tree = new TreeNode(6);
    tree->add_child(4, 0, 2);
    tree->add_child(8, 2, 3);
    tree->add_child(1, 0, 1);
    tree->add_child(2, 1, 3);
    tree->add_child(3, 2, 4);
    tree->add_child(5, 2, 4);
    tree->add_child(9, 0, 9);
    tree->print_tree();
    
    cout << endl << endl;
    cout << "Binary tree (TASK 4)" << endl;
    cout << "Added children: integers 1 through 10; \nTraversal type: in-order.\n\t";
    BinaryTree *treeBin = new BinaryTree();
    treeBin->insert(4);
    treeBin->insert(2);
    treeBin->insert(1);
    treeBin->insert(3);
    treeBin->insert(6);
    treeBin->insert(5);
    treeBin->insert(7);
    treeBin->insert(10);
    treeBin->insert(9);
    treeBin->insert(8);
    treeBin->traversal(treeBin->root);

    cout << endl << endl;
    cout << "Threaded Binary tree (TASK 5)" << endl;
    cout << "Added children: integers 1 through 11, 48; \nTraversal type: in-order.\n\t";
    BinaryTree *treeBin2 = new BinaryTree();
    treeBin2->insertThreadedTree(4);
    treeBin2->insertThreadedTree(2);
    treeBin2->insertThreadedTree(1);
    treeBin2->insertThreadedTree(3);
    treeBin2->insertThreadedTree(6);
    treeBin2->insertThreadedTree(5);
    treeBin2->insertThreadedTree(7);
    treeBin2->insertThreadedTree(10);
    treeBin2->insertThreadedTree(9);
    treeBin2->insertThreadedTree(8);
    treeBin2->insertThreadedTree(48);
    treeBin2->insertThreadedTree(11);
    treeBin2->threadedTraversal();
    
    cout << endl << endl;
    cout << "Expression Tree(TASK 6)";
    string expression = "0 + 120 * x^2 - 140 * y * 1 + 30 / (10 - 4)";
    cout << endl << "Expression used: " << expression << endl;
    ExprTree treeExpr(expression);
    treeExpr.build();
    cout << "Printing the expression through traversal, before simplifying:\n\t";
    treeExpr.print(); cout << endl;
    cout << "Simplified expression: \n\t";
    treeExpr.simplify();
    treeExpr.print(); cout << endl;
    cout << "For demonstration, variable x = 15.34, variable y = 20.01\n\t";
    unordered_map <char, double> variables;
    variables['x'] = 15.34;
    variables['y'] = 20.01;
    treeExpr.evaluate(variables);

    cout << "\nPLEASE EXPAND THE TERMINAL TO SEE EVERYTHING.";
}

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
