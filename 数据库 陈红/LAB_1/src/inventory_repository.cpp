#include "inventory_repository.hpp"

#include <algorithm>
#include <utility>

using std::optional;
using std::pair;
using std::string;
using std::vector;

InventoryRepository::InventoryRepository(CsvStorage storage)
    : storage(std::move(storage)) {}

optional<Error> InventoryRepository::load() {
    auto [loaded, err] = storage.loadProducts();
    if (err)
        return err;
    products = std::move(loaded);
    return std::nullopt;
}

optional<Error> InventoryRepository::persist() const {
    return storage.saveProducts(products);
}

optional<Error> InventoryRepository::addProduct(const Product& product) {
    if (product.id.empty())
        return Error{"商品编号不能为空"};
    if (product.name.empty())
        return Error{"商品名称不能为空"};
    if (product.category.empty())
        return Error{"商品类别不能为空"};
    if (product.stock < 0)
        return Error{"库存不能为负"};
    auto it = std::find_if(products.begin(), products.end(), [&](const Product& p) { return p.id == product.id; });
    if (it != products.end())
        return Error{"商品编号已存在"};
    products.push_back(product);
    return std::nullopt;
}

optional<Error> InventoryRepository::deleteProduct(const string& productId) {
    auto it = std::find_if(products.begin(), products.end(), [&](const Product& p) { return p.id == productId; });
    if (it == products.end())
        return Error{"商品不存在"};
    it->deleted = true;
    return std::nullopt;
}

optional<Error> InventoryRepository::updateProductStock(const string& productId, int64_t newStock) {
    if (newStock < 0)
        return Error{"库存不能为负"};
    for (auto& p : products) {
        if (p.id == productId) {
            p.stock = newStock;
            return std::nullopt;
        }
    }
    return Error{"商品不存在"};
}

optional<Product> InventoryRepository::findById(const string& productId) const {
    for (const auto& p : products)
        if (p.id == productId)
            return p;
    return std::nullopt;
}

vector<Product> InventoryRepository::listAll(bool includeDeleted) const {
    vector<Product> out;
    for (const auto& p : products)
        if (includeDeleted || !p.deleted)
            out.push_back(p);
    return out;
}

vector<Product> InventoryRepository::listByCategory(const string& category, bool includeDeleted) const {
    vector<Product> out;
    for (const auto& p : products)
        if (p.category == category && (includeDeleted || !p.deleted))
            out.push_back(p);
    return out;
}

optional<Error> InventoryRepository::appendRecord(const InventoryRecord& record) {
    return storage.appendRecord(record);
}

pair<vector<InventoryRecord>, optional<Error>> InventoryRepository::loadRecords() const {
    return storage.loadRecords();
}
