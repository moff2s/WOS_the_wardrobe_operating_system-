#include "WearQueue.h"

WearQueue::~WearQueue() {
    while (front) {
        dequeue();
    }
}

void WearQueue::enqueue(const std::string& log) {
    QueueNode* newNode = new QueueNode(log);
    if (!rear) {
        front = rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }
}

void WearQueue::dequeue() {
    if (!front) return;
    QueueNode* temp = front;
    front = front->next;
    if (!front) rear = nullptr;
    delete temp;
}

void WearQueue::displayLog() const {
    QueueNode* temp = front;
    std::cout << "\n--- Wear Log (FIFO) ---" << std::endl;
    while (temp) {
        std::cout << "- " << temp->logEntry << std::endl;
        temp = temp->next;
    }
}

std::vector<std::string> WearQueue::getAllLogs() const {
    std::vector<std::string> logs;
    QueueNode* temp = front;
    while (temp) {
        logs.push_back(temp->logEntry);
        temp = temp->next;
    }
    return logs;
}
