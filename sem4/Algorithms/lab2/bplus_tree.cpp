#include <iostream>
#include <vector>
#include <cmath>
#include <queue>

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

class BPlusTree 
{ 
private:
    struct Node 
    { 
        vector<Complex> keys;
        vector<Node*> children;
        Node* next;
        bool isLeaf;

        Node(bool leaf) : isLeaf(leaf), next(nullptr) {}
    };
   
    Node* root;
    int order; 

public:
    BPlusTree(int order) : root(nullptr), order(order) {}

    void insert(const Complex& value) 
    {
        if (!root) 
        {
            root = new Node(true);
            root->keys.push_back(value);
        }
        else 
        {
            Node* leaf = findLeaf(root, value);
            insertIntoLeaf(leaf, value);
            if (leaf->keys.size() == order)
                splitLeaf(leaf);
        }
    }

    void remove(const Complex& value) 
    {
        if (!root)
            return;
        
        bool valueRemoved = removeRecursive(root, value, nullptr, 0);
        
        // if root is empty
        if (root->keys.empty()) 
        {
            if (root->isLeaf) 
            {
                // leave the tree empty
            } 
            else if (root->children.size() == 1) 
            {
                // if root has only one child, make it the new root
                Node* oldRoot = root;
                root = root->children[0];
                delete oldRoot;
            }
        }
    }

    bool removeRecursive(Node* node, const Complex& value, Node* parent, int parentIndex) 
    {
        bool valueRemoved = false;
        
        if (node->isLeaf) 
        {
            // finding the key to remove
            for (size_t i = 0; i < node->keys.size(); i++) 
            {
                if (node->keys[i] == value) 
                {
                    node->keys.erase(node->keys.begin() + i);
                    valueRemoved = true;
                    break;
                }
            }
        } 
        else 
        {
            int childIndex = 0;
            while (childIndex < node->keys.size() && !(value < node->keys[childIndex])) 
            {
                childIndex++;
            }
            
            valueRemoved = removeRecursive(node->children[childIndex], value, node, childIndex);
            
            if (valueRemoved && childIndex < node->keys.size() && node->keys[childIndex] == value) 
            {
                Node* rightSubtree = node->children[childIndex + 1];
                Node* leftmost = rightSubtree;
                while (!leftmost->isLeaf) 
                {
                    leftmost = leftmost->children[0];
                }
                
                if (!leftmost->keys.empty())
                    node->keys[childIndex] = leftmost->keys[0];
            }
            // check if the child node is empty after deletion
            Node* child = node->children[childIndex];
            if (child->keys.empty()) 
            {
                if (child->isLeaf) 
                {
                    if (childIndex > 0 && node->children[childIndex - 1]->isLeaf) 
                    {
                        Node* leftSibling = node->children[childIndex - 1];
                        leftSibling->next = child->next;
                    }
                    // delete empty leaf node
                    node->children.erase(node->children.begin() + childIndex);
                    if (childIndex < node->keys.size())
                        node->keys.erase(node->keys.begin() + childIndex);
                    else if (childIndex > 0)
                        node->keys.erase(node->keys.begin() + childIndex - 1);
                    
                    delete child;
                }
            }
        }
        
        return valueRemoved;
    }

    Node* getParent(Node* currentNode, Node* childNode) 
    {
        if (currentNode == childNode)
            return nullptr;
        
        if (currentNode->isLeaf)
            return nullptr;
        
        for (size_t i = 0; i < currentNode->children.size(); i++) 
        {
            if (currentNode->children[i] == childNode)
                return currentNode;
        }
        
        for (size_t i = 0; i < currentNode->children.size(); i++) 
        {
            Node* parent = getParent(currentNode->children[i], childNode);
            if (parent)
                return parent;
        }
        
        return nullptr;
    }

    Node* findLeaf(Node* node, const Complex& value) 
    {
        if (node->isLeaf)
            return node;
        else 
        {
            int index = 0;
            while (index < node->keys.size() && !(value < node->keys[index])) 
            {
                index++;
            }
            return findLeaf(node->children[index], value);
        }
    }

    void insertIntoLeaf(Node* leaf, const Complex& value) 
    {
        // finding position to insert value
        int pos = 0;
        while (pos < leaf->keys.size() && leaf->keys[pos] < value) 
        {
            pos++;
        }
        leaf->keys.insert(leaf->keys.begin() + pos, value);
    }

    void splitLeaf(Node* leaf) 
    {
        int mid = leaf->keys.size() / 2;
        Node* newLeaf = new Node(true);
        newLeaf->keys.assign(leaf->keys.begin() + mid, leaf->keys.end());
        leaf->keys.erase(leaf->keys.begin() + mid, leaf->keys.end());
        
        newLeaf->next = leaf->next;
        leaf->next = newLeaf;

        if (leaf == root) 
        {
            Node* newRoot = new Node(false);
            newRoot->keys.push_back(newLeaf->keys[0]);
            newRoot->children.push_back(leaf);
            newRoot->children.push_back(newLeaf);
            root = newRoot;
        }
        else
            insertIntoParent(leaf, newLeaf->keys[0], newLeaf);
    }

    void insertIntoParent(Node* left, const Complex& key, Node* right) 
    { 
        Node* parent = getParent(root, left);
        // finding position to insert key
        int pos = 0;
        while (pos < parent->keys.size() && parent->keys[pos] < key) 
        {
            pos++;
        }
        
        parent->keys.insert(parent->keys.begin() + pos, key);
        parent->children.insert(parent->children.begin() + pos + 1, right);
        
        if (parent->keys.size() == order)
            splitNode(parent);
    }

    void splitNode(Node* node) 
    {
        int mid = node->keys.size() / 2;
        Complex midKey = node->keys[mid];
        
        Node* newSibling = new Node(node->isLeaf);
        newSibling->keys.assign(node->keys.begin() + mid + 1, node->keys.end());
        node->keys.erase(node->keys.begin() + mid, node->keys.end());

        if (!node->isLeaf) 
        {
            newSibling->children.assign(node->children.begin() + mid + 1, node->children.end());
            node->children.erase(node->children.begin() + mid + 1, node->children.end());
        }

        if (node == root) 
        {
            Node* newRoot = new Node(false);
            newRoot->keys.push_back(midKey);
            newRoot->children.push_back(node);
            newRoot->children.push_back(newSibling);
            root = newRoot;
        }
        else
            insertIntoParent(node, midKey, newSibling);
    }

    void printTree() 
    {
        if (!root) 
        {
            cout << "Tree is empty" << endl;
            return;
        }

        queue<Node*> q;
        q.push(root);

        while (!q.empty()) 
        {
            int size = q.size();
            for (int i = 0; i < size; ++i) 
            {
                Node* node = q.front();
                q.pop();

                for (const auto& key : node->keys)
                    cout << key << " ";
                cout << "| ";

                if (!node->isLeaf) 
                {
                    for (Node* child : node->children)
                        q.push(child);
                }
            }
            cout << endl;
        }
    }
};

int main() 
{
    BPlusTree tree(3); 

    tree.insert(Complex(1, 2));
    tree.insert(Complex(-3, 4));
    tree.insert(Complex(5, -6));
    tree.printTree();

    cout << "\nAfter deleting (-3, 4):" << endl;
    tree.remove(Complex(-3, 4));
    tree.printTree();
    
    cout << "\nAfter returning (-3, 4) and deleting (1, 2):" << endl;
    tree.insert(Complex(-3, 4));
    tree.remove(Complex(1, 2));
    tree.printTree();

    return 0;
}