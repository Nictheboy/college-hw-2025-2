#include "cli.hpp"
#include "csv_storage.hpp"
#include "inventory_repository.hpp"
#include "inventory_service.hpp"

#include <iostream>

int main() {
    // 数据文件目录置于 data/
    CsvStorage storage("data");
    InventoryRepository repo(storage);
    if (auto e = repo.load()) {
        std::cerr << e->message << "\n";
        return 1;
    }
    InventoryService service(repo);
    CLI cli(service);
    return cli.run();
}
