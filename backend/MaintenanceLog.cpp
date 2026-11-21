#include "MaintenanceLog.h"

MaintenanceLog::~MaintenanceLog() {
    LogNode* current = head;
    while (current != nullptr) {
        LogNode* nextNode = current->next;
        delete current;
        current = nextNode;
    }
}

// Add a new node at the tail of the doubly linked list.
void MaintenanceLog::addRecord(const MaintenanceRecord& record) {
    LogNode* newNode = new LogNode(record);

    if (tail == nullptr) {
        // The list is empty: the new node is both head and tail.
        head = newNode;
        tail = newNode;
    } else {
        newNode->previous = tail;
        tail->next = newNode;
        tail = newNode;
    }

    ++count;
}

// Walk backward from tail so the most recent activity appears first.
std::vector<MaintenanceRecord> MaintenanceLog::newestFirst() const {
    std::vector<MaintenanceRecord> records;
    records.reserve(count);

    for (LogNode* current = tail; current != nullptr; current = current->previous) {
        records.push_back(current->record);
    }
    return records;
}

std::size_t MaintenanceLog::size() const {
    return count;
}
