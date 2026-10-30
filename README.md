# WOS: The Wardrobe Operating System

A C++ console app that manages a wardrobe using custom data structures: clothing items, outfits, occasions, sorting, undo, and a queue that tracks what to wear next.

## Features

- **Clothing items:** store and manage individual pieces
- **Outfits:** combine items into outfits
- **Occasions:** tag outfits by occasion
- **Wear queue:** rotate what you wear so nothing gets forgotten
- **Sorting:** sort wardrobe items
- **Undo stack:** undo your last actions
- **Doubly linked list:** custom list implementation used for storage

## Project Structure

| File | Purpose |
|------|---------|
| `main.cpp` | Program entry point |
| `ClothingItem.h` | Clothing item class |
| `Outfit.h` | Outfit class |
| `Occasion.h` | Occasion definitions |
| `DoublyLinkedList.h` | Doubly linked list template |
| `WearQueue.h` / `WearQueue.cpp` | Queue for wear rotation |
| `Sorting.h` / `Sorting.cpp` | Sorting functions |
| `UndoStack.h` / `UndoStack.cpp` | Undo stack |
| `assets/` | Project assets |
| `CMakeLists.txt` | Build configuration |

## Build and Run

Requires CMake and a C++ compiler.

    mkdir build
    cd build
    cmake ..
    cmake --build .

Then run the generated executable from the `build` folder.

## Team

- Choeun Linda
- Oeurn Meylih
