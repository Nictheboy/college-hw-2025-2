#pragma once

#include <optional>
#include <string>
#include <vector>
#include "models.hpp"

// 负责与文件系统交互的简单CSV存储层
class CsvStorage {
   public:
    explicit CsvStorage(const std::string& baseDir);

    // products.csv: id,name,category,stock,deleted
    std::optional<Error> saveProducts(const std::vector<Product>& products) const;
    std::pair<std::vector<Product>, std::optional<Error>> loadProducts() const;

    // records.csv: productId,productName,type,operator,timestamp,quantity
    std::optional<Error> appendRecord(const InventoryRecord& rec) const;
    std::pair<std::vector<InventoryRecord>, std::optional<Error>> loadRecords() const;

   private:
    std::string productsPath;
    std::string recordsPath;
};
