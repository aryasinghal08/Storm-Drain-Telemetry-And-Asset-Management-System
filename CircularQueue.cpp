#include "CircularQueue.h"

CircularQueue::CircularQueue()
{
    front = 0;
    rear = -1;
    count = 0;
}

void CircularQueue::enqueue(int sensorID)
{
    if (count == 5)
    {
        cout << "Queue is full!\n";
        return;
    }

    rear = (rear + 1) % 5;

    queue[rear] = sensorID;

    count++;

    cout << "Sensor " << sensorID << " added to alert queue.\n";
}

void CircularQueue::dequeue()
{
    if (count == 0)
    {
        cout << "Queue is empty!\n";
        return;
    }

    cout << "Processing sensor: " << queue[front] << endl;

    front = (front + 1) % 5;

    count--;
}

void CircularQueue::display()
{
    if (count == 0)
    {
        cout << "Alert queue is empty.\n";
        return;
    }

    cout << "\n===== ALERT QUEUE =====\n";

    int index = front;

    for (int i = 0; i < count; i++)
    {
        cout << queue[index] << " ";

        index = (index + 1) % 5;
    }

    cout << endl;
}