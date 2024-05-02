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

bool has_cycle_DFS(const GraphAdjList& graph, size_t vert, vector<bool>& isVisited, size_t start_vert) 
{
    isVisited[vert] = true;

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

    cout << "\nSpanning tree:" << endl;
    print_adjList(st);

    cout << "Total weight of the ST: " << total_weight << endl;

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
    
    cout << "\nMinimal spanning tree:" << endl;
    print_adjList(mst);

    cout << "Total weight of the MST: " << totalWeight << endl;

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

GraphAdjList enter_the_list(bool isDirected = false)
{
    size_t num_vertices;
    cout << "Enter the number of vertices: ";
    cin >> num_vertices;

    GraphAdjList graph(num_vertices);

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
        add_edge_adjList(graph, from, to, weight, 0, isDirected);
    }

    return graph;
}

void matrix_menu()
{
    int choice;

    cout << "\n  Adjacency matrix menu:\n";
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
                        cout << endl;
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
                        cout << endl;
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

    cout << "\n  Adjacency list menu:\n";
    cout << "1. Enter the list\n";
    cout << "2. Generate random list\n";
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
            const GraphAdjList adj_list = enter_the_list(isDirected);
            //print_list(matrix.weight_matrix);
            
            int choice;
            do
            {
                cout << "\n   List menu:\n";
                cout << "1. Print the list\n";
                cout << "2. List to matrix\n";
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
                        print_adjList(adj_list);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 2:
                    {
                        GraphMatrix matrix = list_to_matrix(adj_list);
                        print_matrix(matrix.weight_matrix);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 3:
                    {
                        int start_vert;
                        cout << "Enter the start vert: ";
                        cin >> start_vert;
                        DFS(adj_list, start_vert);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 4:
                    {
                        GraphMatrix matrix = list_to_matrix(adj_list);
                        floyd(matrix);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 5:
                    {
                        GraphMatrix matrix = list_to_matrix(adj_list);
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
                        dijkstra(adj_list, start_vert);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 7:
                    {
                        if(isConnected(adj_list))
                        cout << "Graph is connected." << endl;
                        else
                        cout << "Graph is not connected." << endl;
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 8:
                    {
                        if(hasCycle(adj_list))
                            cout << "Graph has a cycle." << endl;
                        else
                            cout << "Graph is acyclic." << endl;
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 9:
                    {
                        find_connected_components(adj_list);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 10:
                    {
                        topological_sort(adj_list);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 11:
                    {
                        if (!isConnected(adj_list)) 
                        {
                            cout << "Graph isn`t connected.\n";
                            this_thread::sleep_for(chrono::seconds(1));
                            continue;
                        }
                        spanning_tree(adj_list);
                        cout << endl;
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 12:
                    {
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
            const GraphAdjList adj_list = generate_random_graph_list(num_vert, max_weight, isDirected);

            int choice;
            do
            {
                cout << "\n   List menu:\n";
                cout << "1. Print the list\n";
                cout << "2. List to matrix\n";
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
                        print_adjList(adj_list);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 2:
                    {
                        GraphMatrix matrix = list_to_matrix(adj_list);
                        print_matrix(matrix.weight_matrix);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 3:
                    {
                        int start_vert;
                        cout << "Enter the start vert: ";
                        cin >> start_vert;
                        DFS(adj_list, start_vert);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 4:
                    {
                        GraphMatrix matrix = list_to_matrix(adj_list);
                        floyd(matrix);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 5:
                    {
                        GraphMatrix matrix = list_to_matrix(adj_list);
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
                        dijkstra(adj_list, start_vert);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 7:
                    {
                        if(isConnected(adj_list))
                        cout << "Graph is connected." << endl;
                        else
                        cout << "Graph is not connected." << endl;
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 8:
                    {
                        if(hasCycle(adj_list))
                            cout << "Graph has a cycle." << endl;
                        else
                            cout << "Graph is acyclic." << endl;
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 9:
                    {
                        find_connected_components(adj_list);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 10:
                    {
                        topological_sort(adj_list);
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 11:
                    {
                        if (!isConnected(adj_list)) 
                        {
                            cout << "Graph isn`t connected.\n";
                            this_thread::sleep_for(chrono::seconds(1));
                            continue;
                        }
                        spanning_tree(adj_list);
                        cout << endl;
                        this_thread::sleep_for(chrono::seconds(1));
                        continue;
                    }

                    case 12:
                    {
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
        }

        case 3:
            break;

        default:
            cout << "\nInvalid choice.\n";
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
//------------------LIST------------------------------------------------------------------------------------------------------

    cout << "\n-----Representation in the form of an adjacency LIST-----\n";
    this_thread::sleep_for(chrono::seconds(1));

    //-----BLOCK 0------

    cout << "\n\t\tGeneration of random graphs:\n";
    this_thread::sleep_for(chrono::seconds(1));
    cout << "\nGenerating a random directed graph with 4 vertices, max weight of an edge is 9:\n";
    this_thread::sleep_for(chrono::seconds(2));
    GraphAdjList random_graph_dir = generate_random_graph_list(4, 9, true);
    print_adjList(random_graph_dir);
    this_thread::sleep_for(chrono::seconds(3));

    cout << "\nGenerating a random undirected graph with 5 vertices, max weight of an edge is 7:\n";
    this_thread::sleep_for(chrono::seconds(2));
    GraphAdjList random_graph_undir = generate_random_graph_list(5, 7, false);
    print_adjList(random_graph_undir);
    this_thread::sleep_for(chrono::seconds(3));

    cout << "\n----------------------------------------------------------------------\n";
    this_thread::sleep_for(chrono::seconds(1));

    cout << "\n\t\tCreating a specific graph:\n";
    this_thread::sleep_for(chrono::seconds(1));
    cout << "\nCreating a directed connected acyclic graph with 5 vertices and 4 edges:\n";
    this_thread::sleep_for(chrono::seconds(2));

    GraphAdjList graph_adj(5);

    add_edge_adjList(graph_adj, 1, 2, 5, 1, true);
    add_edge_adjList(graph_adj, 2, 5, 8, 1, true);
    add_edge_adjList(graph_adj, 4, 3, 3, 1, true);
    add_edge_adjList(graph_adj, 5, 4, 5, 1, true);

    print_adjList(graph_adj);

    this_thread::sleep_for(chrono::seconds(3));

    cout << "\nConverting this list into a matrix:\n";
    this_thread::sleep_for(chrono::seconds(1));
    GraphMatrix l_to_m = list_to_matrix(graph_adj);
    print_matrix(l_to_m.weight_matrix);
    this_thread::sleep_for(chrono::seconds(3));

    //-----BLOCK 1------

    cout << "\nChecking the graph for connectivity:\n";
    this_thread::sleep_for(chrono::seconds(1));
    if(isConnected(graph_adj))
      cout << "Graph is connected.\n" << endl;
    else
      cout << "Graph is not connected.\n" << endl;
    this_thread::sleep_for(chrono::seconds(1));

    cout << "Looking for connectivity components:\n";
    this_thread::sleep_for(chrono::seconds(1));
    find_connected_components(graph_adj);
    this_thread::sleep_for(chrono::seconds(1));

    cout << "\nChecking the graph for cyclicity:\n";
    this_thread::sleep_for(chrono::seconds(1));
    if(hasCycle(graph_adj))
        cout << "Graph has a cycle.\n" << endl;
    else
        cout << "Graph is acyclic.\n" << endl;
    this_thread::sleep_for(chrono::seconds(1));

    //-----BLOCK 2------

    cout << "DFS from 1st vertex:\n";
    this_thread::sleep_for(chrono::seconds(1));
    int start_vert_list_DFS = 1;
    DFS(graph_adj, start_vert_list_DFS);
    this_thread::sleep_for(chrono::seconds(1));

    //-----BLOCK 3------

    cout << "\nFloyd's algorithm (all shortest paths):\n";
    this_thread::sleep_for(chrono::seconds(1));
    floyd(l_to_m);
    this_thread::sleep_for(chrono::seconds(2));

    cout << "\nDijkstra's algorithm:";
    cout << "\n(start vert = 4)\n";
    this_thread::sleep_for(chrono::seconds(1));
    int start_vert_dijkstra_list = 4;
    dijkstra(graph_adj, start_vert_dijkstra_list);
    this_thread::sleep_for(chrono::seconds(2));

    cout << "\nModified Floyd's algorithm (shortest path between 2 vertices):";
    this_thread::sleep_for(chrono::seconds(1));
    cout << "\n(start vert = 4; end vert = 2)\n";
    this_thread::sleep_for(chrono::seconds(1));
    int start_vert_list_floyd = 4;
    int end_vert_list_floyd = 2;
    floyd_modified(l_to_m, start_vert_list_floyd, end_vert_list_floyd);
    this_thread::sleep_for(chrono::seconds(2));

    //-----BLOCK 4------
    cout << "\nTopological sorting (works only for directed acyclic graphs):\n";
    this_thread::sleep_for(chrono::seconds(1));
    topological_sort(graph_adj);
    this_thread::sleep_for(chrono::seconds(1));

    //-----BLOCK 5------

    cout << "\nFind a spanning tree:";
    this_thread::sleep_for(chrono::seconds(1));
    spanning_tree(graph_adj);
    this_thread::sleep_for(chrono::seconds(2));

    //-----BLOCK 6------

    cout << "\nKruskal's algorithm (min weight spanning tree):";
    this_thread::sleep_for(chrono::seconds(1));
    kruskal(graph_adj);
    this_thread::sleep_for(chrono::seconds(3));
    
//------------------MATRIX----------------------------------------------------------------------------------------------------

    cout << "\n-----Representation in the form of an adjacency MATRIX-----\n";
    this_thread::sleep_for(chrono::seconds(1));

    //-----BLOCK 0------

    cout << "\n\t\tGeneration of random graphs:\n";
    this_thread::sleep_for(chrono::seconds(1));
    cout << "\nGenerating a random directed graph with 6 vertices, max weight of an edge is 4:\n";
    this_thread::sleep_for(chrono::seconds(2));
    GraphMatrix random_graph_directed = generate_random_graph_matrix(6, 4, true);
    print_matrix(random_graph_directed.weight_matrix);
    this_thread::sleep_for(chrono::seconds(3));

    cout << "\nGenerating a random undirected graph with 3 vertices, max weight of an edge is 8:\n";
    this_thread::sleep_for(chrono::seconds(2));
    GraphMatrix random_graph_undirected = generate_random_graph_matrix(3, 8, false);
    print_matrix(random_graph_undirected.weight_matrix);
    this_thread::sleep_for(chrono::seconds(3));

    cout << "\n------------------------------------------------------------------------------\n";
    this_thread::sleep_for(chrono::seconds(1));

    cout << "\n\t\tCreating a specific graph:\n";
    this_thread::sleep_for(chrono::seconds(1));
    cout << "\nCreating an undirected disconnected graph with cycles with 5 vertices and 5 edges:\n";
    this_thread::sleep_for(chrono::seconds(2));

    GraphMatrix graph_matr(5);

    add_edge_matrix(graph_matr, 1, 2, 3, 1, false);
    add_edge_matrix(graph_matr, 1, 4, 5, 1, false);
    add_edge_matrix(graph_matr, 2, 4, 8, 1, false);
    add_edge_matrix(graph_matr, 2, 5, 5, 1, false);
    add_edge_matrix(graph_matr, 4, 5, 2, 1, false);

    print_matrix(graph_matr.weight_matrix);

    this_thread::sleep_for(chrono::seconds(3));

    cout << "\nConverting this matrix into a list:\n";
    this_thread::sleep_for(chrono::seconds(1));
    GraphAdjList m_to_l = matrix_to_list(graph_matr);
    print_adjList(m_to_l);
    this_thread::sleep_for(chrono::seconds(3));

    //-----BLOCK 1------

    cout << "\nChecking the graph for connectivity:\n";
    this_thread::sleep_for(chrono::seconds(1));
    if(isConnected(m_to_l))
      cout << "Graph is connected.\n" << endl;
    else
      cout << "Graph is not connected.\n" << endl;
    this_thread::sleep_for(chrono::seconds(1));

    cout << "Looking for connectivity components:\n";
    this_thread::sleep_for(chrono::seconds(1));
    find_connected_components(m_to_l);
    this_thread::sleep_for(chrono::seconds(1));

    cout << "\nChecking the graph for cyclicity:\n";
    this_thread::sleep_for(chrono::seconds(1));
    if(hasCycle(m_to_l))
        cout << "Graph has a cycle.\n" << endl;
    else
        cout << "Graph is acyclic.\n" << endl;
    this_thread::sleep_for(chrono::seconds(1));

    //-----BLOCK 2------

    cout << "DFS from 3rd vertex:\n";
    this_thread::sleep_for(chrono::seconds(1));
    int start_vert_matr_DFS = 3;
    DFS(m_to_l, start_vert_matr_DFS);
    this_thread::sleep_for(chrono::seconds(1));

    //-----BLOCK 3------

    cout << "\nFloyd's algorithm (all shortest paths):\n";
    this_thread::sleep_for(chrono::seconds(1));
    floyd(graph_matr);
    this_thread::sleep_for(chrono::seconds(2));

    cout << "\nDijkstra's algorithm:";
    cout << "\n(start vert = 4)\n";
    this_thread::sleep_for(chrono::seconds(1));
    int start_vert_dijkstra_matr = 4;
    dijkstra(m_to_l, start_vert_dijkstra_matr);
    this_thread::sleep_for(chrono::seconds(2));

    cout << "\nModified Floyd's algorithm (shortest path between 2 vertices):";
    this_thread::sleep_for(chrono::seconds(1));
    cout << "\n(start vert = 1; end vert = 3)\n";
    this_thread::sleep_for(chrono::seconds(1));
    int start_vert_matr_floyd = 1;
    int end_vert_matr_floyd = 3;
    floyd_modified(graph_matr, start_vert_matr_floyd, end_vert_matr_floyd);
    this_thread::sleep_for(chrono::seconds(2));

    //-----BLOCK 4------

    cout << "\nTopological sorting (works only for directed acyclic graphs):";
    this_thread::sleep_for(chrono::seconds(1));
    topological_sort(m_to_l);
    this_thread::sleep_for(chrono::seconds(1));

    //-----BLOCK 5------

    cout << "\nFind a spanning tree:\n";
    if (!isConnected(m_to_l)) 
    {
        cout << "Graph isn`t connected.\n";
        this_thread::sleep_for(chrono::seconds(1));
    } else
        spanning_tree(m_to_l);
    this_thread::sleep_for(chrono::seconds(2));

    //-----BLOCK 6------

    cout << "\nKruskal's algorithm (min weight spanning tree):";
    this_thread::sleep_for(chrono::seconds(1));
    kruskal(m_to_l);
    this_thread::sleep_for(chrono::seconds(1));
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
