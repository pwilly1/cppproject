#ifndef INVENTORYITEM_H
#define INVENTORYITEM_H
#include <string>
#pragma once

struct InventoryItem {
    std::string name;
    int amount;
    std::string filename;

    InventoryItem(const std::string& name = "", int amount = 0, const std::string& filename = "")
        : name(name), amount(amount), filename(filename){}

    bool canBreak() const {
        return name == "pickaxe";
    }
    bool canPlace() const {
        return name == "stone";
    }
};



#endif