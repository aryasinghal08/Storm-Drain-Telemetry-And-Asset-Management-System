#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <iostream>
using namespace std;

struct Sensor
{
    int id;
    float waterLevel;
    float soilMoisture;
    bool occupied;

    Sensor()
    {
        id = -1;
        waterLevel = 0;
        soilMoisture = 0;
        occupied = false;
    }
};

class HashTable
{
private:
    Sensor table[10];

    int hashFunction(int id);

public:
    void insertSensor(int id, float water, float soil);
    void searchSensor(int id);
    void displaySensors();
};

#endif