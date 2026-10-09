#include <iostream>
#include <string>
#include "HashTable.h"
#include "CircularQueue.h"
#include "MaintenanceLog.h"

using namespace std;

int main() {
    HashTable ht;
    CircularQueue cq;
    MaintenanceLog logSystem;

    int choice;
    do {
        cout << "\n=========================================\n";
        cout << " DEHRADUN MUNICIPAL TELEMETRY ENGINE\n";
        cout << "=========================================\n";
        cout << "1. Insert Sensor Data (Hash Table & Queue)\n";
        cout << "2. Search Sensor & View Alert Queue\n";
        cout << "3. Add Maintenance Log\n";
        cout << "4. View Maintenance Logs\n";
        cout << "5. Display All Active Sensors\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                int id;
                float water, soil;
                cout << "Enter Sensor ID: ";
                cin >> id;
                cout << "Enter Water Level: ";
                cin >> water;
                cout << "Enter Soil Moisture: ";
                cin >> soil;
                
                ht.insertSensor(id, water, soil);
                cq.enqueue(id);
                break;
            }
            case 2: {
                int id;
                cout << "Enter Sensor ID to search: ";
                cin >> id;
                
                ht.searchSensor(id);
                cq.display();
                break;
            }
            case 3: {
                int id;
                string message;
                cout << "Enter Sensor ID: ";
                cin >> id;
                cout << "Enter Maintenance Message: ";
                cin.ignore();
                getline(cin, message);

                logSystem.addLog(id, message);
                break;
            }
            case 4: {
                logSystem.displayLogs();
                break;
            }
            case 5: {
                ht.displaySensors();
                break;
            }
            case 6:
                cout << "Exiting system. Stay safe!\n";
                break;
            default:
                cout << "Invalid choice! Please try again.\n";
        }
    } while (choice != 6);

    return 0;
}
