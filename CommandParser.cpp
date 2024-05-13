#include <iostream>
#include "CommandParser.hpp"
bool Spliter::Split(const std::string& src, std::vector<std::string>& dest, std::string& msg, const std::string& separator) {
	std::string str = src;
	std::string substring;
	std::string::size_type start = 0, index;
	dest.clear();
	index = str.find_first_of(separator, start);
	do {
		if (index != std::string::npos) {    
			substring = str.substr(start, index - start);
			dest.push_back(substring);
			start = index + separator.size();
			index = str.find(separator, start);
			if (start == std::string::npos) break;
		}
	} while (index != std::string::npos);
	substring = str.substr(start);
	dest.push_back(substring);
    if (dest.empty()) {
        msg = "No separator appears";
        return false;
    }
    return true;
}

bool QuerySpliter::Split(const std::string& src, std::vector<std::string>& dest, std::string& msg, const std::string& separator) {
    dest.clear();
    size_t pos = 0;
    size_t len = src.length();
    std::string sentence;
    // bool inQuotes = false;
    char escapeChar = '\\';
    // int step = 0;
    while (pos < len && src[pos] == ' ') {
        pos++;
    }
    std::string query = src.substr(pos);
    std::string op;

    while(true) {
        pos = query.find_first_of(' ');
        if (pos == std::string::npos) {
            return false;
        }
        std::string tmp = query.substr(0, pos);
        if (ColNames.find(tmp) == ColNames.end()) {
            msg = "Invalid column name";
            return false;
        }
        dest.push_back(tmp);
        query = query.substr(query.find_first_of(' ') + 1);
        while (!query.empty() && query[0] == ' ') {
            query = query.substr(1);
        }
        if (query.empty()) {
            return false;
        }
        pos = query.find_first_of(' ');
        if (pos == std::string::npos) {
            return false;
        }
        tmp = query.substr(0, pos);
        if (Relations.find(tmp) == Relations.end()) {
            msg = "Invalid compare signal";
            return false;
        }
        dest.push_back(tmp);
        query = query.substr(pos + 1);
        while (!query.empty() && query[0] == ' ') {
            query = query.substr(1);
        }
        if (query.empty()) {
            return false;
        }
        if (query[0] == '"') {
            pos = 1;
            sentence.clear();
            while (pos < query.length()) {
                if (query[pos] == escapeChar) {
                    ++pos;
                } else if (query[pos] == '"' && query[pos - 1] != escapeChar) {
                    break;
                }
                sentence += query[pos++];
            }
            dest.push_back(sentence);
        } else {
            return false;
        }
        query = query.substr(pos + 1);
        while (!query.empty() && query[0] == ' ') {
            query = query.substr(1);
        }
        if (query.empty()) {
            break;
        }
        pos = query.find_first_of(' ');
        tmp = query.substr(0, pos);
        if (Ops.find(tmp) == Ops.end() || (!op.empty() && op != tmp)) {
            msg = "Invalid logical operator";
            return false;
        }
        op = tmp;
        query = query.substr(pos + 1);
        while (!query.empty() && query[0] == ' ') {
            query = query.substr(1);
        }
        if (query.empty()) {
            return false;
        }
    }
    dest.push_back(op);
    return true;
}

static bool DefaultSplit(const std::string& src, std::vector<std::string>& dest, std::string& msg) {
    dest.clear();
    size_t pos = 0;
    size_t len = src.length();
    std::string sentence;
    bool inQuotes = false;
    char escapeChar = '\\';

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
            dest.push_back(sentence);
            sentence.clear();
            pos++;
        } else if (src[pos] == escapeChar || !inQuotes) {
            if (!inQuotes && src[pos] != ' ') {
                msg = "Invalid character outside quotes";
                return false;
            }
            // 跳过当前字符
            pos++;
        } else {
            sentence += src[pos++];
        }
    }

    if (!sentence.empty()) {
        dest.push_back(sentence);
    }
    return true;
}

bool InsertSpliter::Split(const std::string& src, std::vector<std::string>& dest, std::string& msg, const std::string& separator) {
    bool ret = DefaultSplit(src, dest, msg);
    if (!ret || dest.size() != 3) {
        if (msg.empty()) {
            msg = "Insert term should have 3 columns";
        }
        return false;
    }
    return true;
}

bool DeleteSpliter::Split(const std::string& src, std::vector<std::string>& dest, std::string& msg, const std::string& separator) {
    bool ret = DefaultSplit(src, dest, msg);
    if (!ret || dest.size() > 3) {
        if (msg.empty()) {
            msg = "Delete term should have 1-3 columns";
        }
        return false;
    }
    return true;
}

bool UpdateSpliter::Split(const std::string& src, std::vector<std::string>& dest, std::string& msg, const std::string& separator) {
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
                    msg = "More than one update column";
                    return false;
                }
                while (pos < len && src[pos] != ' ' && src[pos] != ',') {
                    update_col += src[pos++];
                }
                if (ColNames.find(update_col) == ColNames.end()) {
                    msg = "Invalid column name";
                    return false;
                }
                tmp_cnt = dest.size();
                if (tmp_cnt <= 0 || tmp_cnt > 3) {
                    msg = "Invalid column count";
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
        msg = "Invalid update value count";
        return false;
    }
    dest.push_back(update_col);
    return true;
}

bool ModifyCommandParser::Parse(const std::string& command, ModifyCommandContent& output, std::string& msg) {
    std::vector<std::string> components;
    auto index = command.find_first_of(' ');
    std::string method = command.substr(0, index);
    std::string content = command.substr(index+1);
    if (method == "INSERT") {
        output.type = INSERT;
        spliter_ = std::make_shared<InsertSpliter>();
        if (!spliter_->Split(content, components, msg)) {
            return false;
        }
        output.col_values = components;
    } else if (method == "DELETE") {
        output.type = DELETE;
        spliter_ = std::make_shared<DeleteSpliter>();
        if (!spliter_->Split(content, components, msg)) {
            return false;
        }
        output.col_values = components;
    } else if (method == "UPDATE") {
        output.type = UPDATE;
        spliter_ = std::make_shared<UpdateSpliter>();
        if (!spliter_->Split(content, components, msg)) {
            return false;
        }
        output.update_col = components.back();
        components.pop_back();
        output.new_value = components.back();
        components.pop_back();
        output.col_values = components;
    } else {
        msg = "Invalid modify operation";
        return false;
    }
    return true;
}

bool QueryCommandParser::Parse(const std::string& command, QueryCommandContent& output, std::string& msg) {
    std::vector<std::string> components;
    spliter_ = std::make_shared<QuerySpliter>();
    if (!spliter_->Split(command, components, msg) || components.size() % 3 != 1) {
        if (msg.empty()) {
            msg = "Other invalid query command";
        }
        return false;
    }
    if (components.back() == "and") {
        output.type = AND;
    } else if (components.back() == "or") {
        output.type = OR;
    } else if (components.back().empty()) {
        output.type = AND;
    } else {
        if (msg.empty()) {
            msg = "Split error";
        }
        return false;
    }
    components.pop_back();
    for (size_t i = 0; i < components.size(); i += 3) {
        output.cols.push_back(components[i]);
        output.relations.push_back(components[i+1]);
        output.values.push_back(components[i+2]);
    }
    return true;
}