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

    // Оптимізоване порівняння - порівнюємо квадрати модулів замість обчислення кореня
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

// Структура для зберігання ключа та його ваги разом
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

// Допоміжна функція для створення дерева
TreeNode* createSubtree(const vector<KeyWeight>& keyWeights, const vector<vector<int> >& root, int i, int j) 
{
    if (i > j) return nullptr;
    
    int r = root[i][j];
    TreeNode* node = new TreeNode(keyWeights[r].key, keyWeights[r].weight);
    node->left = createSubtree(keyWeights, root, i, r - 1);
    node->right = createSubtree(keyWeights, root, r + 1, j);
    
    return node;
}

// Функція для розрахунку оптимального бінарного дерева пошуку
TreeNode* buildOptimalBST(const vector<KeyWeight>& keyWeights) 
{
    int n = keyWeights.size();
    if (n == 0) return nullptr;
    
    vector<vector<double> > cost(n + 2, vector<double>(n + 1, 0));
    vector<vector<int> > root(n + 1, vector<int>(n + 1, 0));
    
    // Розрахунок сумарної ваги для підпослідовностей
    vector<double> sumWeights(n + 1, 0);
    for (int i = 0; i < n; i++) 
    {
        sumWeights[i + 1] = sumWeights[i] + keyWeights[i].weight;
    }
    
    // Ініціалізація для порожніх підпослідовностей
    for (int i = 1; i <= n; i++) 
    {
        cost[i][i-1] = 0;  // Порожнє піддерево
    }
    
    // Заповнення таблиці для підпослідовностей довжини від 1 до n
    for (int len = 1; len <= n; len++) 
    {
        for (int i = 0; i <= n - len; i++) 
        {
            int j = i + len - 1;
            cost[i][j] = numeric_limits<double>::infinity();
            
            double sumW = sumWeights[j + 1] - sumWeights[i];
            
            // Пошук оптимального кореня для підпослідовності [i, j]
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

// Функція для виводу дерева у запитаному стилі
void printTree(TreeNode* node, int level = 0, bool left = false, TreeNode* root = nullptr) 
{
    if (node == nullptr) return;
    
    if (root == nullptr) 
    {
        // Перший виклик з коренем
        root = node;
        cout << "Key: " << node->key << endl;
        cout << "weight " << node->weight << endl;
    } 
    else 
    {
        // Нащадки
        for (int i = 0; i < level; i++) cout << "|\t";
        
        if (left) cout << "LEFT ";
        else cout << "RIGHT ";
        
        cout << "Key: " << node->key << endl;
        
        for (int i = 0; i < level; i++) cout << "|\t";
        if (left) cout << "LEFT ";
        else cout << "RIGHT ";
        
        cout << "weight " << node->weight << endl;
    }
    
    // Рекурсивно виводимо лівого та правого нащадків
    printTree(node->left, level + 1, true, root);
    printTree(node->right, level + 1, false, root);
}

// Функція для пошуку елемента у дереві
bool search(TreeNode* root, const Complex& key) 
{
    if (root == nullptr) return false;
    if (root->key == key) return true;
    
    if (key < root->key)
        return search(root->left, key);
    else
        return search(root->right, key);
}

// Допоміжна функція для обчислення модуля комплексного числа (для виведення)
double magnitude(const Complex& c) 
{
    return sqrt(c.real * c.real + c.imaginary * c.imaginary);
}

int main() 
{
    // Приклад використання з комплексними числами
    vector<Complex> keys;
    keys.push_back(Complex(1.0, 2.0));
    keys.push_back(Complex(3.0, 1.0));
    keys.push_back(Complex(2.0, 4.0));
    keys.push_back(Complex(5.0, 2.0));
    keys.push_back(Complex(4.0, 0.0));

    // Вага кожного ключа (ймовірність пошуку)
    vector<double> weights;
    weights.push_back(0.15);
    weights.push_back(0.10);
    weights.push_back(0.25);
    weights.push_back(0.05);
    weights.push_back(0.45);

    // Створюємо пари ключ-вага перед сортуванням
    vector<KeyWeight> keyWeights;
    for (size_t i = 0; i < keys.size(); i++) 
    {
        keyWeights.push_back(KeyWeight(keys[i], weights[i]));
    }

    // Сортуємо за модулем комплексного числа
    sort(keyWeights.begin(), keyWeights.end());

    cout << "Ключі (відсортовані за модулем):" << endl;
    for (size_t i = 0; i < keyWeights.size(); i++) 
    {
        cout << "Key " << i << ": " << keyWeights[i].key 
             << ", Weight: " << keyWeights[i].weight 
             << ", |z| = " << magnitude(keyWeights[i].key) << endl;
    }

    // Побудова оптимального BST
    TreeNode* optimalBST = buildOptimalBST(keyWeights);

    cout << "\nОптимальне бінарне дерево пошуку:" << endl;
    printTree(optimalBST);

    // Перевірка пошуку
    Complex searchKey(3.0, 1.0);
    cout << "\nПошук комплексного числа " << searchKey << ": ";
    if (search(optimalBST, searchKey))
        cout << "Знайдено" << endl;
    else
        cout << "Не знайдено" << endl;

    // Обчислення середньої довжини пошуку
    double totalCost = 0.0;
    for (size_t i = 0; i < keyWeights.size(); i++) 
    {
        TreeNode* current = optimalBST;
        int depth = 1; // Корінь має глибину 1
        
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
    
    cout << "\nСередня довжина пошуку: " << totalCost << endl;

    // Звільнення пам'яті
    delete optimalBST;

    return 0;
}