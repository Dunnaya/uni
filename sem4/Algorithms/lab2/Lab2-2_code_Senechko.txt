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
            bool isLeaf;

            Node(bool leaf) : isLeaf(leaf) {}
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

            removeRecursive(root, value);

            if (root->keys.empty() && !root->isLeaf) 
            {
                Node* oldRoot = root;
                root = root->children[0];
                delete oldRoot;
            }
        }

        void removeRecursive(Node* node, const Complex& value) 
        {
            if (node->isLeaf) 
            {
                auto it = find(node->keys.begin(), node->keys.end(), value);
                if (it != node->keys.end())
                    node->keys.erase(it);
            }
            else 
            {
                int index = 0;
                while (index < node->keys.size() && !(value < node->keys[index]))
                    index++;
                
                // ckeck index
                if (index < node->children.size())
                    removeRecursive(node->children[index], value);

                //update the key in the parent node
                for (size_t i = 0; i < node->keys.size(); i++) 
                {
                    if (node->keys[i] == value && i < node->children.size() - 1) 
                    {
                        if (!node->children[i + 1]->keys.empty())
                            node->keys[i] = node->children[i + 1]->keys.front();
                    }
                }
            }

            // delete empty leaf nodes
            if (node->isLeaf && node->keys.empty() && node != root) 
            {
                Node* parent = getParent(root, node);
                if (parent) 
                {
                    auto it = find(parent->children.begin(), parent->children.end(), node);
                    if (it != parent->children.end()) 
                    {
                        parent->children.erase(it);
                        
                        if (!parent->children.empty() && !parent->keys.empty()) 
                        {
                            for (size_t i = 0; i < parent->keys.size(); i++) 
                            {
                                if (parent->keys[i] == value && i < parent->children.size() - 1) 
                                {
                                    if (!parent->children[i + 1]->keys.empty())
                                        parent->keys[i] = parent->children[i + 1]->keys.front();
                                }
                            }
                        }
                        
                        delete node;
                    }
                }
            }
        }

        Node* getParent(Node* currentNode, Node* childNode) 
        {
            if (!currentNode || currentNode->isLeaf || currentNode->children.empty())
                return nullptr;

            for (auto child : currentNode->children) 
            {
                if (child == childNode)
                    return currentNode;
            }

            for (auto child : currentNode->children) 
            {
                Node* parent = getParent(child, childNode);
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
            auto it = lower_bound(leaf->keys.begin(), leaf->keys.end(), value);
            leaf->keys.insert(it, value);
        }

        void splitLeaf(Node* leaf) 
        {
            int mid = leaf->keys.size() / 2;
            Node* newLeaf = new Node(true);
            newLeaf->keys.assign(leaf->keys.begin() + mid, leaf->keys.end());
            leaf->keys.erase(leaf->keys.begin() + mid, leaf->keys.end());

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
            if (!parent) return;
            
            auto it = find(parent->children.begin(), parent->children.end(), left);
            if (it == parent->children.end()) return;
            
            int index = it - parent->children.begin();
            
            parent->keys.insert(parent->keys.begin() + index, key);
            parent->children.insert(parent->children.begin() + index + 1, right);
            
            if (parent->keys.size() == order)
                splitNode(parent);
        }

        void splitNode(Node* node) 
        {
            int mid = node->keys.size() / 2;
            Node* newSibling = new Node(node->isLeaf);
            newSibling->keys.assign(node->keys.begin() + mid + 1, node->keys.end());
            Complex midKey = node->keys[mid];
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
    tree.printTree();

    return 0;
}