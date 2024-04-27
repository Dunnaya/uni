/*
0. Weighted, Directed and Undirected Graph.
   Реалізувати структуру даних граф на основі матриці суміжності.
   Реалізувати структуру даних граф на основі структури суміжності.
1.
2. Depth First Search Algorithm (DFS)
3. */

#include <iostream>
#include <vector>
#include <algorithm> //fill(), min()
#include <list>
#include <utility> //pair<>, make_pair() for weight in the adjList
#include <queue>
#include <cassert>
using namespace std;

void interactiveMode();
void demoMode();
void benchmark();

typedef vector<vector<int> > Matrix;

const int INF = INT_MAX/2;

struct GraphMatrix
{
    size_t num_vert;
    Matrix weight_matrix;

    GraphMatrix(size_t num_vert) : num_vert(num_vert), weight_matrix(num_vert)
    {
        for(size_t i = 0; i < num_vert; i++)
        {
            weight_matrix[i].resize(num_vert);
            fill(weight_matrix[i].begin(), weight_matrix[i].end(), INF);
            weight_matrix[i][i] = 0;
        }
    };
};

void add_edge_matrix(GraphMatrix& graph, size_t from, size_t to, int weight, int offset = 0, bool isDirected = true)
{
    from = from - offset;
    to = to - offset;
    graph.weight_matrix[from][to] = weight;
    if (!isDirected)
        graph.weight_matrix[to][from] = weight;
}

void print_matrix(const Matrix& matrix)
{
    for(size_t i = 0; i < matrix.size(); i++)
    {
        for(size_t j = 0; j < matrix[i].size(); j++)
        {
            if(matrix[i][j] == INF)
            {
                cout << "- ";
            } else cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

struct GraphAdjList
{
    vector<list<pair<int, int> > > adjList;
    size_t num_vert;

    GraphAdjList(size_t num_vert) : num_vert(num_vert)
    {
        adjList.resize(num_vert);
    }
};

void add_edge_adjList(GraphAdjList& graph, size_t from, size_t to, int weight, int offset = 0, bool isDirected = true)
{
    from = from - offset;
    to = to - offset;
    assert(from < graph.num_vert);
    assert(to < graph.num_vert);
    graph.adjList[from].push_back(make_pair(to, weight));
    if (!isDirected && from != to)
        graph.adjList[to].push_back(make_pair(from, weight));
}

void print_adjList(const GraphAdjList& graph)
{
    for(size_t i = 0; i < graph.num_vert; i++)
    {
        cout << "Vertex: " << i << ":";
        for(auto iterator = graph.adjList[i].begin(); iterator != graph.adjList[i].end(); ++iterator)
        {
            cout << " -> " << iterator->first << "(" << iterator->second << ")"; 
        }
        cout << endl;
    }
}

GraphAdjList matrix_to_list(const GraphMatrix& matrix) 
{
    GraphAdjList list(matrix.num_vert);

    for(size_t i = 0; i < matrix.num_vert; i++)
    {
        for(size_t j = 0; j < matrix.num_vert; j++)
        {
            if(matrix.weight_matrix[i][j] != INF)
                add_edge_adjList(list, i, j, matrix.weight_matrix[i][j]);
        }
    }
    return list;
}

GraphMatrix list_to_matrix(const GraphAdjList& list) 
{
    GraphMatrix matrix(list.num_vert);

    for(size_t i = 0; i < list.num_vert; i++)
    {
        for(const pair<int, int>& edge : list.adjList[i]) // pair<j, weight>, (j is vert)
        {
            size_t j = edge.first;
            int weight = edge.second;
            add_edge_matrix(matrix, i, j, weight);
        }
    }
    return matrix;
}

void interactiveMode() {}

void demoMode()
{   
    //examples for undirected graph
    /*GraphMatrix graph(5);

    add_edge_matrix(graph, 1, 2, 8, 1, false);
    add_edge_matrix(graph, 1, 3, 3, 1, false);
    add_edge_matrix(graph, 1, 4, 1, 1, false);
    add_edge_matrix(graph, 2, 3, 4, 1, false);
    add_edge_matrix(graph, 2, 5, 2, 1, false);
    add_edge_matrix(graph, 3, 4, 1, 1, false);
    add_edge_matrix(graph, 3, 5, 1, 1, false);
    add_edge_matrix(graph, 4, 5, 8, 1, false);

    print_matrix(graph.weight_matrix);

    GraphAdjList list = matrix_to_list(graph);
    print_adjList(list);*/
    
    GraphAdjList graph_adj(5);

    add_edge_adjList(graph_adj, 1, 2, 8, 1, false);
    add_edge_adjList(graph_adj, 1, 3, 3, 1, false);
    add_edge_adjList(graph_adj, 1, 4, 1, 1, false);
    add_edge_adjList(graph_adj, 2, 3, 4, 1, false);
    add_edge_adjList(graph_adj, 2, 5, 2, 1, false);
    add_edge_adjList(graph_adj, 3, 4, 1, 1, false);
    add_edge_adjList(graph_adj, 3, 5, 1, 1, false);
    add_edge_adjList(graph_adj, 4, 5, 8, 1, false);

    print_adjList(graph_adj);

    GraphMatrix matrix = list_to_matrix(graph_adj);
    print_matrix(matrix.weight_matrix);
}

void benchmark() {}

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
