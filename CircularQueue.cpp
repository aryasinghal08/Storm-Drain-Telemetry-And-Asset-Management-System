#include "CircularQueue.h"

CircularQueue::CircularQueue(std::size_t capacity)
    : data(capacity == 0 ? 1 : capacity) {
}

// Add a new value at the rear of the queue.
bool CircularQueue::enqueue(const std::string& value) {
    if (count == data.size()) {
        return false; // Queue is full.
    }

    if (contains(value)) {
        return false; // Do not add duplicate alert IDs.
    }

    data[rearIndex] = value;
    rearIndex = (rearIndex + 1) % data.size();
    ++count;
    return true;
}

// Remove the oldest value from the front of the queue.
bool CircularQueue::dequeue(std::string& value) {
    if (isEmpty()) {
        return false;
    }

    value = data[frontIndex];
    frontIndex = (frontIndex + 1) % data.size();
    --count;
    return true;
}

// Search only the active queue entries, in FIFO order.
bool CircularQueue::contains(const std::string& value) const {
    for (std::size_t i = 0; i < count; ++i) {
        const std::size_t index = (frontIndex + i) % data.size();
        if (data[index] == value) {
            return true;
        }
    }
    return false;
}

bool CircularQueue::isEmpty() const {
    return count == 0;
}

std::size_t CircularQueue::size() const {
    return count;
}

// Return a copy of the active values in their queue order.
std::vector<std::string> CircularQueue::getAll() const {
    std::vector<std::string> result;
    result.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        const std::size_t index = (frontIndex + i) % data.size();
        result.push_back(data[index]);
    }
    return result;
}
