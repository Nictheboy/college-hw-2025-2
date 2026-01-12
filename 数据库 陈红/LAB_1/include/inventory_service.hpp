#pragma once

#include <optional>
#include <vector>
#include "inventory_repository.hpp"

struct RecordQuery {
    std::optional<std::string> productId;
    std::optional<std::string> operatorName;
    std::optional<std::string> fromTs;  // inclusive
    std::optional<std::string> toTs;    // inclusive
};

class InventoryService {
   public:
    explicit InventoryService(InventoryRepository repo);

    // 商品目录查看（按类别）
    std::vector<Product> browseByCategory(const std::string& category, bool sortByStockDesc) const;

    // 进货与销售
    std::optional<Error> purchase(const std::string& productId, int64_t quantity, const std::string& operatorName, const std::string& timestamp);
    std::optional<Error> sale(const std::string& productId, int64_t quantity, const std::string& operatorName, const std::string& timestamp);

    // 删除商品（保留记录）
    std::optional<Error> deleteProduct(const std::string& productId);

    // 记录查询
    std::pair<std::vector<InventoryRecord>, std::optional<Error>> queryRecords(const RecordQuery& q) const;

    // 销量汇总
    std::pair<int64_t, std::optional<Error>> summarizeSales(const std::optional<std::string>& category,
                                                            const std::optional<std::string>& fromTs,
                                                            const std::optional<std::string>& toTs) const;

    // 基础商品管理
    std::optional<Error> addProduct(const Product& p);
    std::vector<Product> listAll(bool includeDeleted = false) const;
    std::optional<Error> persist() const;  // 保存到磁盘
    std::optional<Error> load();           // 从磁盘加载

   private:
    InventoryRepository repo;
};
