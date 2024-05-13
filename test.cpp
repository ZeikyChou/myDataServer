#include <bits/stdc++.h>
using namespace std;
#include <iostream>
// clang++ -std=c++17 -o test test.cpp
static const std::unordered_set<std::string> ColNames = {"Nation", "Category", "Entity", "*"};
static const std::unordered_set<std::string> Ops = {"and", "or"};
static const std::unordered_set<std::string> Relations = {"==", "!=", "&=", "$="};
bool Split(const std::string& src, std::vector<std::string>& dest, const std::string& separator) {
    dest.clear();
    size_t pos = 0;
    size_t len = src.length();
    std::string sentence;
    bool inQuotes = false;
    char escapeChar = '\\';
    std::string update_col;
    int tmp_cnt = 0;

    while (pos < len && src[pos] == ' ') {
        pos++;
    }
    while (pos < len) {
        if (src[pos] == '"' && (pos == 0 || src[pos - 1] != escapeChar)) {
            // 开始或结束引号包围的内容
            inQuotes = !inQuotes;
            ++pos;
        } else if (src[pos] == ',' && !inQuotes) {
            // 遇到逗号且不在引号内时，认为引号内容结束
            if (!sentence.empty()) {
                dest.push_back(sentence);
                sentence.clear();
            }
            pos++;
        } else if (src[pos] == escapeChar) {
            pos++;
        } else if (!inQuotes) {
            if (src[pos] != ' ') {
                // 遇到update列标志
                if (!update_col.empty()) {
                    return false;
                }
                while (pos < len && src[pos] != ' ' && src[pos] != ',') {
                    update_col += src[pos++];
                }
                if (ColNames.find(update_col) == ColNames.end()) {
                    return false;
                }
                tmp_cnt = dest.size();
                if (tmp_cnt <= 0 || tmp_cnt > 3) {
                    return false;
                }
            } else {
                pos++;
            }
        } else {
            sentence += src[pos++];
        }
    }

    if (!sentence.empty()) {
        dest.push_back(sentence);
    }
    if (dest.size() != tmp_cnt + 1) {
        return false;
    }
    dest.push_back(update_col);
    return true;
}
int main() {
    vector<string> res;
    string s(" \"China\" , \"Agric\\\"ulture\",\" and\" ,Entity , \"Wheat\"  ");
    string s(" Entity , \"Wheat\"  ");
    // string s;
    // cin >> s;
    cout << Split(s, res, "") << endl;
    for (auto i : res) cout << i << endl;
}