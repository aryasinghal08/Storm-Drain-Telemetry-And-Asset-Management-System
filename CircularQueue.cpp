
#include "CircularQueue.h"
#include <algorithm>
#include <iostream>
using namespace std;

// Constructor
CircularQueue::CircularQueue(size_t capacity)
{
    if (capacity == 0)
    {
        capacity = 1;
    }

    data.resize(capacity);

    frontIndex = 0;
    rearIndex = 0;
    count = 0;
}

// Insert an element into the circular queue
bool CircularQueue::enqueue(const string& value)
{
    // Check whether the queue is full
    if (count == data.size())
    {
        return false;
    }

    // Avoid inserting a duplicate value
    if (contains(value))
    {
        return false;
    }

    // Insert at the rear position
    data[rearIndex] = value;

    // Move rear circularly to the next position
    rearIndex = (rearIndex + 1) % data.size();

    count++;

    return true;
}

// Remove an element from the front
bool CircularQueue::dequeue(string& value)
{
    // Check whether the queue is empty
    if (count == 0)
    {
        return false;
    }

    // Store the front element
    value = data[frontIndex];

    // Move front circularly to the next position
    frontIndex = (frontIndex + 1) % data.size();

    count--;

    return true;
}

// Check whether a value already exists in the queue
bool CircularQueue::contains(const string& value) const
{
    for (size_t i = 0; i < count; i++)
    {
        size_t index = (frontIndex + i) % data.size();

        if (data[index] == value)
        {
            return true;
        }
    }

    return false;
}

// Check whether the queue is empty
bool CircularQueue::isEmpty() const
{
    return count == 0;
}

// Return the current number of elements
size_t CircularQueue::size() const
{
    return count;
}

// Return all elements in queue order
vector<string> CircularQueue::getAll() const
{
    vector<string> result;

    for (size_t i = 0; i < count; i++)
    {
        size_t index = (frontIndex + i) % data.size();

        result.push_back(data[index]);
    }

    return result;
}
