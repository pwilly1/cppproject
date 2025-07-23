#include "Inventory.h"
#include "InventoryItem.h"
#include <iostream>

Inventory::Inventory() {
	addItem("pickaxe", 1, "../../../resources/fc1763.png");

}


void Inventory::addItem(const std::string& name, int amount, const std::string& filename) {

	if (inventoryVector.size() <= 10) {
		inventoryVector.push_back(InventoryItem(name, amount, filename));
	}
	else {
		std::cout << "inventory full" << std::endl;
	}
} 

void Inventory::removeItem(const std::string& name, int amount) {

	if (!inventoryVector[selectedIndex].name.empty()) {
		inventoryVector[selectedIndex].name = "";
		inventoryVector[selectedIndex].amount = 0;
		inventoryVector[selectedIndex].filename = "";
	}
}
