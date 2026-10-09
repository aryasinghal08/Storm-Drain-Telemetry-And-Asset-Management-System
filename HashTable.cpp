#include "HashTable.h"

int HashTable::hashFunction(int id)
{
    return id % 10;
}

void HashTable::insertSensor(int id, float water, float soil)
{
    int index = hashFunction(id);

    int start = index;

    while (table[index].occupied)
    {
        index = (index + 1) % 10;

        if (index == start)
        {
            cout << "Hash table is full!\n";
            return;
        }
    }

    table[index].id = id;
    table[index].waterLevel = water;
    table[index].soilMoisture = soil;
    table[index].occupied = true;

    cout << "Sensor data added successfully.\n";
}

void HashTable::searchSensor(int id)
{
    int index = hashFunction(id);
    int start = index;

    while (table[index].occupied)
    {
        if (table[index].id == id)
        {
            cout << "\nSensor Found!\n";
            cout << "Sensor ID: " << table[index].id << endl;
            cout << "Water Level: " << table[index].waterLevel << endl;
            cout << "Soil Moisture: " << table[index].soilMoisture << endl;

            return;
        }

        index = (index + 1) % 10;

        if (index == start)
            break;
    }

    cout << "Sensor not found.\n";
}

void HashTable::displaySensors()
{
    cout << "\n===== SENSOR DATA =====\n";

    bool found = false;

    for (int i = 0; i < 10; i++)
    {
        if (table[i].occupied)
        {
            found = true;

            cout << "Sensor ID: " << table[i].id
                 << " | Water: " << table[i].waterLevel
                 << " | Soil: " << table[i].soilMoisture
                 << endl;
        }
    }

    if (!found)
    {
        cout << "No sensor data available.\n";
    }
}