#include "Inventory.h"
#include "InventoryItem.h"
#include <iostream>

Inventory::Inventory() {
    inventoryVector.resize(10, InventoryItem{});   
    addItem("pickaxe", 1, "resources/fc1763.png");
}

void Inventory::addItem(const std::string& name,
    int amount,
    const std::string& filename) {
    while (amount > 0) {
        // First try to top up an existing stack of the same item
        bool added = false;
        for (auto& slot : inventoryVector) {
            if (slot.name == name && slot.amount < InventoryItem::MAX_STACK) {
                int space = InventoryItem::MAX_STACK - slot.amount;
                int adding = std::min(amount, space);
                slot.amount += adding;
                amount -= adding;
                added = true;
                break;
            }
        }

        if (!added) {
            // No existing stack has room, find an empty slot
            for (auto& slot : inventoryVector) {
                if (slot.name.empty()) {
                    int adding = std::min(amount, InventoryItem::MAX_STACK);
                    slot = InventoryItem(name, adding, filename);
                    amount -= adding;
                    added = true;
                    break;
                }
            }
        }

        if (!added) {
            std::cout << "inventory full\n";
            return;
        }
    }
}

void Inventory::removeItem(const std::string& name, int amount) {
	for (auto& slot : inventoryVector) {
		if (slot.name == name && slot.amount > 0) {
			slot.amount -= amount;
			if (slot.amount <= 0) {
				slot.name = "";
				slot.amount = 0;
				slot.filename = "";
			}
			return;
		}
	}
}
