
#include <string>
#include <string.h>
#include <map>
#include <fstream>
#include "CommandParser.hpp"

class CommandExecutor {
 private:
    std::string csv_file_;
    std::vector<std::vector<std::string>> look_up_table_;

    bool Insert(const ModifyCommandContent& command);
    bool LoadCSV(std::map<std::string, int>& col_names);
    bool DumpCSV();

    std::shared_ptr<Spliter> spliter_;
 
 public:
    CommandExecutor() {}
    explicit CommandExecutor(const std::string& csv_file) : csv_file_(csv_file) {
      spliter_ = std::make_shared<Spliter>();
    }
    bool Execute(const QueryCommandContent& command, std::vector<std::string>& results, std::string& msg);
    bool Execute(const ModifyCommandContent& command, std::string& msg);
    
};