#include "Inventory.h"
#include "InventoryItem.h"
#include <iostream>

Inventory::Inventory() {
    inventoryVector.resize(10, InventoryItem{});   
    addItem("pickaxe", 1, "../../../resources/fc1763.png");
}

void Inventory::addItem(const std::string& name,
    int amount,
    const std::string& filename) {
    for (auto& slot : inventoryVector) {
        if (slot.name.empty()) {
            slot = InventoryItem(name, amount, filename);
            return;   
        }
    }
    std::cout << "inventory full\n";
}

void Inventory::removeItem(const std::string& name, int amount) {

	if (!inventoryVector[selectedIndex].name.empty()) {
		inventoryVector[selectedIndex].name = "";
		inventoryVector[selectedIndex].amount = 0;
		inventoryVector[selectedIndex].filename = "";
	}
}
