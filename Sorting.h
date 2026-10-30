#ifndef SORTING_H
#define SORTING_H

#include "DoublyLinkedList.h"
#include "ClothingItem.h"

class Sorting {
public:
    static void insertionSortById(DoublyLinkedList<ClothingItem>& list);
};

#endif