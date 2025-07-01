#include "Inventory.h"
#include "InventoryItem.h"

Inventory::Inventory() {
	addItem("pickaxe", 1);

}

void Inventory::addItem(const std::string& name, int amount) {
	inventoryVector.push_back(InventoryItem(name, amount));

} 

void Inventory::removeItem(const std::string& name, int amount) {

}
