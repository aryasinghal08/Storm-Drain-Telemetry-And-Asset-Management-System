#ifndef MAINTENANCELOG_H
#define MAINTENANCELOG_H

#include <iostream>
using namespace std;

struct LogNode
{
    int sensorID;
    string message;

    LogNode* next;
    LogNode* previous;
};

class MaintenanceLog
{
private:
    LogNode* head;
    LogNode* tail;

public:
    MaintenanceLog();

    void addLog(int sensorID, string message);
    void displayLogs();
};

#endif