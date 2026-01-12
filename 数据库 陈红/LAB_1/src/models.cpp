#include "models.hpp"

#include <regex>
#include <sstream>

namespace TimeUtil {
bool isValidDateTime(const std::string& ts) {
    // 简单校验 YYYY-MM-DD HH:MM:SS
    static const std::regex re(R"((\d{4})-(\d{2})-(\d{2}) (\d{2}):(\d{2}):(\d{2}))");
    std::smatch m;
    if (!std::regex_match(ts, m, re))
        return false;
    int y = std::stoi(m[1]);
    int mo = std::stoi(m[2]);
    int d = std::stoi(m[3]);
    int h = std::stoi(m[4]);
    int mi = std::stoi(m[5]);
    int s = std::stoi(m[6]);
    if (y < 1970 || y > 2100)
        return false;
    if (mo < 1 || mo > 12)
        return false;
    if (d < 1 || d > 31)
        return false;  // 简化，不考虑大小月与闰年
    if (h < 0 || h > 23)
        return false;
    if (mi < 0 || mi > 59)
        return false;
    if (s < 0 || s > 59)
        return false;
    return true;
}
}  // namespace TimeUtil

namespace CSV {
static bool needsEscapeChar(char c) {
    return c == '"' || c == ',' || c == '\n' || c == '\r';
}

std::string escape(const std::string& s) {
    bool needQuotes = false;
    for (char c : s) {
        if (needsEscapeChar(c) || c == ' ') {
            needQuotes = true;
            break;
        }
    }
    if (!needQuotes)
        return s;
    std::ostringstream oss;
    oss << '"';
    for (char c : s) {
        if (c == '"')
            oss << '"';
        oss << c;
    }
    oss << '"';
    return oss.str();
}

std::string join(const std::vector<std::string>& cols) {
    std::ostringstream oss;
    for (size_t i = 0; i < cols.size(); ++i) {
        if (i)
            oss << ',';
        oss << escape(cols[i]);
    }
    return oss.str();
}

std::vector<std::string> split(const std::string& line) {
    std::vector<std::string> out;
    std::string cur;
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (inQuotes) {
            if (c == '"') {
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    cur.push_back('"');
                    ++i;
                } else {
                    inQuotes = false;
                }
            } else {
                cur.push_back(c);
            }
        } else {
            if (c == ',') {
                out.push_back(cur);
                cur.clear();
            } else if (c == '"') {
                inQuotes = true;
            } else {
                cur.push_back(c);
            }
        }
    }
    out.push_back(cur);
    return out;
}
}  // namespace CSV
