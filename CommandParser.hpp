
#include <string>
#include <unordered_set>

enum CommandType {
    INSERT,
    DELETE,
    UPDATE,
    AND,
    OR
};

static const std::unordered_set<std::string> ColNames = {"Nation", "Category", "Entity", "*"};
static const std::unordered_set<std::string> Ops = {"and", "or"};
static const std::unordered_set<std::string> Relations = {"==", "!=", "&=", "$="};

struct ModifyCommandContent {
    CommandType type;
    std::vector<std::string> col_values;
    std::string update_col;
    std::string new_value;
};

struct QueryCommandContent {
    CommandType type;
    std::vector<std::string> cols;
    std::vector<std::string> values;
    std::vector<std::string> relations;
};

class Spliter {
 public:
    virtual bool Split(const std::string& src, std::vector<std::string>& dest, std::string& msg, const std::string& separator="");
};

class InsertSpliter : public Spliter {
 public:
    virtual bool Split(const std::string& src, std::vector<std::string>& dest, std::string& msg, const std::string& separator="") override;
};

class UpdateSpliter : public Spliter {
 public:
    virtual bool Split(const std::string& src, std::vector<std::string>& dest, std::string& msg, const std::string& separator="") override;
};

class DeleteSpliter : public Spliter {
 public:
    virtual bool Split(const std::string& src, std::vector<std::string>& dest, std::string& msg, const std::string& separator="") override;
};

class QuerySpliter : public Spliter {
 public:
    virtual bool Split(const std::string& src, std::vector<std::string>& dest, std::string& msg, const std::string& separator="") override;
};


class ModifyCommandParser {
 private:
    std::shared_ptr<Spliter> spliter_{nullptr};
 public:
    ModifyCommandParser();
    ~ModifyCommandParser();

    bool Parse(const std::string& command, ModifyCommandContent& output, std::string& msg);
};

class QueryCommandParser {
 private:
    std::shared_ptr<Spliter> spliter_{nullptr};
 public:
    QueryCommandParser();
    ~QueryCommandParser();

    bool Parse(const std::string& command, QueryCommandContent& output, std::string& msg);
};

