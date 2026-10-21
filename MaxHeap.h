#ifndef MAXHEAP_H
#define MAXHEAP_H

#include <cstddef>
#include <string>
#include <vector>

struct RiskEntry {
    std::string id;
    std::string location;
    std::string status;
    double score = 0.0;
};

// Max-heap: the entry with the highest risk score is at the root.
class MaxHeap {
private:
    std::vector<RiskEntry> heap;
    void siftUp(std::size_t index);
    void siftDown(std::size_t index);

public:
    void insert(const RiskEntry& entry);
    bool peekHighest(RiskEntry& entry) const;
    bool removeHighest(RiskEntry& entry);
    void clear();
    bool isEmpty() const;
    std::size_t size() const;
};

#endif
