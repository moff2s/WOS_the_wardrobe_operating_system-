#ifndef OUTFIT_H
#define OUTFIT_H

#include <string>
#include "DoublyLinkedList.h"
#include "ClothingItem.h"

struct Outfit {
    int id;
    int occasionId; // Tracks which occasion this outfit belongs to
    std::string name;
    // Pointers to ClothingItems to save memory
    DoublyLinkedList<ClothingItem*> items;

    Outfit(int id = 0, int occasionId = 0, std::string name = "")
        : id(id), occasionId(occasionId), name(name) {}
};

#endif
