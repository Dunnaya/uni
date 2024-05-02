/*          - Weighted, Directed and Undirected Graphs -
0. (1) Matrix.
   (2) Adj List.
1. (5) Is connected?
   (6) Connected components.
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
#include <chrono>
#include <thread>
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
        for(const pair<int, int>& edge : list.adjList[i]) // pair<j, weight>, (j is a vert)
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

void has_cycle_DFS(const GraphAdjList& graph, vector<int>& visited, size_t vert, bool& has_cycle) 
{
    visited[vert] = 1;

    for(auto neighbor : graph.adjList[vert]) 
    {
        if(visited[neighbor.first] == 0) 
            has_cycle_DFS(graph, visited, neighbor.first, has_cycle);
        else if(visited[neighbor.first] == 1)
            has_cycle = true;
    }
    
    visited[vert] = 2;
}

bool hasCycle(const GraphAdjList& graph)
{
    vector<int> visited(graph.num_vert, 0);

    bool has_cycle = false;
    for(size_t i = 0; i < graph.num_vert; i++)
    {
        if(!visited[i])
        {
            has_cycle_DFS(graph, visited, i, has_cycle);
        }
    }
    return !has_cycle;
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

void connected_comp_DFS(const GraphAdjList& graph, size_t vert, vector<bool>& isVisited, vector<int>& comp) 
{
    isVisited[vert] = true;
    comp.push_back(vert);

    for(const auto& neighbor : graph.adjList[vert])
    {
        size_t neighbor_vert = neighbor.first;
        if(!isVisited[neighbor_vert])
            connected_comp_DFS(graph, neighbor_vert, isVisited, comp);
    }
}


vector<vector<int> > find_connected_components(const GraphAdjList& graph) 
{
    vector<vector<int> > connected_components;
    vector<bool> isVisited(graph.num_vert, false);

    for (size_t i = 0; i < graph.num_vert; ++i) 
    {
        if (!isVisited[i]) 
        {
            vector<int> component;
            connected_comp_DFS(graph, i, isVisited, component);
            connected_components.push_back(component);
            
            cout << "Connected component: ";
            for (size_t j = 0; j < component.size(); ++j)
            {
                cout << component[j] << " ";
            }
            cout << endl;
        }
    }

    return connected_components;
}

//dist between all pairs of vert
//expected that there are no edges of negative weight
void floyd(const GraphMatrix& graph)
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

//dist between two given vert
void floyd_modified(const GraphMatrix& graph, size_t start_vert, size_t end_vert)
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

//shortest dist from given vert to others
//could also be implemented as the derivation of a row of a matrix from the Floyd's algo
void dijkstra(const GraphAdjList& graph, size_t start_vert)
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
            cout << "Vertex " << i << ": no path.\n";
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

//additional for Kruskal's algorithm
struct Edge 
{
    int from, to, weight;
    Edge(int from, int to, int weight) : from(from), to(to), weight(weight) {}
};

bool edge_compare(const Edge& a, const Edge& b) 
{
    return a.weight < b.weight;
}

int find_parent(int vert, vector<int>& parent) 
{
    if (parent[vert] == -1)
        return vert;
    return find_parent(parent[vert], parent);
}

void union_sets(int x, int y, vector<int>& parent) 
{
    int parentX = find_parent(x, parent);
    int parentY = find_parent(y, parent);
    parent[parentY] = parentX;
}

pair<GraphAdjList, int> kruskal(const GraphAdjList& graph)
{
    vector<Edge> edges;
    for (size_t i = 0; i < graph.num_vert; ++i) 
    {
        for (const auto& neighbor : graph.adjList[i]) 
        {
            edges.push_back(Edge(i, neighbor.first, neighbor.second));
        }
    }

    sort(edges.begin(), edges.end(), edge_compare);

    vector<int> parent(graph.num_vert, -1);
    GraphAdjList mst(graph.num_vert);

    int totalWeight = 0;
    for (const auto& edge : edges) 
    {
        int from = edge.from;
        int to = edge.to;
        int weight = edge.weight;

        int parentFrom = find_parent(from, parent);
        int parentTo = find_parent(to, parent);

        if (parentFrom != parentTo) 
        {
            totalWeight += weight;
            union_sets(parentFrom, parentTo, parent);
            add_edge_adjList(mst, from, to, weight);
        }
    }
    
    cout << "\nMinimal Spanning Tree (Kruskal's Algorithm):" << endl;
    print_adjList(mst);

    cout << "Total Weight of the Minimal Spanning Tree: " << totalWeight << endl;

    return make_pair(mst, totalWeight);
}

GraphMatrix enter_the_matrix(bool isDirected = false)
{
    size_t num_vertices;
    cout << "Enter the number of vertices: ";
    cin >> num_vertices;

    GraphMatrix graph(num_vertices);

    size_t num_edges;
    cout << "Enter the number of edges: ";
    cin >> num_edges;

    cout << "Enter the vertices and weight of the edge (from, to, weight):" << endl;
    for (int i = 0; i < num_edges; ++i) 
    {
        int from, to, weight;
        cout << "Edge " << (i + 1) << ": ";
        cin >> from >> to >> weight;
        if (from < 0 || from >= num_vertices || to < 0 || to >= num_vertices) 
        {
            cout << "Invalid vertex index. Please enter vertices within range [0, " << (num_vertices - 1) << "]." << endl;
            break;
        }
        add_edge_matrix(graph, from, to, weight, 0, isDirected);
    }

    return graph;
}

void matrix_menu()
{
    int choice;

    cout << "\n   Matrix menu:\n";
    cout << "1. Enter the matrix\n";
    cout << "2. Generate random matrix\n";
    cout << "3. Exit\n";
    cout << "Enter your choice: ";
    cin >> choice;

    switch(choice)
    {
        case 1:
        {
            bool isDirected;
            cout << "Is graph directed? (Enter 1 for true or 0 for false): ";
            cin >> isDirected;
            const GraphMatrix matrix = enter_the_matrix(isDirected);
            //print_matrix(matrix.weight_matrix);
            
            int choice;
            do
            {
                cout << "\n   Matrix menu:\n";
                cout << "1. Print the matrix\n";
                cout << "2. Matrix to list\n";
                cout << "3. DFS\n";
                cout << "4. Floyd's algorithm (all shortest paths)\n";
                cout << "5. Modified Floyd's algorithm (shortest path between 2)\n";
                cout << "6. Dijkstra's algorithm\n";
                cout << "7. Check for connectivity\n";
                cout << "8. Check for cyclicity\n";
                cout << "9. Search for connectivity components\n";
                cout << "10. Topological sort (only for directed acyclic graph)\n";
                cout << "11. Find a spanning tree\n";
                cout << "12. Kruskal's algorithm (min weight spanning tree)\n";
                cout << "13. Exit\n";
                cout << "Enter your choice: ";
                cin >> choice;
            
                switch (choice)
                {
                    case 1:
                    {
                        print_matrix(matrix.weight_matrix);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 2:
                    {
                        GraphAdjList adj_list = matrix_to_list(matrix);
                        print_adjList(adj_list);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 3:
                    {
                        int start_vert;
                        cout << "Enter the start vert: ";
                        cin >> start_vert;
                        GraphAdjList adj_list = matrix_to_list(matrix);
                        DFS(adj_list, start_vert);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 4:
                    {
                        floyd(matrix);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 5:
                    {
                        size_t start_vert, end_vert;
                        cout << "\nEnter the start and end vertices: ";
                        cin >> start_vert >> end_vert;
                        floyd_modified(matrix, start_vert, end_vert);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 6:
                    {
                        size_t start_vert;
                        cout << "\nEnter the start vert: ";
                        cin >> start_vert;
                        GraphAdjList adj_list = matrix_to_list(matrix);
                        dijkstra(adj_list, start_vert);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 7:
                    {
                        GraphAdjList adj_list = matrix_to_list(matrix);
                        if(isConnected(adj_list))
                        cout << "Graph is connected." << endl;
                        else
                        cout << "Graph is not connected." << endl;
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 8:
                    {
                        GraphAdjList adj_list = matrix_to_list(matrix);
                        if(hasCycle(adj_list))
                            cout << "Graph has a cycle." << endl;
                        else
                            cout << "Graph is acyclic." << endl;
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 9:
                    {
                        GraphAdjList adj_list = matrix_to_list(matrix);
                        find_connected_components(adj_list);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 10:
                    {
                        GraphAdjList adj_list = matrix_to_list(matrix);
                        topological_sort(adj_list);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 11:
                    {
                        GraphAdjList adj_list = matrix_to_list(matrix);
                        if (!isConnected(adj_list)) 
                        {
                            cout << "Graph isn`t connected.\n";
                            this_thread::sleep_for(chrono::seconds(1));
                            continue;
                        }
                        spanning_tree(adj_list);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 12:
                    {
                        GraphAdjList adj_list = matrix_to_list(matrix);
                        if (!isConnected(adj_list)) 
                        {
                            cout << "Graph isn`t connected.\n";
                            this_thread::sleep_for(chrono::seconds(1));
                            continue;
                        }
                        kruskal(adj_list);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 13:
                    {
                        cout << "Exiting...\n";
                        break;
                    }

                    default:
                        cout << "\nInvalid choice.\n";
                }
            } while (choice != 13);
            break;
        }

        case 2:
        {
            size_t num_vert;
            int max_weight;
            bool isDirected;
            cout << "\nEnter the number of vertices: ";
            cin >> num_vert;
            cout << "Enter the max weight of an edge: ";
            cin >> max_weight;
            cout << "Is graph directed? (Enter 1 for true or 0 for false): ";
            cin >> isDirected;
            const GraphMatrix matrix = generate_random_graph_matrix(num_vert, max_weight, isDirected);

            int choice;
            do
            {
                cout << "\n   Matrix menu:\n";
                cout << "1. Print the matrix\n";
                cout << "2. Matrix to list\n";
                cout << "3. DFS\n";
                cout << "4. Floyd's algorithm (all shortest paths)\n";
                cout << "5. Modified Floyd's algorithm (shortest path between 2)\n";
                cout << "6. Dijkstra's algorithm\n";
                cout << "7. Check for connectivity\n";
                cout << "8. Check for cyclicity\n";
                cout << "9. Search for connectivity components\n";
                cout << "10. Topological sort (only for directed acyclic graph)\n";
                cout << "11. Find a spanning tree\n";
                cout << "12. Kruskal's algorithm (min weight spanning tree)\n";
                cout << "13. Exit\n";
                cout << "Enter your choice: ";
                cin >> choice;

                switch (choice)
                {
                    case 1:
                    {
                        print_matrix(matrix.weight_matrix);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 2:
                    {
                        GraphAdjList adj_list = matrix_to_list(matrix);
                        print_adjList(adj_list);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 3:
                    {
                        int start_vert;
                        cout << "Enter the start vert: ";
                        cin >> start_vert;
                        GraphAdjList adj_list = matrix_to_list(matrix);
                        DFS(adj_list, start_vert);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 4:
                    {
                        floyd(matrix);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 5:
                    {
                        size_t start_vert, end_vert;
                        cout << "\nEnter the start and end vertices: ";
                        cin >> start_vert >> end_vert;
                        floyd_modified(matrix, start_vert, end_vert);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 6:
                    {
                        size_t start_vert;
                        cout << "\nEnter the start vert: ";
                        cin >> start_vert;
                        GraphAdjList adj_list = matrix_to_list(matrix);
                        dijkstra(adj_list, start_vert);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 7:
                    {
                        GraphAdjList adj_list = matrix_to_list(matrix);
                        if(isConnected(adj_list))
                        cout << "Graph is connected." << endl;
                        else
                        cout << "Graph is not connected." << endl;
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 8:
                    {
                        GraphAdjList adj_list = matrix_to_list(matrix);
                        if(hasCycle(adj_list))
                            cout << "Graph has a cycle." << endl;
                        else
                            cout << "Graph is acyclic." << endl;
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 9:
                    {
                        GraphAdjList adj_list = matrix_to_list(matrix);
                        find_connected_components(adj_list);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 10:
                    {
                        GraphAdjList adj_list = matrix_to_list(matrix);
                        topological_sort(adj_list);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 11:
                    {
                        GraphAdjList adj_list = matrix_to_list(matrix);
                        if (!isConnected(adj_list)) 
                        {
                            cout << "Graph isn`t connected.\n";
                            this_thread::sleep_for(chrono::seconds(1));
                            continue;
                        }
                        spanning_tree(adj_list);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 12:
                    {
                        GraphAdjList adj_list = matrix_to_list(matrix);
                        if (!isConnected(adj_list)) 
                        {
                            cout << "Graph isn`t connected.\n";
                            this_thread::sleep_for(chrono::seconds(1));
                            continue;
                        }
                        kruskal(adj_list);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 13:
                    {
                        cout << "Exiting...\n";
                        break;
                    }

                    default:
                        cout << "\nInvalid choice.\n";
                }
                break;
            } while(choice != 13);
        }

        case 3:
            break;

        default:
            cout << "\nInvalid choice.\n";
    }
}

void adjList_menu()
{
    int choice;
    cout << "\n   List menu:\n";
    cout << "1. Enter the list\n";
    cout << "2. Generate random list\n";
    cout << "3. Print the list\n";
    cout << "4. List to matrix\n";
    cout << "5. DFS\n";
    cout << "6. Floyd's algorithm (all shortest paths)\n";
    cout << "7. Modified Floyd's algorithm (shortest path between 2)\n";
    cout << "8. Dijkstra's algorithm\n";
    cout << "9. Check for connectivity?\n";
    cout << "10. Check for cyclicity\n";
    cout << "11. Search for connectivity components.\n";
    cout << "12. Topological sort (only for directed acyclic graph)\n";
    cout << "13. Find a spanning tree\n";
    cout << "14. Kruskal's algorithm (min weight spanning tree)\n";
    cout << "15. Return to the main menu\n";
    cout << "Enter your choice: ";
    cin >> choice;

    switch(choice)
    {
        do
        {
            case 1:
            {
                break;
            }

            case 2:
            {
                break;
            }

            case 3:
            {
                break;
            }

            case 4:
            {
                break;
            }

            case 5:
            {
                break;
            }

            case 6:
            {
                break;
            }

            case 7:
            {
                break;
            }

            case 8:
            {
                break;
            }

            case 9:
            {
                break;
            }

            case 10:
            {
                break;
            }

            case 11:
            {
                break;
            }

            case 12:
            {
                break;
            }

            case 13:
            {
                break;
            }

            case 14:
            {
                break;
            }

            case 15:
            {
                break;
            }
        } while (choice != 15);
    }
}

void interactiveMode() 
{
    int choice;

    cout << "\n   Main menu:\n1. Matrix representation\n2. Adjacency list representation\n3. Exit\n";
    cout << "Enter your choice: ";
    cin >> choice;

    switch (choice)
    {
        case 1:
        {
            matrix_menu();
            break;
        }

        case 2:
        {
            adjList_menu();
            break;
        }

        case 3:
        {
            cout << "Exiting...\n";
            break;
        }
        
        default:
        {
            cout << "Invalid input.";
            break;
        }
    }
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
    DFS(graph_adj, start_vert);

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

    kruskal(graph_adj);
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
