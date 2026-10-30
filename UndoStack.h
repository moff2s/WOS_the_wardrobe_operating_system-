#ifndef UNDO_STACK_H
#define UNDO_STACK_H

#include <string>

struct Action {
    std::string type; // "ADD_OUTFIT", "DELETE_OUTFIT", etc.
    int outfitId;
};

struct StackNode {
    Action action;
    StackNode* next;
    StackNode(Action act) : action(act), next(nullptr) {}
};

class UndoStack {
private:
    StackNode* top;

public:
    UndoStack() : top(nullptr) {}
    ~UndoStack();

    void push(Action action);
    Action pop();
    bool isEmpty() const { return top == nullptr; }
};

#endif