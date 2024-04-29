/*
0. Weighted, Directed and Undirected Graph.
   Реалізувати структуру даних граф на основі матриці суміжності.
   Реалізувати структуру даних граф на основі структури суміжності.
1. Is acyclic?
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

void ordinary_DFS(const GraphAdjList& graph, size_t vert, vector<bool>& isVisited) 
{
    isVisited[vert] = true;
    cout << vert << " ";

    for(const auto& neighbor : graph.adjList[vert])
    {
        size_t neighbor_vert = neighbor.first;
        if(!isVisited[neighbor_vert])
            ordinary_DFS(graph, neighbor_vert, isVisited);
    }
}

bool compare_neighbors(const pair<int, int>& a, const pair<int, int>& b) //for weighted_DFS
{
    return a.second < b.second; //second is a weight
}

void weighted_DFS(const GraphAdjList& graph, size_t vert, vector<bool> isVisited)
{
    isVisited[vert] = true;
    cout << vert << " ";

    //just sorting the neighboring vertices by increasing the edge weight
    vector<pair<int, int> > sorted_neighbors(graph.adjList[vert].begin(), graph.adjList[vert].end());
    sort(sorted_neighbors.begin(), sorted_neighbors.end(), compare_neighbors);

    for(const auto& neighbor : graph.adjList[vert])
    {
        size_t neighbor_vert = neighbor.first;
        if(!isVisited[neighbor_vert])
            weighted_DFS(graph, neighbor_vert, isVisited);
    }
}

void DFS(const GraphAdjList& graph, size_t start_vert)
{
    vector<bool> isVisited(graph.num_vert, false);

    cout << "DFS with ordinary neighbor order: ";
    ordinary_DFS(graph, start_vert, isVisited);
    cout << endl;

    fill(isVisited.begin(), isVisited.end(), false); //reseting attendance

    cout << "DFS with weighted neighbor order: ";
    weighted_DFS(graph, start_vert, isVisited);
    cout << endl;
}

bool has_cycle_DFS(const GraphAdjList& graph, size_t vert, vector<bool>& isVisited, size_t start_vert) 
{
    isVisited[vert] = true;
    cout << vert << " ";

    for(const auto& neighbor : graph.adjList[vert])
    {
        size_t neighbor_vert = neighbor.first;
        if(!isVisited[neighbor_vert])
        {
            if(has_cycle_DFS(graph, neighbor_vert, isVisited, vert))
            return true;
        }
        else if(neighbor_vert != start_vert)
                return true;
    }
    return false;
}

bool hasCycle(const GraphAdjList& graph)
{
    vector<bool> isVisited(graph.num_vert, false);

    for(size_t i = 0; i < graph.num_vert; i++)
    {
        if(!isVisited[i])
        {
            if(has_cycle_DFS(graph, i, isVisited, -1))
                return true;
        }
    }
    return false;
}

void interactiveMode() 
{}

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

    /*GraphMatrix matrix = list_to_matrix(graph_adj);
    print_matrix(matrix.weight_matrix);*/
    size_t start_vert = 0;
    cout << "DFS (traversal):\n";
    DFS(graph_adj, start_vert);

    if(hasCycle(graph_adj))
        cout << "Graph has a cycle." << endl;
    else
        cout << "Graph is acyclic." << endl;
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
