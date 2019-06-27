#include <iostream>
#include <string.h>
#include <vector>
#include <queue>

using namespace std;

// Map class
class Map {
public:
    const static int Height = 5;
    const static int Width = 6;
    vector<vector<int>> grid = {
        {0, 1, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0},
        {0, 0, 0, 1, 1, 0}
    };
    
    vector<vector<int>> heuristic = {
        {9, 8, 7, 6, 5, 4},
        {8, 7, 6, 5, 4, 3},
        {7, 6, 5, 4, 3, 2},
        {6, 5, 4, 3, 2, 1}, 
        {5, 4, 3, 2, 1, 0}
    };
};

// Planner class
class Planner : Map {
public:
    int start[2] = {0, 0};
    int goal[2] = {Height - 1, Width - 1};
    int cost = 1;
    
    vector<vector<int>> movements = {
        {1, 0},
        {-1, 0},
        {0, 1},
        {0, -1}
    };
    string movements_arrows[4] = {"v", "^", ">", "<"};
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

// Bfs search algorithm
void search(Map &map, Planner &planner) {
    // Create a visited array to check if the point is visited (Closed List)
    vector<vector<bool>> visited(map.Height, vector<bool>(map.Width, false));
    
    // Create a expand array to store the counts for expansion
    vector<vector<int>> expand(map.Height, vector<int>(map.Width, -1));
    
    // Create a action array to store the index of actions in order to get the path for backward
    vector<vector<int>> action(map.Height, vector<int>(map.Width, -1));

    typedef pair<int, int> coord;
    typedef pair<int, int> distance;
    typedef pair<distance, coord> point;
    auto cmp = [](point& a, point& b) {
        return a.first.first > b.first.first;
    };
    
    // Open list
    priority_queue<point, vector<point>, decltype(cmp)> minheap(cmp);
    
    int x = planner.start[0];
    int y = planner.start[1];
    int g = 0;
    int f = g + map.heuristic[x][y];
    minheap.push(point(distance(f, g), coord(x, y)));
    visited[x][y] = true;
    
    bool found = false;
    int count = 0;
    int x2, y2, g2, f2;
    
    while (!found && !minheap.empty()) {
        auto node = minheap.top(); minheap.pop();
        x = node.second.first;
        y = node.second.second;
        f = node.first.first;
        g = node.first.second;
        
        expand[x][y] = count;
        count++;
        
        if (x == planner.goal[0] && y == planner.goal[1]) {
            found = true;
        }
        else {
            for (int i = 0; i < planner.movements.size(); i++) {
                x2 = x + planner.movements[i][0];
                y2 = y + planner.movements[i][1];
                if (!(0 <= x2 && x2 < map.Height && 0 <= y2 && y2 < map.Width)) {
                    continue;
                }
                if (visited[x2][y2] == false && map.grid[x2][y2] == 0) {
                    g2 = g + planner.cost;
                    f2 = g2 + map.heuristic[x2][y2];
                    minheap.push(point(distance(f2, g2), coord(x2, y2)));
                    visited[x2][y2] = true;
                    action[x2][y2] = i;
                }
            }
        }
    }  // Found the goal
    
    // Print the 2D map
    cout << "Our map:" << endl;
    print2DVector(map.grid);
    // Print the 2D expansion
    cout << "The expansion for A star:" << endl;
    print2DVector(expand);
    
    // Find the path with robot orientation
    vector<vector<string>> policy(map.Height, vector<string>(map.Width, "-"));
    
    // Going backward
    x = planner.goal[0];
    y = planner.goal[1];
    policy[x][y] = "*";
    
    while (x != planner.start[0] || y != planner.start[1]) {
        x2 = x - planner.movements[action[x][y]][0];
        y2 = y - planner.movements[action[x][y]][1];
        policy[x2][y2] = planner.movements_arrows[action[x][y]];
        x = x2;
        y = y2;
    }
    // Print the path with arrows
    cout << "The shortest path:" << endl;
    print2DVector(policy);
}

int main() {
    Map map;
    Planner planner;
    search(map, planner);
    return 0;
}
