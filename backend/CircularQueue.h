#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H

#include <cstddef>
#include <string>
#include <vector>

// Fixed-capacity circular queue used to track alert IDs awaiting review.
class CircularQueue {
private:
    std::vector<std::string> data;
    std::size_t frontIndex = 0;
    std::size_t rearIndex = 0;
    std::size_t count = 0;

public:
    explicit CircularQueue(std::size_t capacity = 100);

    bool enqueue(const std::string& value);
    bool dequeue(std::string& value);
    bool contains(const std::string& value) const;
    bool isEmpty() const;
    std::size_t size() const;
    std::vector<std::string> getAll() const;
};

#endif
