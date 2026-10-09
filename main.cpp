#include <iostream>

#include "Graph.h"
#include "HashTable.h"
#include "CircularQueue.h"
#include "MaxHeap.h"
#include "MaintenanceLog.h"

using namespace std;

int main()
{
    // Create all data structures

    Graph drainageGraph(4);

    HashTable sensorTable;

    CircularQueue alertQueue;

    MaxHeap riskHeap;

    MaintenanceLog maintenanceLog;


    // Set drainage locations

    drainageGraph.setLocation(0, "ISBT");
    drainageGraph.setLocation(1, "Prince Chowk");
    drainageGraph.setLocation(2, "Ballupur Chowk");
    drainageGraph.setLocation(3, "Sahastradhara Road");


    // Create drainage connections

    drainageGraph.addEdge(0, 1);
    drainageGraph.addEdge(1, 2);
    drainageGraph.addEdge(2, 3);


    int choice;

    do
    {
        cout << "\n====================================\n";
        cout << " STORM DRAIN TELEMETRY SYSTEM\n";
        cout << "====================================\n";

        cout << "1. Display Drainage Network\n";
        cout << "2. Add Sensor Data\n";
        cout << "3. Search Sensor\n";
        cout << "4. Display All Sensors\n";
        cout << "5. Add Alert to Queue\n";
        cout << "6. Process Alert Queue\n";
        cout << "7. Display Alert Queue\n";
        cout << "8. Add Risk\n";
        cout << "9. Display Highest Risk\n";
        cout << "10. Display Risk Heap\n";
        cout << "11. Add Maintenance Log\n";
        cout << "12. Display Maintenance Logs\n";
        cout << "0. Exit\n";

        cout << "\nEnter your choice: ";
        cin >> choice;


        switch (choice)
        {
            case 1:
            {
                drainageGraph.displayGraph();
                break;
            }


            case 2:
            {
                int id;
                float water;
                float soil;

                cout << "Enter Sensor ID: ";
                cin >> id;

                cout << "Enter Water Level: ";
                cin >> water;

                cout << "Enter Soil Moisture: ";
                cin >> soil;

                sensorTable.insertSensor(id, water, soil);

                break;
            }


            case 3:
            {
                int id;

                cout << "Enter Sensor ID to search: ";
                cin >> id;

                sensorTable.searchSensor(id);

                break;
            }


            case 4:
            {
                sensorTable.displaySensors();

                break;
            }


            case 5:
            {
                int id;

                cout << "Enter Sensor ID: ";
                cin >> id;

                alertQueue.enqueue(id);

                break;
            }


            case 6:
            {
                alertQueue.dequeue();

                break;
            }


            case 7:
            {
                alertQueue.display();

                break;
            }


            case 8:
            {
                int id;
                int risk;

                cout << "Enter Sensor ID: ";
                cin >> id;

                cout << "Enter Risk Score (1-100): ";
                cin >> risk;

                riskHeap.insertRisk(id, risk);

                break;
            }


            case 9:
            {
                riskHeap.displayHighestRisk();

                break;
            }


            case 10:
            {
                riskHeap.displayHeap();

                break;
            }


            case 11:
            {
                int id;
                string message;

                cout << "Enter Sensor ID: ";
                cin >> id;

                cout << "Enter maintenance message: ";
                cin.ignore();
                getline(cin, message);

                maintenanceLog.addLog(id, message);

                break;
            }


            case 12:
            {
                maintenanceLog.displayLogs();

                break;
            }


            case 0:
            {
                cout << "\nExiting program...\n";
                break;
            }


            default:
            {
                cout << "Invalid choice!\n";
            }
        }

    } while (choice != 0);


    return 0;
}