/*Реалізувати оптимальне бінарне дерево пошуку (динамічне програмування)*/
#include <iostream>
#include <ostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
using namespace std;

struct Complex 
{
    double real;
    double imaginary;

    Complex(double r = 0.0, double i = 0.0) : real(r), imaginary(i) {}

    Complex operator+(const Complex& other) const 
    {
        return Complex(real + other.real, imaginary + other.imaginary);
    }

    // comparing squared magnitudes not to compute square root
    bool operator<(const Complex& other) const 
    {
        return (real * real + imaginary * imaginary) < (other.real * other.real + other.imaginary * other.imaginary);
    }

    bool operator==(const Complex& other) const 
    {
        return real == other.real && imaginary == other.imaginary;
    }

    friend ostream& operator<<(ostream& os, const Complex& c) 
    {
        if (c.imaginary >= 0)
            os << c.real << "+" << c.imaginary << "i";
        else
            os << c.real << "-" << -c.imaginary << "i";
        return os;
    }
};

struct KeyWeight 
{
    Complex key;
    double weight;
    
    KeyWeight(const Complex& k = Complex(), double w = 0.0) : key(k), weight(w) {}
    
    bool operator<(const KeyWeight& other) const 
    {
        return key < other.key;
    }
};

struct TreeNode
{
    Complex key;
    double weight;
    TreeNode* left;
    TreeNode* right;

    TreeNode(const Complex& k, double w) : key(k), weight(w), left(nullptr), right(nullptr) {}
    ~TreeNode() 
    {
        delete left;
        delete right;
    }
};

TreeNode* createSubtree(const vector<KeyWeight>& keyWeights, const vector<vector<int> >& root, int i, int j) 
{
    if (i > j) return nullptr;
    
    int r = root[i][j];
    TreeNode* node = new TreeNode(keyWeights[r].key, keyWeights[r].weight);
    node->left = createSubtree(keyWeights, root, i, r - 1);
    node->right = createSubtree(keyWeights, root, r + 1, j);
    
    return node;
}

TreeNode* buildOptimalBST(const vector<KeyWeight>& keyWeights) 
{
    int n = keyWeights.size();
    if (n == 0) return nullptr;
    
    vector<vector<double> > cost(n + 2, vector<double>(n + 1, 0));
    vector<vector<int> > root(n + 1, vector<int>(n + 1, 0));
    
    // computing cumulative weights
    vector<double> sumWeights(n + 1, 0);
    for (int i = 0; i < n; i++) 
    {
        sumWeights[i + 1] = sumWeights[i] + keyWeights[i].weight;
    }
    
    // initializing cost for empty subtrees
    for (int i = 1; i <= n; i++) 
    {
        cost[i][i-1] = 0;
    }
    
    for (int len = 1; len <= n; len++) 
    {
        for (int i = 0; i <= n - len; i++) 
        {
            int j = i + len - 1;
            cost[i][j] = numeric_limits<double>::infinity();
            
            double sumW = sumWeights[j + 1] - sumWeights[i];
            
            // search for the best root
            for (int r = i; r <= j; r++) 
            {
                double c = cost[i][r-1] + cost[r+1][j] + sumW;
                if (c < cost[i][j]) 
                {
                    cost[i][j] = c;
                    root[i][j] = r;
                }
            }
        }
    }
    
    return createSubtree(keyWeights, root, 0, n - 1);
}

void printTree(TreeNode* node, int level = 0, bool left = false, TreeNode* root = nullptr) 
{
    if (node == nullptr) return;
    
    if (root == nullptr) 
    {
        root = node;
        cout << "Key: " << node->key << endl;
        cout << "weight " << node->weight << endl;
    } 
    else 
    {
        for (int i = 0; i < level; i++) cout << "|\t";
        
        if (left) cout << "LEFT ";
        else cout << "RIGHT ";
        
        cout << "Key: " << node->key << endl;
        
        for (int i = 0; i < level; i++) cout << "|\t";
        if (left) cout << "LEFT ";
        else cout << "RIGHT ";
        
        cout << "weight " << node->weight << endl;
    }
    
    printTree(node->left, level + 1, true, root);
    printTree(node->right, level + 1, false, root);
}

bool search(TreeNode* root, const Complex& key) 
{
    if (root == nullptr) return false;
    if (root->key == key) return true;
    
    if (key < root->key)
        return search(root->left, key);
    else
        return search(root->right, key);
}

double magnitude(const Complex& c) 
{
    return sqrt(c.real * c.real + c.imaginary * c.imaginary);
}

int main() 
{
    vector<Complex> keys;
    keys.push_back(Complex(1.0, 2.0));
    keys.push_back(Complex(-1.0, 1.0));
    keys.push_back(Complex(3.0, 0.0));
    keys.push_back(Complex(0.0, -2.0));
    keys.push_back(Complex(-2.0, -2.0));

    vector<double> weights;
    weights.push_back(0.1);
    weights.push_back(0.2);
    weights.push_back(0.3);
    weights.push_back(0.15);
    weights.push_back(0.25);

    // creating keyWeight pairs
    vector<KeyWeight> keyWeights;
    for (size_t i = 0; i < keys.size(); i++) 
    {
        keyWeights.push_back(KeyWeight(keys[i], weights[i]));
    }

    // sorting keys by their module
    sort(keyWeights.begin(), keyWeights.end());
    cout << "Keys (sorted by module):" << endl;
    for (size_t i = 0; i < keyWeights.size(); i++) 
    {
        cout << "Key " << i << ": " << keyWeights[i].key 
             << ", Weight: " << keyWeights[i].weight 
             << ", |z| = " << magnitude(keyWeights[i].key) << endl;
    }

    // creating optimal BST
    TreeNode* optimalBST = buildOptimalBST(keyWeights);
    cout << "\nOptimal binary search tree:" << endl;
    printTree(optimalBST);

    // search revision
    Complex searchKey(0.0, -2.0);
    cout << "\nFinding the complex number " << searchKey << ": ";
    if (search(optimalBST, searchKey))
        cout << "Found" << endl;
    else
        cout << "Not found" << endl;

    // average search length
    double totalCost = 0.0;
    for (size_t i = 0; i < keyWeights.size(); i++) 
    {
        TreeNode* current = optimalBST;
        int depth = 1; // root depth is 1
        
        while (current != nullptr && !(current->key == keyWeights[i].key)) 
        {
            depth++;
            if (keyWeights[i].key < current->key)
                current = current->left;
            else
                current = current->right;
        }
        
        totalCost += depth * keyWeights[i].weight;
    }
    
    cout << "\nAverage search length: " << totalCost << endl;

    // cleaning up
    delete optimalBST;
    return 0;
}