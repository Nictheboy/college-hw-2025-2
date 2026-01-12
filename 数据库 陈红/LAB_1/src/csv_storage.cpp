#include "csv_storage.hpp"

#include <filesystem>
#include <fstream>

using std::optional;
using std::pair;
using std::string;
using std::vector;

CsvStorage::CsvStorage(const string& baseDir) {
    std::filesystem::create_directories(baseDir);
    productsPath = baseDir + "/products.csv";
    recordsPath = baseDir + "/records.csv";
}

optional<Error> CsvStorage::saveProducts(const vector<Product>& products) const {
    std::ofstream ofs(productsPath, std::ios::trunc);
    if (!ofs.is_open())
        return Error{"无法写入产品文件"};
    // header
    ofs << "id,name,category,stock,deleted\n";
    for (const auto& p : products) {
        ofs << CSV::join({p.id, p.name, p.category, std::to_string(p.stock), p.deleted ? "1" : "0"}) << '\n';
    }
    return std::nullopt;
}

pair<vector<Product>, optional<Error>> CsvStorage::loadProducts() const {
    vector<Product> out;
    std::ifstream ifs(productsPath);
    if (!ifs.is_open())
        return {out, std::nullopt};
    string line;
    bool first = true;
    while (std::getline(ifs, line)) {
        if (first) {
            first = false;
            if (line.rfind("id,", 0) == 0)
                continue;
        }
        auto cols = CSV::split(line);
        if (cols.size() < 5)
            continue;
        Product p;
        p.id = cols[0];
        p.name = cols[1];
        p.category = cols[2];
        p.stock = std::stoll(cols[3]);
        p.deleted = (cols[4] == "1");
        out.push_back(p);
    }
    return {out, std::nullopt};
}

optional<Error> CsvStorage::appendRecord(const InventoryRecord& rec) const {
    std::ofstream ofs(recordsPath, std::ios::app);
    if (!ofs.is_open())
        return Error{"无法写入记录文件"};
    string typeStr = (rec.type == RecordType::Purchase) ? "purchase" : "sale";
    ofs << CSV::join({rec.productId, rec.productName, typeStr, rec.operatorName, rec.timestamp, std::to_string(rec.quantity)}) << '\n';
    return std::nullopt;
}

pair<vector<InventoryRecord>, optional<Error>> CsvStorage::loadRecords() const {
    vector<InventoryRecord> out;
    std::ifstream ifs(recordsPath);
    if (!ifs.is_open())
        return {out, std::nullopt};
    string line;
    while (std::getline(ifs, line)) {
        auto cols = CSV::split(line);
        if (cols.size() < 6)
            continue;
        InventoryRecord r;
        r.productId = cols[0];
        r.productName = cols[1];
        r.type = (cols[2] == "purchase") ? RecordType::Purchase : RecordType::Sale;
        r.operatorName = cols[3];
        r.timestamp = cols[4];
        r.quantity = std::stoll(cols[5]);
        out.push_back(r);
    }
    return {out, std::nullopt};
}
