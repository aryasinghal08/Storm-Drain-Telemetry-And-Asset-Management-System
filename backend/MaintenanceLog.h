#ifndef MAINTENANCELOG_H
#define MAINTENANCELOG_H

#include <cstddef>
#include <string>
#include <vector>

struct MaintenanceRecord {
    std::string id;
    std::string siteId;
    std::string location;
    std::string action;
    std::string note;
    std::string timestamp;
    std::string by;
};


struct LogNode {
    MaintenanceRecord record;
    LogNode* previous = nullptr;
    LogNode* next = nullptr;

    explicit LogNode(const MaintenanceRecord& value) : record(value) {}
};

// Keeps maintenance activity in insertion order and returns it newest-first.
class MaintenanceLog {
private:
    LogNode* head = nullptr;
    LogNode* tail = nullptr;
    std::size_t count = 0;

public:
    MaintenanceLog() = default;
    ~MaintenanceLog();

    MaintenanceLog(const MaintenanceLog&) = delete;
    MaintenanceLog& operator=(const MaintenanceLog&) = delete;

    void addRecord(const MaintenanceRecord& record);
    std::vector<MaintenanceRecord> newestFirst() const;
    std::size_t size() const;
};

#endif
