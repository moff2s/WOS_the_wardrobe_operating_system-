#ifndef CLOTHING_ITEM_H
#define CLOTHING_ITEM_H

#include <string>

struct ClothingItem {
    int id;
    std::string name;
    std::string category; // Top, Bottom, Shirt, Pants, etc.
    std::string brand;
    std::string imagePath; // Path to picture file

    ClothingItem(int id = 0, std::string name = "", std::string category = "", std::string brand = "", std::string imagePath = "")
        : id(id), name(name), category(category), brand(brand), imagePath(imagePath) {}
};

#endif
