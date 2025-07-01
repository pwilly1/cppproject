#ifndef INVENTORYITEM_H
#define INVENTORYITEM_H
#include <string>
#pragma once

struct InventoryItem {
    std::string name;
    int amount;

    InventoryItem(const std::string& name = "", int amount = 0)
        : name(name), amount(amount) {}
};



#endif