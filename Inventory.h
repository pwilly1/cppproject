#ifndef INVENTORY_H
#define INVENTORY_H
#include <string>
#include <vector>
#include "InventoryItem.h"
class Inventory {


public:

	Inventory();
	void setSelectedIndex(int index) { if (index <= inventoryVector.size() - 1) { selectedIndex = index; } }
	const int getSelectedIndex() const { return selectedIndex; }
	InventoryItem getItem() { return inventoryVector[selectedIndex]; }
	void addItem(const std::string& name, int amount, const std::string& filename);
	void removeItem(const std::string& name, int amount);
	const std::vector<InventoryItem> getInventory() { return inventoryVector; }



private:
	int selectedIndex = 0;
	std::vector<InventoryItem> inventoryVector;


};




#endif