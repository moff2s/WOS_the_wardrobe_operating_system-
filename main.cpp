// ============================================================
// Wardrobe Operating System — Raylib GUI with DLL Navigation
// ============================================================
#include "raylib.h"
#include "ClothingItem.h"
#include "DoublyLinkedList.h"
#include "Occasion.h"
#include "Outfit.h"
#include "WearQueue.h"
#include "UndoStack.h"
#include "Sorting.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>

// Global Texture Cache
std::unordered_map<std::string, Texture2D> textureCache;

Texture2D GetOrLoadTexture(const std::string& path) {
    if (path.empty()) return Texture2D{0};
    auto it = textureCache.find(path);
    if (it != textureCache.end()) {
        return it->second;
    }
    Texture2D tex = LoadTexture(path.c_str());
    if (tex.id != 0) {
        SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);
    }
    textureCache[path] = tex;
    return tex;
}

void UnloadAllTextures() {
    for (auto& pair : textureCache) {
        if (pair.second.id != 0) {
            UnloadTexture(pair.second);
        }
    }
    textureCache.clear();
}

// Draw texture fitted preserving aspect ratio
void DrawTextureFitted(Texture2D texture, Rectangle dest) {
    if (texture.id == 0) {
        DrawRectangleRec(dest, GetColor(0xEAEDEDFF));
        DrawText("No Image", (int)dest.x + (int)(dest.width - MeasureText("No Image", 14)) / 2, (int)dest.y + (int)dest.height / 2 - 8, 14, DARKGRAY);
        return;
    }

    float scale = fminf(dest.width / (float)texture.width, dest.height / (float)texture.height);
    float drawW = (float)texture.width * scale;
    float drawH = (float)texture.height * scale;
    float drawX = dest.x + (dest.width - drawW) / 2.0f;
    float drawY = dest.y + (dest.height - drawH) / 2.0f;

    Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
    Rectangle target = { drawX, drawY, drawW, drawH };
    DrawTexturePro(texture, source, target, { 0.0f, 0.0f }, 0.0f, WHITE);
}

// Reusable standard button
bool DrawButton(Rectangle rect, const char* label, bool highlighted = false, Color customBg = BLANK) {
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, rect);
    Color fill = highlighted ? DARKGREEN : (hovered ? LIGHTGRAY : GRAY);
    if (customBg.a > 0) {
        fill = hovered ? ColorAlpha(customBg, 0.8f) : customBg;
    }
    Color textColor = (highlighted || customBg.a > 0) ? RAYWHITE : BLACK;

    DrawRectangleRec(rect, fill);
    DrawRectangleLinesEx(rect, 2, BLACK);

    int fontSize = 16;
    int textWidth = MeasureText(label, fontSize);
    DrawText(label, (int)(rect.x + (rect.width - textWidth) / 2), (int)(rect.y + (rect.height - fontSize) / 2), fontSize, textColor);

    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

// Draw a single clothing card in the vertical outfit stack
void DrawVerticalClothingCard(Rectangle rect, const ClothingItem& item) {
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, rect);

    Color bg = hovered ? GetColor(0xFBFCFCFF) : WHITE;
    DrawRectangleRounded(rect, 0.06f, 4, bg);

    Color borderColor = hovered ? GetColor(0x3498DBFF) : GetColor(0xBDC3C7FF);
    DrawRectangleRoundedLinesEx(rect, 0.06f, 4, hovered ? 2.0f : 1.5f, borderColor);

    // Image frame
    Rectangle imgBox = { rect.x + 8.0f, rect.y + 8.0f, rect.width - 16.0f, 115.0f };
    Texture2D tex = GetOrLoadTexture(item.imagePath);
    DrawTextureFitted(tex, imgBox);

    // Clothing name
    int nameFontSize = 13;
    int nameWidth = MeasureText(item.name.c_str(), nameFontSize);
    int nameX = (int)(rect.x + (rect.width - nameWidth) / 2);
    DrawText(item.name.c_str(), nameX, (int)(rect.y + 130), nameFontSize, BLACK);

    // Category and style / brand info
    std::string styleInfo = item.category + "  *  " + item.brand;
    int styleFontSize = 12;
    int styleWidth = MeasureText(styleInfo.c_str(), styleFontSize);
    int styleX = (int)(rect.x + (rect.width - styleWidth) / 2);
    DrawText(styleInfo.c_str(), styleX, (int)(rect.y + 150), styleFontSize, DARKGRAY);
}

// App states
enum class Screen { OCCASION, OUTFIT, HISTORY };

// Populate sample occasions using DoublyLinkedList<ClothingItem>
void initOccasions(std::vector<Occasion>& occasions) {
    Occasion casual(1, "Casual");

    // 5 Casual Tops with uploaded photos
    casual.tops.append(ClothingItem(1, "Gingham Layered Knit", "Top", "Stylenanda", "assets/casual_top_1.jpg"));
    casual.tops.append(ClothingItem(2, "One-Shoulder Eyelet Top", "Top", "Cider", "assets/casual_top_2.jpg"));
    casual.tops.append(ClothingItem(3, "Cropped Leather Jacket", "Top", "nuulu", "assets/casual_top_3.jpg"));
    casual.tops.append(ClothingItem(4, "Plaid Corset Top", "Top", "Zara", "assets/casual_top_4.jpg"));
    casual.tops.append(ClothingItem(5, "Pinstripe Tailored Blazer", "Top", "H&M", "assets/casual_top_5.jpg"));

    // 5 Casual Bottoms with photos
    casual.bottoms.append(ClothingItem(6, "Wide Leg Blue Jeans", "Bottom", "Levi's", "assets/casual_bottom_1.jpg"));
    casual.bottoms.append(ClothingItem(7, "Navy Tailored Trousers", "Bottom", "Zara", "assets/casual_bottom_2.jpg"));
    casual.bottoms.append(ClothingItem(8, "Cream Slit Midi Skirt", "Bottom", "Cider", "assets/casual_bottom_3.jpg"));
    casual.bottoms.append(ClothingItem(9, "Black Wrap Skort", "Bottom", "H&M", "assets/casual_bottom_4.jpg"));
    casual.bottoms.append(ClothingItem(10, "Bell Bottom Flare Jeans", "Bottom", "Diesel", "assets/casual_bottom_5.jpg"));

    occasions.push_back(casual);

    Occasion party(2, "Party");
    party.tops.append(ClothingItem(11, "Black Blouse", "Top", "Mango"));
    party.tops.append(ClothingItem(12, "Silk Shirt", "Top", "Gucci"));
    party.tops.append(ClothingItem(13, "Sequin Top", "Top", "Zara"));
    party.tops.append(ClothingItem(14, "White Button-up", "Top", "Calvin Klein"));
    party.tops.append(ClothingItem(15, "Red Blouse", "Top", "H&M"));

    party.bottoms.append(ClothingItem(16, "Black Skirt", "Bottom", "Zara"));
    party.bottoms.append(ClothingItem(17, "Dark Jeans", "Bottom", "Levi's"));
    party.bottoms.append(ClothingItem(18, "Slacks", "Bottom", "Armani"));
    party.bottoms.append(ClothingItem(19, "Leather Pants", "Bottom", "Mango"));
    party.bottoms.append(ClothingItem(20, "Midi Skirt", "Bottom", "H&M"));
    occasions.push_back(party);

    Occasion ceremony(3, "Ceremony");
    ceremony.tops.append(ClothingItem(21, "White Dress Shirt", "Top", "Brooks Brothers"));
    ceremony.tops.append(ClothingItem(22, "Formal Blazer", "Top", "Hugo Boss"));
    ceremony.tops.append(ClothingItem(23, "Silk Top", "Top", "Armani"));
    ceremony.tops.append(ClothingItem(24, "Vest", "Top", "Ralph Lauren"));
    ceremony.tops.append(ClothingItem(25, "Formal Blouse", "Top", "Chanel"));

    ceremony.bottoms.append(ClothingItem(26, "Black Suit Pants", "Bottom", "Hugo Boss"));
    ceremony.bottoms.append(ClothingItem(27, "Formal Trousers", "Bottom", "Ralph Lauren"));
    ceremony.bottoms.append(ClothingItem(28, "Long Skirt", "Bottom", "Gucci"));
    ceremony.bottoms.append(ClothingItem(29, "Pressed Slacks", "Bottom", "Brooks Brothers"));
    ceremony.bottoms.append(ClothingItem(30, "Formal Skirt", "Bottom", "Chanel"));
    occasions.push_back(ceremony);
}

int main() {
    const int screenWidth = 1000;
    const int screenHeight = 665;
    InitWindow(screenWidth, screenHeight, "Wardrobe Operating System - Outfit Builder");
    SetTargetFPS(60);

    std::vector<Occasion> occasions;
    initOccasions(occasions);

    WearQueue wearLog;
    UndoStack undoStack;

    Screen screen = Screen::OCCASION;
    int selectedOccasion = -1;

    // Active Doubly Linked List node pointers for Top and Bottom
    Node<ClothingItem>* currentTopNode = nullptr;
    Node<ClothingItem>* currentBottomNode = nullptr;

    std::string statusMsg = "Welcome to Wardrobe Operating System!";

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(0xF7F9FAFF));

        switch (screen) {

        case Screen::OCCASION: {
            DrawText("Choose an Occasion", 40, 30, 28, GetColor(0x2C3E50FF));
            DrawText("Select an occasion to build your outfit using Doubly Linked Lists:", 40, 68, 16, GRAY);

            for (size_t i = 0; i < occasions.size(); i++) {
                Rectangle btn = { 40, (float)(100 + i * 75), 320, 55 };
                if (DrawButton(btn, occasions[i].name.c_str())) {
                    selectedOccasion = (int)i;
                    // Initialize DLL node pointers to the heads of both DoublyLinkedLists
                    currentTopNode = occasions[selectedOccasion].tops.getHead();
                    currentBottomNode = occasions[selectedOccasion].bottoms.getHead();
                    screen = Screen::OUTFIT;
                    statusMsg = "Browsing " + occasions[selectedOccasion].name + " outfits via Doubly Linked List.";
                }
            }

            Rectangle historyBtn = { 40, 480, 240, 45 };
            if (DrawButton(historyBtn, "View Wear History")) {
                screen = Screen::HISTORY;
            }

            Rectangle undoBtn = { 300, 480, 220, 45 };
            if (DrawButton(undoBtn, "Undo Last Action")) {
                if (!undoStack.isEmpty()) {
                    Action act = undoStack.pop();
                    statusMsg = "Undid action: " + act.type + " (ID: " + std::to_string(act.outfitId) + ")";
                } else {
                    statusMsg = "Nothing to undo!";
                }
            }
            break;
        }

        case Screen::OUTFIT: {
            Occasion& occ = occasions[selectedOccasion];

            // --- HEADER NAVIGATION BAR ---
            Rectangle backBtn = { 30, 16, 150, 34 };
            if (DrawButton(backBtn, "< Back")) {
                screen = Screen::OCCASION;
            }

            std::string titleStr = occ.name + " Outfit Builder";
            DrawText(titleStr.c_str(), 205, 20, 24, GetColor(0x2C3E50FF));

            Rectangle histBtn = { 650, 16, 150, 34 };
            if (DrawButton(histBtn, "Wear History")) {
                screen = Screen::HISTORY;
            }

            Rectangle undoTopBtn = { 815, 16, 155, 34 };
            if (DrawButton(undoTopBtn, "Undo Action")) {
                if (!undoStack.isEmpty()) {
                    Action act = undoStack.pop();
                    statusMsg = "Undid action: " + act.type + " (ID: " + std::to_string(act.outfitId) + ")";
                } else {
                    statusMsg = "Nothing to undo!";
                }
            }

            // Central alignment calculation: center cards at X = 395
            const float cardWidth = 210.0f;
            const float cardHeight = 175.0f;
            const float centerX = (screenWidth - cardWidth) / 2.0f;

            // ==========================================
            // 1. TOP SECTION (ABOVE Bottom)
            // ==========================================
            const char* topHeader = "TOP";
            int topHdrWidth = MeasureText(topHeader, 18);
            DrawText(topHeader, (int)(screenWidth / 2 - topHdrWidth / 2), 65, 18, GetColor(0x1B4F72FF));

            Rectangle topCard = { centerX, 88.0f, cardWidth, cardHeight };
            if (currentTopNode != nullptr) {
                DrawVerticalClothingCard(topCard, currentTopNode->data);
            } else {
                DrawRectangleRec(topCard, WHITE);
                DrawRectangleLinesEx(topCard, 1.5f, LIGHTGRAY);
                DrawText("No Tops in List", (int)topCard.x + 40, (int)topCard.y + 75, 16, GRAY);
            }

            // Top Previous & Next Buttons directly below Top card
            Rectangle topPrevBtn = { centerX, 268.0f, 100.0f, 32.0f };
            Rectangle topNextBtn = { centerX + 110.0f, 268.0f, 100.0f, 32.0f };

            if (currentTopNode != nullptr) {
                // Top Previous button: traverses backward via prev
                if (DrawButton(topPrevBtn, "< Previous")) {
                    if (currentTopNode->prev != nullptr) {
                        currentTopNode = currentTopNode->prev;
                    } else {
                        currentTopNode = occ.tops.getTail(); // Wrap around DLL
                    }
                    statusMsg = "Top: " + currentTopNode->data.name;
                }

                // Top Next button: traverses forward via next
                if (DrawButton(topNextBtn, "Next >")) {
                    if (currentTopNode->next != nullptr) {
                        currentTopNode = currentTopNode->next;
                    } else {
                        currentTopNode = occ.tops.getHead(); // Wrap around DLL
                    }
                    statusMsg = "Top: " + currentTopNode->data.name;
                }
            }

            // ==========================================
            // 2. BOTTOM SECTION (BELOW Top)
            // ==========================================
            const char* bottomHeader = "BOTTOM";
            int botHdrWidth = MeasureText(bottomHeader, 18);
            DrawText(bottomHeader, (int)(screenWidth / 2 - botHdrWidth / 2), 312, 18, GetColor(0x1B4F72FF));

            Rectangle bottomCard = { centerX, 335.0f, cardWidth, cardHeight };
            if (currentBottomNode != nullptr) {
                DrawVerticalClothingCard(bottomCard, currentBottomNode->data);
            } else {
                DrawRectangleRec(bottomCard, WHITE);
                DrawRectangleLinesEx(bottomCard, 1.5f, LIGHTGRAY);
                DrawText("No Bottoms in List", (int)bottomCard.x + 30, (int)bottomCard.y + 75, 16, GRAY);
            }

            // Bottom Previous & Next Buttons directly below Bottom card
            Rectangle bottomPrevBtn = { centerX, 515.0f, 100.0f, 32.0f };
            Rectangle bottomNextBtn = { centerX + 110.0f, 515.0f, 100.0f, 32.0f };

            if (currentBottomNode != nullptr) {
                // Bottom Previous button: traverses backward via prev
                if (DrawButton(bottomPrevBtn, "< Previous")) {
                    if (currentBottomNode->prev != nullptr) {
                        currentBottomNode = currentBottomNode->prev;
                    } else {
                        currentBottomNode = occ.bottoms.getTail(); // Wrap around DLL
                    }
                    statusMsg = "Bottom: " + currentBottomNode->data.name;
                }

                // Bottom Next button: traverses forward via next
                if (DrawButton(bottomNextBtn, "Next >")) {
                    if (currentBottomNode->next != nullptr) {
                        currentBottomNode = currentBottomNode->next;
                    } else {
                        currentBottomNode = occ.bottoms.getHead(); // Wrap around DLL
                    }
                    statusMsg = "Bottom: " + currentBottomNode->data.name;
                }
            }

            // ==========================================
            // 4. OUTFIT BUTTON (BELOW both sections)
            // ==========================================
            Rectangle wearBtn = { (screenWidth - 240.0f) / 2.0f, 560.0f, 240.0f, 44.0f };
            if (DrawButton(wearBtn, "Wear This Outfit", false, DARKGREEN)) {
                if (currentTopNode && currentBottomNode) {
                    std::string topName = currentTopNode->data.name;
                    std::string bottomName = currentBottomNode->data.name;
                    std::string logEntry = occ.name + ": " + topName + " + " + bottomName;
                    
                    wearLog.enqueue(logEntry);
                    undoStack.push(Action{"WEAR_OUTFIT", occ.id});
                    statusMsg = "Logged: " + logEntry;
                }
            }

            // DLL Node Inspector Panel on the left side
            DrawRectangle(30, 88, 220, 160, WHITE);
            DrawRectangleLines(30, 88, 220, 160, GetColor(0xBDC3C7FF));
            DrawText("DLL Node Info", 42, 98, 15, GetColor(0x2C3E50FF));
            
            if (currentTopNode) {
                DrawText(TextFormat("Top ID: %d", currentTopNode->data.id), 42, 122, 13, DARKGRAY);
                DrawText(TextFormat("Prev: %s", currentTopNode->prev ? currentTopNode->prev->data.name.c_str() : "(Head)"), 42, 140, 12, GRAY);
                DrawText(TextFormat("Next: %s", currentTopNode->next ? currentTopNode->next->data.name.c_str() : "(Tail)"), 42, 158, 12, GRAY);
            }
            if (currentBottomNode) {
                DrawText(TextFormat("Bottom ID: %d", currentBottomNode->data.id), 42, 182, 13, DARKGRAY);
                DrawText(TextFormat("Prev: %s", currentBottomNode->prev ? currentBottomNode->prev->data.name.c_str() : "(Head)"), 42, 200, 12, GRAY);
                DrawText(TextFormat("Next: %s", currentBottomNode->next ? currentBottomNode->next->data.name.c_str() : "(Tail)"), 42, 218, 12, GRAY);
            }

            // Instructions on the right side
            DrawRectangle(750, 88, 220, 160, WHITE);
            DrawRectangleLines(750, 88, 220, 160, GetColor(0xBDC3C7FF));
            DrawText("DLL Navigation", 762, 98, 15, GetColor(0x2C3E50FF));
            DrawText("- [Next] moves via next", 762, 125, 12, DARKGRAY);
            DrawText("- [Previous] moves via prev", 762, 145, 12, DARKGRAY);
            DrawText("- Tops & bottoms navigate", 762, 165, 12, DARKGRAY);
            DrawText("  100% independently.", 762, 182, 12, DARKGRAY);
            DrawText("- Click [Wear] to enqueue.", 762, 207, 12, GetColor(0x27AE60FF));

            break;
        }

        case Screen::HISTORY: {
            DrawText("Wear History (FIFO Queue)", 40, 25, 26, GetColor(0x2C3E50FF));
            DrawText("Chronological record of worn outfits:", 40, 58, 16, GRAY);

            // Non-destructive read from WearQueue
            std::vector<std::string> logs = wearLog.getAllLogs();
            for (size_t i = 0; i < logs.size() && i < 11; i++) {
                DrawRectangle(40, (int)(95 + i * 38), 900, 32, WHITE);
                DrawRectangleLines(40, (int)(95 + i * 38), 900, 32, LIGHTGRAY);
                DrawText(TextFormat("%d.  %s", (int)(i + 1), logs[i].c_str()), 55, (int)(102 + i * 38), 16, DARKGRAY);
            }
            if (logs.empty()) {
                DrawText("No outfits worn yet.", 55, 110, 18, GRAY);
            }

            Rectangle popBtn = { 720, 520, 220, 45 };
            if (!logs.empty()) {
                if (DrawButton(popBtn, "Pop Oldest Log (Dequeue)")) {
                    wearLog.dequeue();
                    statusMsg = "Popped earliest log entry (FIFO)!";
                }
            }

            Rectangle backBtn = { 40, 520, 140, 45 };
            if (DrawButton(backBtn, "< Back")) {
                screen = (selectedOccasion >= 0) ? Screen::OUTFIT : Screen::OCCASION;
            }
            break;
        }
        }

        // Status bar at bottom
        DrawRectangle(0, screenHeight - 28, screenWidth, 28, GetColor(0xEAEDEDFF));
        DrawText(statusMsg.c_str(), 20, screenHeight - 20, 14, DARKGRAY);

        EndDrawing();
    }

    UnloadAllTextures();
    CloseWindow();
    return 0;
}

