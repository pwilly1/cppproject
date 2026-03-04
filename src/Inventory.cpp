#include "Inventory.h"
#include "InventoryItem.h"
#include "tinyxml2.h"
#include <iostream>

using namespace tinyxml2;

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

void Inventory::clear() {
	inventoryVector.assign(10, InventoryItem{});
	selectedIndex = 0;
	addItem("pickaxe", 1, "resources/fc1763.png");
}

void Inventory::saveToXML(XMLDocument& doc, XMLElement* root) {
	XMLElement* invEl = doc.NewElement("inventory");
	for (int i = 0; i < (int)inventoryVector.size(); i++) {
		XMLElement* slotEl = doc.NewElement("slot");
		slotEl->SetAttribute("index", i);
		slotEl->SetAttribute("name", inventoryVector[i].name.c_str());
		slotEl->SetAttribute("amount", inventoryVector[i].amount);
		slotEl->SetAttribute("filename", inventoryVector[i].filename.c_str());
		invEl->InsertEndChild(slotEl);
	}
	root->InsertEndChild(invEl);
}

void Inventory::loadFromXML(XMLElement* inventoryElement) {
	if (!inventoryElement) return;

	inventoryVector.assign(10, InventoryItem{});

	for (XMLElement* slotEl = inventoryElement->FirstChildElement("slot");
		slotEl;
		slotEl = slotEl->NextSiblingElement("slot")) {
		int index = slotEl->IntAttribute("index", -1);
		if (index < 0 || index >= (int)inventoryVector.size()) continue;
		inventoryVector[index].name     = slotEl->Attribute("name")     ? slotEl->Attribute("name")     : "";
		inventoryVector[index].amount   = slotEl->IntAttribute("amount", 0);
		inventoryVector[index].filename = slotEl->Attribute("filename") ? slotEl->Attribute("filename") : "";
	}
	std::cout << "Inventory loaded from save.\n";
}
