#include "cli.hpp"

#include <iostream>
#include <limits>

using std::cin;
using std::cout;
using std::endl;
using std::string;

CLI::CLI(InventoryService& service) : service(service) {}

string CLI::prompt(const char* label) const {
    cout << label << ": ";
    string s;
    std::getline(cin, s);
    return s;
}

void CLI::printMenu() const {
    cout << "\n=== 商城库存管理 ===\n";
    cout << "1) 添加商品\n";
    cout << "2) 按类别浏览（库存排序）\n";
    cout << "3) 进货\n";
    cout << "4) 销售\n";
    cout << "5) 删除商品\n";
    cout << "6) 查询进销记录\n";
    cout << "7) 销量汇总\n";
    cout << "8) 列出所有商品\n";
    cout << "0) 退出\n";
}

void CLI::handleAddProduct() {
    Product p;
    p.id = prompt("商品编号");
    p.name = prompt("商品名称");
    p.category = prompt("商品类别");
    string stockStr = prompt("初始库存(整数) ");
    try {
        p.stock = std::stoll(stockStr);
    } catch (...) {
        cout << "库存必须为整数\n";
        return;
    }
    if (p.stock < 0) {
        cout << "库存不能为负\n";
        return;
    }
    if (auto e = service.addProduct(p)) {
        cout << e->message << "\n";
        return;
    }
    if (auto e = service.persist()) {
        cout << e->message << "\n";
        return;
    }
    cout << "添加成功\n";
}

void CLI::handleBrowseByCategory() {
    string cat = prompt("类别");
    string order = prompt("按库存排序(asc/desc)");
    bool desc = (order == "desc");
    auto list = service.browseByCategory(cat, desc);
    cout << "ID\t名称\t类别\t库存\n";
    for (const auto& p : list) {
        cout << p.id << '\t' << p.name << '\t' << p.category << '\t' << p.stock << '\n';
    }
}

void CLI::handlePurchase() {
    string id = prompt("商品编号");
    string qtyStr = prompt("数量");
    string op = prompt("操作人");
    string ts = prompt("时间(YYYY-MM-DD HH:MM:SS)");
    int64_t qty = 0;
    try {
        qty = std::stoll(qtyStr);
    } catch (...) {
        cout << "数量需为整数\n";
        return;
    }
    if (auto e = service.purchase(id, qty, op, ts)) {
        cout << e->message << "\n";
        return;
    }
    cout << "进货成功\n";
}

void CLI::handleSale() {
    string id = prompt("商品编号");
    string qtyStr = prompt("数量");
    string op = prompt("操作人");
    string ts = prompt("时间(YYYY-MM-DD HH:MM:SS)");
    int64_t qty = 0;
    try {
        qty = std::stoll(qtyStr);
    } catch (...) {
        cout << "数量需为整数\n";
        return;
    }
    if (auto e = service.sale(id, qty, op, ts)) {
        cout << e->message << "\n";
        return;
    }
    cout << "销售成功\n";
}

void CLI::handleDeleteProduct() {
    string id = prompt("商品编号");
    if (auto e = service.deleteProduct(id)) {
        cout << e->message << "\n";
        return;
    }
    if (auto e = service.persist()) {
        cout << e->message << "\n";
        return;
    }
    cout << "已删除(逻辑)\n";
}

void CLI::handleQueryRecords() {
    RecordQuery q;
    string pid = prompt("按商品编号过滤(可留空)");
    if (!pid.empty())
        q.productId = pid;
    string op = prompt("按操作人过滤(可留空)");
    if (!op.empty())
        q.operatorName = op;
    string from = prompt("开始时间(可留空)");
    if (!from.empty())
        q.fromTs = from;
    string to = prompt("结束时间(可留空)");
    if (!to.empty())
        q.toTs = to;
    auto [recs, err] = service.queryRecords(q);
    if (err) {
        cout << err->message << "\n";
        return;
    }
    cout << "商品\t类型\t操作人\t时间\t数量\n";
    for (const auto& r : recs) {
        cout << r.productName << '\t' << (r.type == RecordType::Purchase ? "进货" : "销售")
             << '\t' << r.operatorName << '\t' << r.timestamp << '\t' << r.quantity << '\n';
    }
}

void CLI::handleSummarizeSales() {
    string cat = prompt("按类别汇总(可留空)");
    string from = prompt("开始时间(可留空)");
    string to = prompt("结束时间(可留空)");
    std::optional<string> catOpt, fromOpt, toOpt;
    if (!cat.empty())
        catOpt = cat;
    if (!from.empty())
        fromOpt = from;
    if (!to.empty())
        toOpt = to;
    auto [sum, err] = service.summarizeSales(catOpt, fromOpt, toOpt);
    if (err) {
        cout << err->message << "\n";
        return;
    }
    cout << "总销量: " << sum << '\n';
}

void CLI::handleListAll() {
    auto list = service.listAll(false);
    cout << "ID\t名称\t类别\t库存\n";
    for (const auto& p : list)
        cout << p.id << '\t' << p.name << '\t' << p.category << '\t' << p.stock << '\n';
}

int CLI::run() {
    while (true) {
        printMenu();
        cout << "选择: ";
        string choice;
        std::getline(cin, choice);
        if (choice == "0" || cin.eof())
            break;
        if (choice == "1")
            handleAddProduct();
        else if (choice == "2")
            handleBrowseByCategory();
        else if (choice == "3")
            handlePurchase();
        else if (choice == "4")
            handleSale();
        else if (choice == "5")
            handleDeleteProduct();
        else if (choice == "6")
            handleQueryRecords();
        else if (choice == "7")
            handleSummarizeSales();
        else if (choice == "8")
            handleListAll();
        else
            cout << "无效选择\n";
    }
    return 0;
}
