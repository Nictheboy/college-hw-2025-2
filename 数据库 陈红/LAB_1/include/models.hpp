#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

struct Product {
    std::string id;        // 商品编号（唯一）
    std::string name;      // 商品名称
    std::string category;  // 商品类别
    int64_t stock = 0;     // 当前库存
    bool deleted = false;  // 是否已删除（逻辑删除，记录仍保留）
};

enum class RecordType { Purchase,
                        Sale };

struct InventoryRecord {
    std::string productId;     // 商品编号
    std::string productName;   // 商品名称（冗余以便历史记录可读）
    RecordType type;           // 进货/销售
    std::string operatorName;  // 操作人
    std::string timestamp;     // ISO 8601: YYYY-MM-DD HH:MM:SS
    int64_t quantity = 0;      // 数量（正整数）
};

// 简单的时间校验与解析工具
namespace TimeUtil {
bool isValidDateTime(const std::string& ts);  // 检查格式与范围
}

// 通用错误类型
struct Error {
    std::string message;
};

// CSV序列化/反序列化
namespace CSV {
std::string escape(const std::string& s);
std::string join(const std::vector<std::string>& cols);
std::vector<std::string> split(const std::string& line);
}  // namespace CSV
