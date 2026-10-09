#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H

#include <iostream>
using namespace std;

class CircularQueue
{
private:
    int queue[5];
    int front;
    int rear;
    int count;

public:
    CircularQueue();

    void enqueue(int sensorID);
    void dequeue();
    void display();
};

#endif