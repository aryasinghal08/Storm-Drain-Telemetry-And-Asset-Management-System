#include "Graph.h"

#include <algorithm>
#include <queue>

Graph::Graph(int count)
    : names(count > 0 ? static_cast<std::size_t>(count) : 0),
      adjacency(count > 0 ? static_cast<std::size_t>(count) : 0) {
}

bool Graph::setLocation(int index, const std::string& name) {
    if (index < 0 || index >= getVertexCount() || name.empty()) {
        return false;
    }

    names[static_cast<std::size_t>(index)] = name;
    return true;
}

// Add a one-way connection between two drainage zones.
bool Graph::addEdge(int from, int to) {
    if (from < 0 || to < 0 || from >= getVertexCount() ||
        to >= getVertexCount() || from == to) {
        return false;
    }

    std::vector<int>& neighbours = adjacency[static_cast<std::size_t>(from)];
    if (std::find(neighbours.begin(), neighbours.end(), to) == neighbours.end()) {
        neighbours.push_back(to);
    }
    return true;
}

// Breadth-First Search visits reachable vertices level by level.
std::vector<int> Graph::BFS(int start) const {
    std::vector<int> traversalOrder;
    if (start < 0 || start >= getVertexCount()) {
        return traversalOrder;
    }

    std::vector<bool> visited(names.size(), false);
    std::queue<int> pending;

    pending.push(start);
    visited[static_cast<std::size_t>(start)] = true;

    while (!pending.empty()) {
        const int current = pending.front();
        pending.pop();
        traversalOrder.push_back(current);

        for (int neighbour : adjacency[static_cast<std::size_t>(current)]) {
            if (!visited[static_cast<std::size_t>(neighbour)]) {
                visited[static_cast<std::size_t>(neighbour)] = true;
                pending.push(neighbour);
            }
        }
    }

    return traversalOrder;
}

bool Graph::canReach(int start, int target) const {
    const std::vector<int> visitedOrder = BFS(start);
    return std::find(visitedOrder.begin(), visitedOrder.end(), target) != visitedOrder.end();
}

int Graph::getVertexCount() const {
    return static_cast<int>(names.size());
}

std::string Graph::getLocation(int index) const {
    if (index < 0 || index >= getVertexCount()) {
        return "Unknown";
    }
    return names[static_cast<std::size_t>(index)];
}
