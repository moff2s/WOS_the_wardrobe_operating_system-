#ifndef WEAR_QUEUE_H
#define WEAR_QUEUE_H

#include <string>
#include <vector>
#include <iostream>

struct QueueNode {
    std::string logEntry;
    QueueNode* next;
    QueueNode(std::string log) : logEntry(log), next(nullptr) {}
};

class WearQueue {
private:
    QueueNode* front;
    QueueNode* rear;

public:
    WearQueue() : front(nullptr), rear(nullptr) {}
    ~WearQueue();

    void enqueue(const std::string& log);
    void dequeue();
    void displayLog() const;

    // Non-destructively reads all logs for GUI screens
    std::vector<std::string> getAllLogs() const;
    bool isEmpty() const { return front == nullptr; }
};

#endif
