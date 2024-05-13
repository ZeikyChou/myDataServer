#include <iostream>
#include "CommandExecutor.hpp"
bool CommandExecutor::LoadCSV(std::map<std::string, int>& col_names) {
    col_names.clear();
    look_up_table_.clear();
    std::ifstream file(csv_file_);
    if (!file.is_open()) {
        return false;
    }
    std::string line;
    int cnt = 0;
    while (std::getline(file, line)) {
        std::vector<std::string> values;
        std::string msg;
        if (!spliter_->Split(line, values, msg, ",")) {
            return false;
        }
        if (values.size() != 3) {
            return false;
        }
        if (cnt == 0) {
            col_names[values[0]] = 0;
            col_names[values[1]] = 1;
            col_names[values[2]] = 2;
            cnt++;
            continue;
        }
        look_up_table_.emplace_back(std::move(values));
    }
    file.close();
    return true;
}

bool CommandExecutor::DumpCSV() {
    std::ofstream file(csv_file_, std::ios_base::out | std::ios_base::trunc);
    if (!file.is_open()) {
        return false;
    }
    file << "Nation," << "Category," << "Entity\n";
    for (const auto& row : look_up_table_) {
        for (int i = 0; i < 3; ++i) {
            file << row[i];
            if (i != 2) {
                file << ",";
            } else {
                file << "\n";
            }
        }
    }
    file.close();
    look_up_table_.clear();
    return true;
}

bool CommandExecutor::Execute(const QueryCommandContent& command, std::vector<std::string>& results, std::string& msg) {
    std::map<std::string, int> col_names;

    this->LoadCSV(col_names);
    if (command.type == AND) {
        for (const auto& row : look_up_table_) {
            int term_count = command.cols.size();
            // std::cout << "term " << term_count << std::endl;
            int i = 0;
            for (; i < term_count; ++i) {
                if (command.relations[i] == "==") {
                    std::cout << command.cols[i] << std::endl;
                    std::cout << col_names[command.cols[i]] << std::endl;
                    if (row[col_names[command.cols[i]]] != command.values[i]) {
                        break;
                    }
                } else if (command.relations[i] == "!=") {
                    if (row[col_names[command.cols[i]]] == command.values[i]) {
                        break;
                    }
                } else if (command.relations[i] == "&="){
                    if (row[col_names[command.cols[i]]].find(command.values[i]) == std::string::npos) {
                        break;
                    }
                } else if (command.relations[i] == "$=") {
                    if (strcasecmp(row[col_names[command.cols[i]]].c_str(), command.values[i].c_str()) != 0) {
                        break;
                    }
                } else {
                    msg = "Parse error";
                    return false;
                }
            }
            if (i == term_count) {
                results.emplace_back(row[0]+","+row[1]+","+row[2]);
            }
        }
    } else if (command.type == OR) {
        bool hit = false;
        int term_count = command.cols.size();
        for (const auto& row : look_up_table_) {
            for (int i = 0; i < term_count; ++i) {
                if (command.relations[i] == "==") {
                    if (row[col_names[command.cols[i]]] == command.values[i]) {
                        results.emplace_back(row[0]+","+row[1]+","+row[2]);
                        break;
                    }
                } else if (command.relations[i] == "!=") {
                    if (row[col_names[command.cols[i]]] != command.values[i]) {
                        results.emplace_back(row[0]+","+row[1]+","+row[2]);
                        break;
                    }
                } else if (command.relations[i] == "&="){
                    std::cout << row[col_names[command.cols[i]]] << std::endl;
                    std::cout << command.values[i] << std::endl;
                    if (row[col_names[command.cols[i]]].find(command.values[i]) != std::string::npos) {
                        results.emplace_back(row[0]+","+row[1]+","+row[2]);
                        break;
                    }
                } else if (command.relations[i] == "$=") {
                    std::cout << row[col_names[command.cols[i]]] << std::endl;
                    std::cout << command.values[i] << std::endl;
                    if (strcasecmp(row[col_names[command.cols[i]]].c_str(), command.values[i].c_str()) == 0) {
                        results.emplace_back(row[0]+","+row[1]+","+row[2]);
                        break;
                    }
                } else {
                    msg = "Parse error";
                    return false;
                }
            }
        }
    } else {
        msg = "Parse error";
        return false;
    }
    return true;
}

bool CommandExecutor::Execute(const ModifyCommandContent& command, std::string& msg) {
    std::map<std::string, int> col_names;
    this->LoadCSV(col_names);
    if (command.type == INSERT) {
        look_up_table_.emplace_back(command.col_values);
        msg = "Insert success";
    } else if (command.type == DELETE) {
        bool flag = false;
        for (auto it = look_up_table_.begin(); it != look_up_table_.end();) {
            int i = 0;
            for (; i < command.col_values.size(); ++i) {
                if ((*it)[i] != command.col_values[i]) {
                    break;
                }
            }
            if (i == command.col_values.size()) {
                flag = true;
                it = look_up_table_.erase(it);
            } else {
                ++it;
            }
        }
        if (flag) {
            msg = "Delete success";
        } else {
            msg = "Nothing to delete";
        }
    } else if (command.type == UPDATE) {
        bool flag = false;
        for (auto it = look_up_table_.begin(); it != look_up_table_.end(); ++it) {
            int i = 0;
            for (; i < command.col_values.size(); ++i) {
                if ((*it)[i] != command.col_values[i]) {
                    break;
                }
            }
            if (i == command.col_values.size()) {
                flag = true;
                (*it)[col_names[command.update_col]] = command.new_value;
            }
        }
        if (flag) {
            msg = "Update success";
        } else {
            msg = "Update Nothing";
        }
    } else {
        msg = "Invalid modify operation";
        return false;
    }
    if (this->DumpCSV()) {
        return true;
    }
    msg = "Dump error";
    return false;
}