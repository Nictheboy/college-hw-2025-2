#pragma once

#include "inventory_service.hpp"

class CLI {
   public:
    CLI(InventoryService& service);
    int run();

   private:
    void printMenu() const;
    void handleAddProduct();
    void handleBrowseByCategory();
    void handlePurchase();
    void handleSale();
    void handleDeleteProduct();
    void handleQueryRecords();
    void handleSummarizeSales();
    void handleListAll();

    std::string prompt(const char* label) const;

    InventoryService& service;
};
