#include "inventory_service.hpp"

#include <algorithm>

using std::optional;
using std::pair;
using std::string;
using std::vector;

InventoryService::InventoryService(InventoryRepository repo)
    : repo(std::move(repo)) {}

vector<Product> InventoryService::browseByCategory(const string& category, bool sortByStockDesc) const {
    auto list = repo.listByCategory(category, false);
    std::sort(list.begin(), list.end(), [&](const Product& a, const Product& b) {
        return sortByStockDesc ? (a.stock > b.stock) : (a.stock < b.stock);
    });
    return list;
}

static optional<Error> validateCommon(const Product& p, int64_t quantity, const string& operatorName, const string& timestamp) {
    if (quantity <= 0)
        return Error{"数量必须为正数"};
    if (operatorName.empty())
        return Error{"操作人不能为空"};
    if (!TimeUtil::isValidDateTime(timestamp))
        return Error{"时间格式应为 YYYY-MM-DD HH:MM:SS"};
    return std::nullopt;
}

optional<Error> InventoryService::purchase(const string& productId, int64_t quantity, const string& operatorName, const string& timestamp) {
    auto pOpt = repo.findById(productId);
    if (!pOpt)
        return Error{"商品不存在"};
    auto p = *pOpt;
    if (auto e = validateCommon(p, quantity, operatorName, timestamp))
        return e;
    int64_t newStock = p.stock + quantity;
    if (auto e = repo.updateProductStock(p.id, newStock))
        return e;
    if (auto e = repo.persist())
        return e;
    InventoryRecord rec{p.id, p.name, RecordType::Purchase, operatorName, timestamp, quantity};
    if (auto e = repo.appendRecord(rec))
        return e;
    return std::nullopt;
}

optional<Error> InventoryService::sale(const string& productId, int64_t quantity, const string& operatorName, const string& timestamp) {
    auto pOpt = repo.findById(productId);
    if (!pOpt)
        return Error{"商品不存在"};
    auto p = *pOpt;
    if (auto e = validateCommon(p, quantity, operatorName, timestamp))
        return e;
    if (p.stock < quantity)
        return Error{"库存不足"};
    int64_t newStock = p.stock - quantity;
    if (auto e = repo.updateProductStock(p.id, newStock))
        return e;
    if (auto e = repo.persist())
        return e;
    InventoryRecord rec{p.id, p.name, RecordType::Sale, operatorName, timestamp, quantity};
    if (auto e = repo.appendRecord(rec))
        return e;
    return std::nullopt;
}

optional<Error> InventoryService::deleteProduct(const string& productId) {
    return repo.deleteProduct(productId);
}

pair<vector<InventoryRecord>, optional<Error>> InventoryService::queryRecords(const RecordQuery& q) const {
    auto [records, err] = repo.loadRecords();
    if (err)
        return {{}, err};
    vector<InventoryRecord> out;
    for (const auto& r : records) {
        if (q.productId && r.productId != *q.productId)
            continue;
        if (q.operatorName && r.operatorName != *q.operatorName)
            continue;
        if (q.fromTs && r.timestamp < *q.fromTs)
            continue;
        if (q.toTs && r.timestamp > *q.toTs)
            continue;
        out.push_back(r);
    }
    return {out, std::nullopt};
}

pair<int64_t, optional<Error>> InventoryService::summarizeSales(const optional<string>& category,
                                                                const optional<string>& fromTs,
                                                                const optional<string>& toTs) const {
    auto [records, err] = repo.loadRecords();
    if (err)
        return {0, err};
    int64_t total = 0;
    for (const auto& r : records) {
        if (r.type != RecordType::Sale)
            continue;
        if (fromTs && r.timestamp < *fromTs)
            continue;
        if (toTs && r.timestamp > *toTs)
            continue;
        if (category) {
            // 需要按类别过滤，需读取商品列表
            auto list = repo.listAll(true);
            auto it = std::find_if(list.begin(), list.end(), [&](const Product& p) { return p.id == r.productId; });
            if (it == list.end() || it->category != *category)
                continue;
        }
        total += r.quantity;
    }
    return {total, std::nullopt};
}

optional<Error> InventoryService::addProduct(const Product& p) {
    return repo.addProduct(p);
}

vector<Product> InventoryService::listAll(bool includeDeleted) const {
    return repo.listAll(includeDeleted);
}

optional<Error> InventoryService::persist() const {
    return repo.persist();
}
optional<Error> InventoryService::load() {
    return repo.load();
}
