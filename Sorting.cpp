#include "Sorting.h"

void Sorting::insertionSortById(DoublyLinkedList<ClothingItem>& list) {
    Node<ClothingItem>* start = list.getHead();
    if (!start || !start->next) return;

    Node<ClothingItem>* current = start->next;
    while (current != nullptr) {
        ClothingItem key = current->data;
        Node<ClothingItem>* prevNode = current->prev;

        while (prevNode != nullptr && prevNode->data.id > key.id) {
            prevNode->next->data = prevNode->data;
            prevNode = prevNode->prev;
        }

        if (prevNode == nullptr) {
            start->data = key;
        } else {
            prevNode->next->data = key;
        }
        current = current->next;
    }
}