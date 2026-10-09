#include "MaintenanceLog.h"

MaintenanceLog::MaintenanceLog()
{
    head = NULL;
    tail = NULL;
}

void MaintenanceLog::addLog(int sensorID, string message)
{
    LogNode* newNode = new LogNode;

    newNode->sensorID = sensorID;
    newNode->message = message;

    newNode->next = NULL;
    newNode->previous = NULL;

    if (head == NULL)
    {
        head = newNode;
        tail = newNode;
    }
    else
    {
        tail->next = newNode;
        newNode->previous = tail;

        tail = newNode;
    }

    cout << "Maintenance log added successfully.\n";
}

void MaintenanceLog::displayLogs()
{
    if (head == NULL)
    {
        cout << "No maintenance logs available.\n";
        return;
    }

    cout << "\n===== MAINTENANCE LOGS =====\n";

    LogNode* current = head;

    while (current != NULL)
    {
        cout << "Sensor ID: " << current->sensorID << endl;
        cout << "Message: " << current->message << endl;
        cout << "--------------------------\n";

        current = current->next;
    }
}