#include "Request.h"

Request::Request(const std::string& request) {
    SetType(request);

    if (type_ == TypeRequest::None) {
        throw "The request is bad";
    }
}

void Request::Condition::SetCondition(const std::string& condition) {
    if (condition == "AND") {
        condition_ = AND;
    } else if (condition == "OR") {
        condition_ = OR;
    } else if (condition == "ON") {
        condition_ = ON;
    } else if (condition == "WHERE") {
        condition_ = WHERE;
    } else {
        throw "The condition is bad";
    }
}

void Request::Condition::SetOperation(const std::string& operation) {
    if (operation == ">") {
        operation_ = More;
    } else if (operation == ">=") {
        operation_ = MoreOrEquals;
    } else if (operation == "<") {
        operation_ = Lower;
    } else if (operation == "<=") {
        operation_ = LowerOrEquals;
    } else if (operation == "=" || operation == "IS") {
        operation_ = Equals;
    } else if (operation == "!=") {
        operation_ = NotEquals;
    }
}

void Request::Condition::SetLhs(const std::string& lhs) {
    lhs_ = lhs;
}

void Request::Condition::SetRhs(const std::string& rhs) {
    if (rhs == "NULL" || rhs == "NOT NULL") {
        if (operation_ != Equals && operation_ != IsNull && operation_ != IsNotNull) {
            throw "The condition is bad";
        }

        if (rhs == "NULL") {
            operation_ = IsNull;
        } else {
            operation_ = IsNotNull;
        }
    }

    rhs_ = rhs;
}

[[nodiscard]] Request::Condition::Type Request::Condition::GetCondition() const {
    return condition_;
}

[[nodiscard]] Request::Condition::Operation Request::Condition::GetOperation() const {
    return operation_;
}

[[nodiscard]] std::string Request::Condition::GetLhs() const {
    return lhs_;
}

[[nodiscard]] std::string Request::Condition::GetRhs() const {
    return rhs_;
}

[[nodiscard]] Request::TypeRequest Request::GetTypeRequest() const {
    return type_;
}

[[nodiscard]] std::unordered_map<std::string, std::string> Request::GetData() const {
    return columns_values_;
}

[[nodiscard]] std::vector<Request::Condition> Request::GetConditions() const {
    return conditions_;
}

[[nodiscard]] std::string Request::GetTableName() const {
    return name_table_;
}

void Request::SetCondition(const std::string& condition) {
    if (condition.length() == 0) {
        return;
    }

    std::regex Regex(R"((WHERE|AND|OR|ON)\s+([^\s=]+)\s*([(?:IS)<>!=]+)\s*((?:NOT NULL|NULL)|(?:'[^']*')|(?:[^,\s]+)+))");
    std::smatch result;
    std::string::const_iterator search(condition.cbegin());
    while(std::regex_search(search, condition.cend(), result, Regex)) {
        if (result.size() >= 5) {
            Condition tmp;
            tmp.SetCondition(result[1].str());
            tmp.SetLhs(result[2].str());
            tmp.SetOperation(result[3].str());
            tmp.SetRhs(result[4].str());

            conditions_.emplace_back(tmp);
        } else {
            throw "The condition is bad!";
        }

        search = result.suffix().first;
    }

    if (conditions_.empty()) {
        throw "The conditions are empty!";
    }
}

void Request::SetColumnsValues(const std::string& s, std::vector<std::string>& mas) {
    std::regex reg(R"(('[^']*')|([^,\s]+))");
    std::sregex_iterator reg_end;

    for (std::sregex_iterator reg_iter(s.begin(), s.end(), reg); reg_iter != reg_end; ++reg_iter) {
        mas.emplace_back(reg_iter->str());
    }
}

void Request::SetType(const std::string& s) {
    if (SelectHandler(s)) {
        type_ = Select;
    } else if (InsertHandler(s)) {
        type_ = Insert;
    } else if (DeleteHandler(s)) {
        type_ = Delete;
    } else if (UpdateHandler(s)) {
        type_ = Update;
    } else if (CreateHandler(s)) {
        type_ = Create;
    } else if (DropHandler(s)) {
        type_ = Drop;
    } else if (JoinHandler(s)) {
        type_ = Join;
    }
}

bool Request::SelectHandler(const std::string& request) {
    std::regex reg(R"(SELECT\s+(.+)\s+FROM\s+([^\s]+)\s*(?:(WHERE\s+[^;]+))?[\s;]*)");
    std::smatch result;

    std::string columns_str;
    if (std::regex_match(request, result, reg) && result.size() >= 3) {
        columns_str = result[1].str();
        if (columns_str == "*") {
            columns_values_["*"] = "";
        } else {
            std::vector<std::string> columns;
            SetColumnsValues(columns_str, columns);

            for (auto& column : columns) {
                columns_values_[column] = "";
            }
        }

        name_table_ = result[2].str();
        if (result.size() >= 4) {
            SetCondition(result[3].str());
        }

        return true;
    }

    return false;
}

bool Request::DeleteHandler(const std::string& request)  {
    std::regex reg(R"(DELETE\s+FROM\s+([^\s]+)\s*(?:(WHERE\s+[^;]+))?;*)");
    std::smatch result;

    if (std::regex_match(request, result, reg) && result.size() >= 2) {
        name_table_ = result[1].str();
        if (result.size() >= 3) {
            SetCondition(result[2].str());
        }

        return true;
    }

    return false;
}

bool Request::InsertHandler(const std::string& request) {
    std::regex reg(R"(INSERT\s+INTO\s+([^\s]+)\s*\(([A-Za-z0-9-_,\s]+)\)\s*VALUES\s*\((['"A-Za-z0-9-_,\s\.]+)\)[\s;]*)");
    std::smatch result;

    if (std::regex_match(request, result, reg) && result.size() >= 4) {
        name_table_ = result[1].str();
        std::vector<std::string> columns;
        std::vector<std::string> values;
        SetColumnsValues(result[2].str(), columns);
        SetColumnsValues(result[3].str(), values);
        if (columns.size() != values.size()) {
            throw "The insert is wrong!";
        }

        for (size_t i = 0; i < columns.size(); ++i) {
            columns_values_[columns[i]] = values[i];
        }

        return true;
    }

    return false;
}

bool Request::UpdateHandler(const std::string& request)  {
    std::regex reg(R"(UPDATE\s+([^\s]+)\s+SET\s+([^(?:WHERE)]+)\s*(?:(WHERE\s+[^;]+))?[\s;]*)");
    std::smatch result;

    if (std::regex_match(request, result, reg) && result.size() >= 3) {
        name_table_ = result[1].str();
        std::string columns_str = result[2].str();
        std::regex columnsRegex(R"(('[^']*')|[^\s=,]+)");
        std::sregex_iterator columnsIter(columns_str.begin(), columns_str.end(), columnsRegex);
        std::sregex_iterator columnsEnd;
        std::string column;
        for (; columnsIter != columnsEnd; ++columnsIter) {
            std::string element_str = columnsIter->str();
            if (column.empty()) {
                column = element_str;
            } else {
                columns_values_[column] = element_str;
                column = "";
            }
        }

        if (result.size() >= 4) {
            SetCondition(result[3].str());
        }

        return true;
    }

    return false;
}

bool Request::CreateHandler(const std::string& request) {
    std::regex reg(R"(CREATE\s+TABLE\s+([^\s]+)\s+\(([\sA-Za-z0-9-_,\(\)]*)\)[\s;]*)");
    std::smatch result;

    if (std::regex_match(request, result, reg) && result.size() >= 2) {
        name_table_ = result[1].str();
        std::string table_struct = result[2].str();
        std::regex table_struct_regex(R"(([^\s,\(]+)\s+([^\s,]+(?:\s+(?:PRIMARY KEY))*))");
        std::string::const_iterator start_search(table_struct.cbegin());
        while (std::regex_search(start_search, table_struct.cend(), result, table_struct_regex)) {
            if (result.size() >= 3 && result[2].str() != "KEY" && result[2].str() != "REFERENCES") {
                columns_values_[result[1].str()] = result[2].str();
            }

            start_search = result.suffix().first;
        }

        if (columns_values_.empty()) {
            return false;
        }

        std::regex foreign_key_regex(R"(FOREIGN\s+KEY\s+\(([^\s]+)\)\s+REFERENCES\s+([^\s\)]+)\(([^\s,]+)\))");
        start_search = table_struct.cbegin();
        while (std::regex_search(start_search, table_struct.cend(), result, foreign_key_regex)) {
            if (result.size() >= 4) {
                columns_values_["F " + result[1].str()] = result[2].str() + " " + result[3].str();
                if (!columns_values_.contains(result[1].str())) {
                    return false;
                }
            }

            start_search = result.suffix().first;
        }

        return true;
    }

    return false;
}

bool Request::DropHandler(const std::string& request) {
    std::regex reg(R"(DROP\s+TABLE\s+([^\s;]+)[\s;]*)");
    std::smatch result;

    if (std::regex_match(request, result, reg) && result.size() >= 2) {
        name_table_ = result[1].str();
        return true;
    }

    return false;
}

bool Request::JoinHandler(const std::string& request) {
    std::regex reg(R"(SELECT\s+(.+)\s+FROM\s+([^\s]+)\s+(LEFT|RIGHT|INNER)?\s+JOIN\s+([^\s]+)\s+(ON\s+[^(?:WHERE)]+)\s*(?:(WHERE\s+[^;]+))?[\s;]*)");
    std::smatch result;

    if (std::regex_match(request, result, reg) && result.size() >= 6) {
        std::string columns_str = result[1].str();
        if (columns_str == "*") {
            columns_values_["*"] = "";
        } else {
            std::vector<std::string> columns;
            SetColumnsValues(columns_str, columns);

            for (auto& column : columns) {
                columns_values_[column] = "";
            }
        }

        name_table_ = result[2].str();

        columns_values_["TYPE JOIN"]  = result[3].str();
        columns_values_["TABLE JOIN"] = result[4].str();
        SetCondition(result[5].str());

        if (result.size() >= 7) {
            SetCondition(result[6].str());
        }

        return true;
    }

    return false;
}