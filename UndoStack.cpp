#include "UndoStack.h"

UndoStack::~UndoStack() {
    while (!isEmpty()) {
        pop();
    }
}

void UndoStack::push(Action action) {
    StackNode* newNode = new StackNode(action);
    newNode->next = top;
    top = newNode;
}

Action UndoStack::pop() {
    if (isEmpty()) return Action{"NONE", -1};
    StackNode* temp = top;
    Action act = temp->action;
    top = top->next;
    delete temp;
    return act;
}