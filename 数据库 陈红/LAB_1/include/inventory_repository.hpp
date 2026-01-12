#pragma once

#include <optional>
#include <vector>
#include "csv_storage.hpp"
#include "models.hpp"

// 负责在内存层管理商品集合与持久化
class InventoryRepository {
   public:
    explicit InventoryRepository(CsvStorage storage);

    std::optional<Error> load();
    std::optional<Error> persist() const;

    // 基础CRUD（逻辑删除）
    std::optional<Error> addProduct(const Product& product);
    std::optional<Error> deleteProduct(const std::string& productId);
    std::optional<Error> updateProductStock(const std::string& productId, int64_t newStock);
    std::optional<Product> findById(const std::string& productId) const;
    std::vector<Product> listAll(bool includeDeleted = false) const;
    std::vector<Product> listByCategory(const std::string& category, bool includeDeleted = false) const;

    // 进货/销售记录
    std::optional<Error> appendRecord(const InventoryRecord& record);
    std::pair<std::vector<InventoryRecord>, std::optional<Error>> loadRecords() const;

   private:
    CsvStorage storage;
    std::vector<Product> products;
};
