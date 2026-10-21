#include "HashTable.h"

#include <functional>

HashTable::HashTable(std::size_t capacity)
    : table(capacity == 0 ? 101 : capacity) {
}

std::size_t HashTable::hash(const std::string& key) const {
    return std::hash<std::string>{}(key) % table.size();
}

// Insert a new site or replace its previous reading. Linear probing handles collisions.
bool HashTable::insertOrUpdate(const SiteRecord& record) {
    if (record.id.empty() || table.empty()) {
        return false;
    }

    const std::size_t startIndex = hash(record.id);

    for (std::size_t step = 0; step < table.size(); ++step) {
        const std::size_t index = (startIndex + step) % table.size();
        SiteRecord& slot = table[index];

        if (!slot.valid || slot.id == record.id) {
            if (!slot.valid) {
                ++count;
            }
            slot = record;
            slot.valid = true;
            return true;
        }
    }

    return false; // Every slot is occupied by another key.
}

const SiteRecord* HashTable::find(const std::string& id) const {
    if (table.empty() || id.empty()) {
        return nullptr;
    }

    const std::size_t startIndex = hash(id);
    for (std::size_t step = 0; step < table.size(); ++step) {
        const std::size_t index = (startIndex + step) % table.size();
        const SiteRecord& slot = table[index];

        // An unused slot means the key was not inserted further along this probe sequence.
        if (!slot.valid) {
            return nullptr;
        }
        if (slot.id == id) {
            return &slot;
        }
    }

    return nullptr;
}

std::vector<SiteRecord> HashTable::getAll() const {
    std::vector<SiteRecord> records;
    records.reserve(count);

    for (const SiteRecord& record : table) {
        if (record.valid) {
            records.push_back(record);
        }
    }
    return records;
}

std::size_t HashTable::size() const {
    return count;
}
