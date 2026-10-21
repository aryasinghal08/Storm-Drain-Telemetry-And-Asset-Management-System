#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <cstddef>
#include <string>
#include <vector>

// Latest weather-informed record stored for one configured site.
struct SiteRecord {
    std::string id;
    std::string location;
    std::string observedAt;
    std::string status;
    double latitude = 0.0;
    double longitude = 0.0;
    double precipitation = 0.0;
    double rain = 0.0;
    double humidity = 0.0;
    double wind = 0.0;
    double risk = 0.0;
    bool valid = false;
    bool stale = true;
};

// Hash table with linear probing for location records.
class HashTable {
private:
    std::vector<SiteRecord> table;
    std::size_t count = 0;
    std::size_t hash(const std::string& key) const;

public:
    explicit HashTable(std::size_t capacity = 101);

    bool insertOrUpdate(const SiteRecord& record);
    const SiteRecord* find(const std::string& id) const;
    std::vector<SiteRecord> getAll() const;
    std::size_t size() const;
};

#endif
