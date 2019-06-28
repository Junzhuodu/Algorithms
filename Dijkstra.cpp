#include <iostream>
#include <string>
#include <queue>
#include <vector>

using namespace std;

// iPair ==> Integer Pair
typedef pair<int, int> iPair;

// Graph class
class Graph {
public:
    const static int V = 9;
    // adjacent matrix for graph
    vector<vector<int>> adj_matrix = {
        {0, 4, 0, 0, 0, 0, 0, 8, 0}, 
        {4, 0, 8, 0, 0, 0, 0, 11, 0}, 
        {0, 8, 0, 7, 0, 4, 0, 0, 2}, 
        {0, 0, 7, 0, 9, 14, 0, 0, 0}, 
        {0, 0, 0, 9, 0, 10, 0, 0, 0}, 
        {0, 0, 4, 14, 10, 0, 2, 0, 0}, 
        {0, 0, 0, 0, 0, 2, 0, 1, 6}, 
        {8, 11, 0, 0, 0, 0, 1, 0, 7}, 
        {0, 0, 2, 0, 0, 0, 6, 7, 0}        
    };
};

class Planner : Graph {
public:
    int start, target;
    Planner(int s, int t) {
        start = s;
        target = t;
    }
};

// Template function for print 2DVector of any type
template<typename T>
void print2DVector(T& Vec) {
    if (Vec.size() == 0 || Vec[0].size() == 0) {
        return;
    }
    for (int i = 0; i < Vec.size(); i++) {
        for (int j = 0; j < Vec[0].size(); j++) {
            std::cout << Vec[i][j] << ' ';
        }
        cout << endl;
    }
}

void search(Graph &graph, Planner& planner) {
    // close list
    vector<bool> visited(graph.V, false);
    // open list
    priority_queue<iPair, vector<iPair>, greater<iPair>> pq;
    
    // distance list (distance from start vertix to other vertix)
    vector<int> dist(graph.V, INT_MAX);
    dist[planner.start] = 0;
    pq.push(make_pair(0, planner.start));
    
    bool found = false;
    vector<int> action(graph.V, -1); // save pre-vertix for printing the path

    while (!found && !pq.empty()) {
        // choosen the smallest weight as current vertix from open list
        int u = pq.top().second;
        pq.pop();
        visited[u] = true;
        if (u == planner.target) {
            found = true;
        }
        else {
            for (int v = 0; v < graph.V; v++) {
                int weight = graph.adj_matrix[u][v];
                // if there is a shorter path to v through u
                if (!visited[v] && weight != 0 &&
                    dist[v] > dist[u] + weight) {
                    // updating distance of v
                    dist[v] = dist[u] + weight;
                    pq.push(make_pair(dist[v], v));
                    action[v] = u;
                }
            }            
        }
    } // found the target
    
    // print adjacent matrix
    cout << "The adjacent matrix for the graph" << endl;
    print2DVector(graph.adj_matrix);
    
    // Find the path from start to target
    vector<iPair> path;
    int v = planner.target;
    path.push_back(iPair(v, dist[v]));
    v = action[v];
    while (v != planner.start) {
        path.push_back(iPair(v, dist[v]));
        v = action[v];
    }
    path.push_back(iPair(planner.start, 0));
    // print the path
    cout << "The path from start vertix " << planner.start << 
        " to target vertix "<< planner.target << endl;
    for (int i = path.size() - 1; i >= 0; i--) {
        cout << "vertix: " << path[i].first << " " 
            << "path cost: " << path[i].second << endl;
    }
    
}


int main() {
    Graph graph;
    Planner planner(0, 4);
    search(graph, planner);
}