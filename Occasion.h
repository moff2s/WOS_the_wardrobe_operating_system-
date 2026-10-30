#ifndef OCCASION_H
#define OCCASION_H

#include <string>
#include "DoublyLinkedList.h"
#include "ClothingItem.h"

struct Occasion {
    int id;
    std::string name;
    DoublyLinkedList<ClothingItem> tops;
    DoublyLinkedList<ClothingItem> bottoms;

    Occasion(int id = 0, std::string name = "")
        : id(id), name(name) {}
};

#endif
