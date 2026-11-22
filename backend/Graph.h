#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>

class Graph {
private:
    std::vector<std::string> names;
    std::vector<std::vector<int>> adjacency;

public:
    explicit Graph(int count = 0);

    bool setLocation(int index, const std::string& name);
    bool addEdge(int from, int to);
    std::vector<int> BFS(int start) const;
    bool canReach(int start, int target) const;
    int getVertexCount() const;
    std::string getLocation(int index) const;
};

#endif
