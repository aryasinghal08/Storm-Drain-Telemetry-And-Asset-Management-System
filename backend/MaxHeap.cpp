#include "MaxHeap.h"

#include <utility>

// Restore heap order after adding an entry at the end.
void MaxHeap::siftUp(std::size_t index) {
    while (index > 0) {
        const std::size_t parent = (index - 1) / 2;
        if (heap[parent].score >= heap[index].score) {
            break;
        }

        std::swap(heap[parent], heap[index]);
        index = parent;
    }
}

// Restore heap order after replacing the root with the last entry.
void MaxHeap::siftDown(std::size_t index) {
    while (true) {
        const std::size_t leftChild = 2 * index + 1;
        const std::size_t rightChild = 2 * index + 2;
        std::size_t largest = index;

        if (leftChild < heap.size() && heap[leftChild].score > heap[largest].score) {
            largest = leftChild;
        }
        if (rightChild < heap.size() && heap[rightChild].score > heap[largest].score) {
            largest = rightChild;
        }
        if (largest == index) {
            break;
        }

        std::swap(heap[index], heap[largest]);
        index = largest;
    }
}

void MaxHeap::insert(const RiskEntry& entry) {
    heap.push_back(entry);
    siftUp(heap.size() - 1);
}

bool MaxHeap::peekHighest(RiskEntry& entry) const {
    if (heap.empty()) {
        return false;
    }
    entry = heap.front();
    return true;
}

bool MaxHeap::removeHighest(RiskEntry& entry) {
    if (heap.empty()) {
        return false;
    }

    entry = heap.front();
    if (heap.size() == 1) {
        heap.pop_back();
        return true;
    }

    heap.front() = heap.back();
    heap.pop_back();
    siftDown(0);
    return true;
}

void MaxHeap::clear() {
    heap.clear();
}

bool MaxHeap::isEmpty() const {
    return heap.empty();
}

std::size_t MaxHeap::size() const {
    return heap.size();
}
