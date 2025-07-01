#ifndef INVENTORY_H
#define INVENTORY_H
#include <string>
#include <vector>
#include "InventoryItem.h"
class Inventory {


public:

	Inventory();
	void addItem(const std::string& name, int amount);
	void removeItem(const std::string& name, int amount);
	std::vector<InventoryItem> getInventory() { return inventoryVector; }



private:

	std::vector<InventoryItem> inventoryVector;


};




#endif