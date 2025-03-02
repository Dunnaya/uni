#include <iostream>
#include <vector>
#include <list>
#include <climits>
#include <cmath>
#include <map>
#include <queue>
#include <set>
#include <functional>

using namespace std;

// Graph representation using adjacency list
class Graph {
private:
    int V;                      // Number of vertices
    vector<list<pair<int, double> > > adj; // Adjacency list of vertex and weight pairs

public:
    // Constructor
    Graph(int vertices) : V(vertices) {
        adj.resize(vertices);
    }

    // Function to add an edge to graph
    void addEdge(int u, int v, double weight) {
        adj[u].push_back(make_pair(v, weight));
    }

    // Fibonacci Heap Node structure for Dijkstra's algorithm
    struct FibNode {
        int vertex;
        double key;
        FibNode* parent;
        FibNode* child;
        FibNode* left;
        FibNode* right;
        int degree;
        bool marked;

        FibNode(int v, double k) : vertex(v), key(k), parent(nullptr), child(nullptr), 
                                   degree(0), marked(false) {
            left = right = this; // Circular linked list
        }
    };

    // Fibonacci Heap implementation
    class FibonacciHeap {
    private:
        FibNode* minNode;
        int nodeCount;

        // Link two nodes in the circular doubly linked list
        void link(FibNode* node1, FibNode* node2) {
            node1->right = node2;
            node2->left = node1;
        }

        // Insert a node into the circular doubly linked list
        void insertIntoRootList(FibNode* node) {
            if (minNode == nullptr) {
                minNode = node;
                node->left = node->right = node;
            } else {
                link(node, minNode->right);
                link(minNode, node);
                if (node->key < minNode->key) {
                    minNode = node;
                }
            }
        }

        // Remove a node from its circular doubly linked list
        void removeFromList(FibNode* node) {
            node->left->right = node->right;
            node->right->left = node->left;
        }

        // Consolidate the heap after extract min operation
        void consolidate() {
            int maxDegree = static_cast<int>(log2(nodeCount)) + 1;
            vector<FibNode*> degreeArray(maxDegree, nullptr);

            // Create a list of roots
            vector<FibNode*> roots;
            if (minNode == nullptr) return;

            FibNode* current = minNode;
            do {
                roots.push_back(current);
                current = current->right;
            } while (current != minNode);

            for (FibNode* root : roots) {
                // Remove from root list
                if (root->right == root) {
                    minNode = nullptr;
                } else {
                    removeFromList(root);
                    if (root == minNode) {
                        minNode = root->right;
                    }
                }

                // Consolidate based on degree
                while (true) {
                    if (degreeArray[root->degree] == nullptr) {
                        degreeArray[root->degree] = root;
                        break;
                    }

                    FibNode* other = degreeArray[root->degree];
                    degreeArray[root->degree] = nullptr;

                    // Determine which node becomes the parent
                    FibNode* parent, *child;
                    if (root->key <= other->key) {
                        parent = root;
                        child = other;
                    } else {
                        parent = other;
                        child = root;
                    }

                    // Make child a child of parent
                    child->parent = parent;
                    if (parent->child == nullptr) {
                        parent->child = child;
                        child->left = child->right = child;
                    } else {
                        link(child, parent->child->right);
                        link(parent->child, child);
                    }

                    parent->degree++;
                    root = parent;
                }
            }

            // Reconstruct the root list and find the new minimum
            minNode = nullptr;
            for (int i = 0; i < maxDegree; i++) {
                if (degreeArray[i] != nullptr) {
                    if (minNode == nullptr) {
                        // Create a new root list with just this node
                        minNode = degreeArray[i];
                        minNode->left = minNode->right = minNode;
                        minNode->parent = nullptr;
                    } else {
                        // Add to the root list
                        insertIntoRootList(degreeArray[i]);
                        degreeArray[i]->parent = nullptr;
                    }
                }
            }
        }

        // Cut a node from its parent and add to the root list
        void cut(FibNode* child, FibNode* parent) {
            // Remove child from parent's child list
            if (parent->child == child) {
                if (child->right == child) {
                    parent->child = nullptr;
                } else {
                    parent->child = child->right;
                }
            }
            removeFromList(child);
            parent->degree--;

            // Add child to root list
            insertIntoRootList(child);
            child->parent = nullptr;
            child->marked = false;
        }

        // Cascading cut operation
        void cascadingCut(FibNode* node) {
            FibNode* parent = node->parent;
            if (parent != nullptr) {
                if (!node->marked) {
                    node->marked = true;
                } else {
                    cut(node, parent);
                    cascadingCut(parent);
                }
            }
        }

    public:
        FibonacciHeap() : minNode(nullptr), nodeCount(0) {}

        ~FibonacciHeap() {
            // Cleanup would be implemented here in a complete version
        }

        // Check if heap is empty
        bool isEmpty() const {
            return minNode == nullptr;
        }

        // Insert a new node with key k
        FibNode* insert(int vertex, double key) {
            FibNode* node = new FibNode(vertex, key);
            insertIntoRootList(node);
            nodeCount++;
            return node;
        }

        // Extract the minimum node
        FibNode* extractMin() {
            FibNode* min = minNode;
            if (min != nullptr) {
                // Add all of min's children to the root list
                if (min->child != nullptr) {
                    FibNode* child = min->child;
                    do {
                        FibNode* next = child->right;
                        child->parent = nullptr;
                        insertIntoRootList(child);
                        child = next;
                    } while (child != min->child);
                }

                // Remove min from the root list
                if (min->right == min) {
                    // Last node
                    minNode = nullptr;
                } else {
                    minNode = min->right;
                    removeFromList(min);
                    consolidate();
                }

                nodeCount--;
            }
            return min;
        }

        // Decrease the key of a node
        void decreaseKey(FibNode* node, double newKey) {
            if (newKey > node->key) {
                // New key is greater than current key - not allowed
                return;
            }

            node->key = newKey;
            FibNode* parent = node->parent;

            if (parent != nullptr && node->key < parent->key) {
                cut(node, parent);
                cascadingCut(parent);
            }

            if (node->key < minNode->key) {
                minNode = node;
            }
        }
    };

    // Bellman-Ford algorithm to calculate h values for Johnson's algorithm
    bool bellmanFord(int source, vector<double>& h) {
        // Initialize distances
        h.assign(V + 1, INFINITY);
        h[source] = 0;

        // Relax edges V times
        for (int i = 0; i < V; i++) {
            for (int u = 0; u < V; u++) {
                for (auto& edge : adj[u]) {
                    int v = edge.first;
                    double weight = edge.second;
                    if (h[u] != INFINITY && h[u] + weight < h[v]) {
                        h[v] = h[u] + weight;
                    }
                }
            }
        }

        // Check for negative cycles
        for (int u = 0; u < V; u++) {
            for (auto& edge : adj[u]) {
                int v = edge.first;
                double weight = edge.second;
                if (h[u] != INFINITY && h[u] + weight < h[v]) {
                    // Negative cycle exists
                    return false;
                }
            }
        }

        return true;
    }

    // Dijkstra's algorithm using Fibonacci heap
    void dijkstra(int src, vector<double>& dist, const vector<double>& h) {
        // Initialize distances
        dist.assign(V, INFINITY);
        dist[src] = 0;

        // Create a Fibonacci heap
        FibonacciHeap fh;
        map<int, FibNode*> nodeMap; // Map to keep track of vertices in the heap

        // Add all vertices to the heap
        for (int v = 0; v < V; v++) {
            nodeMap[v] = fh.insert(v, dist[v]);
        }

        // Process vertices in order of distance
        while (!fh.isEmpty()) {
            FibNode* u_node = fh.extractMin();
            int u = u_node->vertex;
            
            // Skip unreachable vertices
            if (dist[u] == INFINITY) continue;

            // Update distances to adjacent vertices
            for (auto& edge : adj[u]) {
                int v = edge.first;
                double weight = edge.second;
                
                // Reweight the edge
                double reweighted = weight + h[u] - h[v];
                
                if (dist[u] + reweighted < dist[v]) {
                    dist[v] = dist[u] + reweighted;
                    fh.decreaseKey(nodeMap[v], dist[v]);
                }
            }
            
            delete u_node; // Clean up the extracted node
        }

        // Convert distances back to original weights
        for (int v = 0; v < V; v++) {
            if (dist[v] != INFINITY) {
                dist[v] = dist[v] - h[src] + h[v];
            }
        }
    }

    // Johnson's algorithm for all pairs shortest paths
    vector<vector<double> > johnson() {
        // Create a new graph with an additional vertex
        Graph g(V + 1);
        
        // Copy all edges from original graph
        for (int u = 0; u < V; u++) {
            for (auto& edge : adj[u]) {
                g.addEdge(u, edge.first, edge.second);
            }
        }
        
        // Add edges from the new vertex (V) to all other vertices with weight 0
        for (int v = 0; v < V; v++) {
            g.addEdge(V, v, 0);
        }
        
        // Calculate h values using Bellman-Ford
        vector<double> h;
        if (!g.bellmanFord(V, h)) {
            cout << "Graph contains a negative weight cycle" << endl;
            return vector<vector<double> >();
        }
        
        // Calculate shortest paths for each vertex using Dijkstra's algorithm
        vector<vector<double> > dist(V, vector<double>(V));
        for (int src = 0; src < V; src++) {
            vector<double> d;
            dijkstra(src, d, h);
            dist[src] = d;
        }
        
        return dist;
    }

    // Print the solution
    void printSolution(const vector<vector<double> >& dist) {
        cout << "All-Pairs Shortest Paths:" << endl;
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                if (dist[i][j] == INFINITY) {
                    cout << "INF\t";
                } else {
                    cout << dist[i][j] << "\t";
                }
            }
            cout << endl;
        }
    }
};

int main() {
    // Create a graph with 4 vertices
    int V = 4;
    Graph g(V);

    // Add edges with real number weights
    g.addEdge(0, 1, 3.0);
    g.addEdge(0, 3, 5.0);
    g.addEdge(1, 0, 2.0);
    g.addEdge(1, 2, -2.0);
    g.addEdge(1, 3, 4.0);
    g.addEdge(2, 1, 1.0);
    g.addEdge(2, 3, 2.0);
    g.addEdge(3, 2, -2.0);

    // Run Johnson's algorithm
    vector<vector<double> > dist = g.johnson();
    
    // Print the solution
    if (!dist.empty()) {
        g.printSolution(dist);
    }

    return 0;
}