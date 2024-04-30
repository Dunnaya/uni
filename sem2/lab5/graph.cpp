/*          - Weighted, Directed and Undirected Graphs -
0. (1) Matrix.
   (2) Adj List.
1. (5) Is connected?
   (7) Is acyclic?
2. (11) Depth First Search Algorithm (DFS).
3. (14) Dijkstra's algorithm (від заданої вершини до всіх інших).
   (15) Floyd's algorithm (між усіма вершинами графу).
    Modified Floyd's algorithm (між двома заданими вершинами).
4. (18) Topological sort (DFS).
5. (19) Spanning tree (DFS).
6. (21) Min weight spanning tree (Kruskal).*/

#include <iostream>
#include <vector>
#include <algorithm> //fill()
#include <list>
#include <utility> //pair<>, make_pair() for weight in the adjList
#include <queue>
#include <cassert>
#include <stack>
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

GraphMatrix generate_random_graph_matrix(size_t num_vertex, int max_weight, bool isDirected = false) 
{
    GraphMatrix graph(num_vertex);

    int edges_added = 0;
    while (edges_added < num_vertex) 
    {
        size_t u = rand() % num_vertex;
        size_t v = rand() % num_vertex;
        int weight = 1 + rand() % max_weight;

        if (u != v && graph.weight_matrix[u][v] == INF) 
        {
            graph.weight_matrix[u][v] = weight;
            if(!isDirected) graph.weight_matrix[v][u] = weight;
            edges_added++;
        }
    }

    return graph;
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

GraphAdjList generate_random_graph_list(size_t num_vert, int max_weight, bool isDirected = false)
{
    GraphAdjList graph(num_vert);

    for (size_t from = 0; from < num_vert; from++)
    {
        for (size_t to = 0; to < num_vert; to++)
        {
            if (from != to && rand() % 2 == 1)
            {
                int weight = 1 + rand() % max_weight;

                graph.adjList[from].push_back(make_pair(to, weight));
                if (!isDirected && from != to)
                    graph.adjList[to].push_back(make_pair(from, weight));
            }
        }
    }

    return graph;
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
    cout << "The cycle is: ";
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

bool isConnected(const GraphAdjList& graph)
{
    vector<bool> isVisited(graph.num_vert, false);
    size_t start_vert = 0;

    ordinary_DFS(graph, start_vert, isVisited);

    for(bool visited : isVisited)
    {
        if(!visited)
            return false;
    }

    return true;
}

//expected that there are no edges of negative weight
void floyd(const GraphMatrix& graph) //dist between all pairs of vert
{
    Matrix dist = graph.weight_matrix;

    for(size_t k = 0; k < graph.num_vert; k++) //intermediate point
    {
        for(size_t i = 0; i < graph.num_vert; i++)
        {
            for(size_t j = 0; j < graph.num_vert; j++)
            {
                if(dist[i][k] != INF && dist[k][j] != INF && dist[i][k] + dist[k][j] < dist[i][j])
                    dist[i][j] = dist[i][k] + dist[k][j];
            }
        }
    }

    cout << "Shortest paths matrix: \n";
    print_matrix(dist);
}

void floyd_modified(const GraphMatrix& graph, size_t start_vert, size_t end_vert) //dist between two given vert
{
    if (start_vert < 0 || start_vert >= graph.num_vert || end_vert < 0 || end_vert >= graph.num_vert) 
    {
        cout << "Invalid input.\n";
        return;
    }

    Matrix dist = graph.weight_matrix;
    Matrix path(graph.num_vert, vector<int>(graph.num_vert, -1)); //matrix of paths

    for(size_t k = 0; k < graph.num_vert; k++) //intermediate point
    {
        for(size_t i = 0; i < graph.num_vert; i++)
        {
            for(size_t j = 0; j < graph.num_vert; j++)
            {
                if(dist[i][k] != INF && dist[k][j] != INF && dist[i][k] + dist[k][j] < dist[i][j])
                {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    path[i][j] = k;
                }
            }
        }
    }

    cout << "Shortest path between " << start_vert << " and " << end_vert << ": ";
    if(dist[start_vert][end_vert] == INF || start_vert > graph.num_vert || end_vert > graph.num_vert)
    {
        cout << "There is no path.\n";
        return;
    }

    vector<int> shortest_path;
    int vertex = end_vert;

    while (vertex != -1) 
    {
        shortest_path.push_back(vertex);
        vertex = path[start_vert][vertex];
    }
    cout << start_vert << " -> ";
    for (int i = shortest_path.size() - 1; i >= 0; --i) 
    {
        cout << shortest_path[i];
        if (i != 0) cout << " -> ";
    }
    cout << endl;
}

//можна було також реалізувати як вивід рядка матриці з алг. Флойда
void dijkstra(const GraphAdjList& graph, size_t start_vert) //shortest dist from given vert to others
{
    if (start_vert < 0 || start_vert >= graph.num_vert) 
    {
        cout << "Invalid input.\n";
        return;
    }

    vector<int> dist(graph.num_vert, INF);
    dist[start_vert] = 0;

    priority_queue<pair<int, int>, vector<pair<int, int> >, greater<pair<int, int> > > priority_q;
    priority_q.push(make_pair(0, start_vert));

    while(!priority_q.empty())
    {
        int cur_vert = priority_q.top().second;
        priority_q.pop();

        for(const auto& neighbor : graph.adjList[cur_vert])
        {
            int neigh_vert = neighbor.first;
            int weight = neighbor.second;

            if(dist[neigh_vert] > dist[cur_vert] + weight)
            {
                dist[neigh_vert] = dist[cur_vert] + weight;
                priority_q.push(make_pair(dist[neigh_vert], neigh_vert));
            }
        }
    }

    cout << "Shortest distances from vertex " << start_vert << ":\n";
    for(size_t i = 0; i < graph.num_vert; i++)
    {
        if(dist[i] == INF)
            cout << "Vertex " << i << ": no path.";
        else
            cout << "Vertex " << i << ": " << dist[i] << endl;
    }
}

void topological_sort_additional(const GraphAdjList& graph, size_t vert, vector<bool>& isVisited, stack<int>& order)
{
    isVisited[vert] = true;

    for (const auto& neighbor : graph.adjList[vert]) 
    {
        size_t neighbor_vert = neighbor.first;
        if (!isVisited[neighbor_vert]) 
            topological_sort_additional(graph, neighbor_vert, isVisited, order);
    }

    order.push(vert);
}

void topological_sort(const GraphAdjList& graph)
{
    if(hasCycle(graph))
    {
        cout << "\nThe graph contains a cycle. Topological sorting is impossible.\n";
        return;
    }

    vector<bool> isVisited(graph.num_vert, false);
    stack<int> order;

    for(size_t i = 0; i < graph.num_vert; i++)
    {
        if(!isVisited[i])
            topological_sort_additional(graph, i, isVisited, order);
    }

    cout << "Topological sorting order:\n";
    while(!order.empty())
    {
        cout << order.top() << " ";
        order.pop();
    }
    cout << endl;
}

void dfs_st(const GraphAdjList& graph, size_t start_vert, vector<bool>& isVisited, GraphAdjList& st, int& total_weight)
{
    isVisited[start_vert] = true;

    for (const auto& neighbor : graph.adjList[start_vert]) 
    {
        size_t neighbor_vert = neighbor.first;
        int weight = neighbor.second;

        if(!isVisited[neighbor_vert])
        {
            st.adjList[start_vert].push_back(make_pair(neighbor_vert, weight));
            st.adjList[neighbor_vert].push_back(make_pair(start_vert, weight));
            total_weight += weight;
            //add_edge_adjList(mst, start_vert, neighbor_vert, weight);
            dfs_st(graph, neighbor_vert, isVisited, st, total_weight);
        }
    }
}

pair<GraphAdjList, int> spanning_tree(const GraphAdjList& graph)
{
    GraphAdjList st(graph.num_vert);
    vector<bool> isVisited(graph.num_vert, false);
    int total_weight = 0;

    size_t start_vert = 0;
    for (size_t i = 0; i < graph.num_vert; i++) 
    {
        if (!isVisited[i]) 
        {
            start_vert = i;
            break;
        }
    }

    dfs_st(graph, start_vert, isVisited, st, total_weight);

    return make_pair(st, total_weight);
}





void matrix_menu()
{
    int choice;
    cout << "\n   Matrix menu:";
    cout << "\n1. Enter the matrix\n2. Generate random matrix\n3. Print the matrix\n4. Matrix to list\n5. Floyd's algorithm (all shortest paths)\n";
    cout << "6. Modified Floyd's algorithm (shortest path between 2)\n7. Dijkstra's algorithm (shortest path between 1 and the others)\n";
    cout << "8. Check for cyclicity\n9. Topological sort (only for directed acyclic graph)\n";
}

void adjList_menu()
{
    int choice;
    cout << "\n   Adjacency list menu:";
    cout << "\n1. Enter the list\n2. Generate random list\n3. Print the list\n4. List to matrix\n5. Floyd's algorithm (all shortest paths)\n";
    cout << "6. Modified Floyd's algorithm (shortest path between 2)\n7. Dijkstra's algorithm (shortest path between 1 and the others)";
    cout << "8. Check for cyclicity\n9. Topological sort (only for directed acyclic graph)\n";
}

void interactiveMode() 
{
    int type;
    cout << "\nGraph representation types:\n1. Matrix\n2. Adjacency list\n";
    cout << "Select a graph representation type: ";
    cin >> type;

    if(type == 1)
    matrix_menu();
    else if(type == 2)
            adjList_menu();
        else
            cout << "Invalid input.";
}

void demoMode()
{   
    //examples for undirected graph
    GraphMatrix graph(5);

    add_edge_matrix(graph, 1, 2, 8, 1, false);
    add_edge_matrix(graph, 1, 3, 3, 1, false);
    add_edge_matrix(graph, 1, 4, 1, 1, false);
    add_edge_matrix(graph, 2, 3, 4, 1, false);
    add_edge_matrix(graph, 2, 5, 2, 1, false);
    add_edge_matrix(graph, 3, 4, 1, 1, false);
    add_edge_matrix(graph, 3, 5, 1, 1, false);
    add_edge_matrix(graph, 4, 5, 8, 1, false);

    print_matrix(graph.weight_matrix);

    floyd(graph);

    floyd_modified(graph, 0, 2);

    /*GraphAdjList list = matrix_to_list(graph);
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

    isConnected(graph_adj);
    if(isConnected(graph_adj))
      cout << "Graph is connected." << endl;
    else
      cout << "Graph is not connected." << endl;

    cout << "Min spanning tree:\n";
    pair<GraphAdjList, int> st_pair = spanning_tree(graph_adj);
    GraphAdjList st = st_pair.first;
    int total_weight = st_pair.second;
    print_adjList(st);
    cout << "Total weight of the tree: " << total_weight << "\n";

    /*print_adjList(graph_adj);

    GraphMatrix matrix = list_to_matrix(graph_adj);
    print_matrix(matrix.weight_matrix);*/
    size_t start_vert = 0;
    //cout << "DFS (traversal):\n";
    //DFS(graph_adj, start_vert);

    //if(hasCycle(graph_adj))
      //  cout << "Graph has a cycle." << endl;
    //else
      //  cout << "Graph is acyclic." << endl;

    dijkstra(graph_adj, 0);
    topological_sort(graph_adj);

    cout << "Random directed graph (matrix):\n";
    GraphMatrix random_graph_directed = generate_random_graph_matrix(5, 8, true);
    print_matrix(random_graph_directed.weight_matrix);

    cout << "Random undirected graph (matrix):\n";
    GraphMatrix random_graph_undirected = generate_random_graph_matrix(5, 8, false);
    print_matrix(random_graph_undirected.weight_matrix);

    cout << "Random directed graph (list):\n";
    GraphAdjList random_graph_dir = generate_random_graph_list(5, 8, true);
    print_adjList(random_graph_dir);

    cout << "Random graph undirected (list):\n";
    GraphAdjList random_graph_undir = generate_random_graph_list(5, 8, false);
    print_adjList(random_graph_undir);
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
