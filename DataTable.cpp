#include "DataTable.h"

DataTable::DataTable(const Request &sql, const std::map<std::string, std::unique_ptr<DataTable>>& other_tables) {
    if (sql.GetTypeRequest() != Request::TypeRequest::None) {
        return;
    }

    for (auto x : sql.GetData()) {
        auto param = split(x.second);
        std::string col_type = param[0];

        if (col_type == "INT") {
            AddColumnByType(x.first, GeneralColumn::TypeData::Int);
        } else if (col_type == "BOOL") {
            AddColumnByType(x.first, GeneralColumn::TypeData::Bool);
        } else if (col_type == "VARCHAR") {
            AddColumnByType(x.first, GeneralColumn::TypeData::Varchar);
        } else if (col_type == "DOUBLE") {
            AddColumnByType(x.first, GeneralColumn::TypeData::Double);
        } else if (col_type == "FLOAT") {
            AddColumnByType(x.first, GeneralColumn::TypeData::Float);
        }

        if (std::find(param.begin(), param.end(), "PRIMARY") != param.end()) {
            columns_[x.first]->SetPrimaryKeyFlag(true);
        }

    }

    for (auto x : sql.GetData()) {
        if (x.first[0] == 'F' && x.first[1] == ' ') {
            CreateForeignKey(x.first.substr(2), x.second, other_tables);
            continue;
        }
    }
}

size_t DataTable::Count() const {
    return columns_.begin()->second->Size();
}

void DataTable::UpdateRow(const Request& sql) {
    if (sql.GetTypeRequest() != Request::TypeRequest::Update) {
        return;
    }

    auto conditions = sql.GetConditions();
    for (size_t i = 0; i < Count(); ++i) {
        if (CheckConditions(conditions, i)) {

            for (auto& [column, value]: sql.GetData()) {
                if (!columns_.contains(column)) {
                    throw "Bad update construction";
                }

                columns_[column]->SetData(value, i);
            }

        }
    }
}

void DataTable::DeleteRow(const Request& sql) {
    if (sql.GetTypeRequest() != Request::TypeRequest::Delete) {
        return;
    }

    auto conditions = sql.GetConditions();
    for (long long i = Count() - 1; i >= 0; --i) {
        if (CheckConditions(conditions, i)) {

            for (auto& x : columns_) {
                x.second->DeleteData(i);
            }

        }
    }
}


DataTable::DataTable(DataTable& table, const Request& sql) {
    if (sql.GetTypeRequest() != Request::TypeRequest::Join && sql.GetTypeRequest() != Request::TypeRequest::Select) {
        return;
    }

    auto row = sql.GetData();
    for (const auto& x : table.columns_) {
        if (row.contains(x.second->GetColumnName()) || row.contains("*")) {
            AddColumnByType(x.second->GetColumnName(), x.second->Type());
        }
    }

    auto conditions = sql.GetConditions();
    for (size_t i = 0; i < table.Count(); ++i) {
        if (table.CheckConditions(conditions, i)) {

            for (auto& x: columns_) {
                x.second->CopyDataFrom(table.columns_[x.first].get(), i);
            }

        }
    }
}

std::vector<std::string> DataTable::split(const std::string& str, char seperator) {
    std::vector<std::string> words;
    std::stringstream ss(str);
    std::string cur;

    while (std::getline(ss, cur, seperator)) {
        words.emplace_back(cur);
    }

    return words;
}

bool DataTable::CheckConditions(const std::vector<Request::Condition>& conditions, size_t index_row) {
    if (conditions.empty()) {
        return true;
    }

    size_t index_condition = 0;
    while (index_condition < conditions.size() &&
        conditions[index_condition].GetCondition() != Request::Condition::Type::WHERE) {
        ++index_condition;
    }

    if (index_condition >= conditions.size()) {
        return true;
    }

    bool res = CheckCondition(conditions[index_condition], index_row);
    ++index_condition;
    for (; index_condition < conditions.size(); ++index_condition) {
        bool cur = CheckCondition(conditions[index_condition], index_row);
        if (conditions[index_condition].GetCondition() == Request::Condition::AND) {
            res = res && cur;
        } else if (conditions[index_condition].GetCondition() == Request::Condition::OR) {
            res = res || cur;
        } else if (conditions[index_condition].GetCondition() == Request::Condition::WHERE || conditions[index_condition].GetCondition() == Request::Condition::ON) {
            throw "The operand is bad";
        }
    }

    return res;
}

bool DataTable::CheckCondition(const Request::Condition& condition, size_t index_row) {
    if (columns_.contains(condition.GetLhs()) && columns_.contains(condition.GetRhs())) {
        return columns_[condition.GetRhs()]->Compare(*columns_[condition.GetLhs()], condition, index_row);
    } else if (columns_.contains(condition.GetLhs())) {
        return columns_[condition.GetLhs()]->Compare(condition.GetRhs(), condition, index_row);
    } else if (columns_.contains(condition.GetRhs())) {
        return columns_[condition.GetRhs()]->Compare(condition.GetLhs(), condition, index_row);
    }

    return true;
}

void DataTable::CreateForeignKey(const std::string& column, const std::string& foreign,
                             const std::map<std::string, std::unique_ptr<DataTable>>& other_tables) {
    auto param = split(foreign);
    columns_[column].get()->SetForeignKey(param[0], other_tables.at(param[0])->columns_.at(param[1]).get());
}

void DataTable::AddColumnByType(const std::string& name, const GeneralColumn::TypeData& type) {
    if (GeneralColumn::TypeData::Int == type) {
        AddColumn<int>(name, type);
    } else if (GeneralColumn::TypeData::Varchar == type) {
        AddColumn<std::string>(name, type);
    } else if (GeneralColumn::TypeData::Double == type) {
        AddColumn<double>(name, type);
    } else if (GeneralColumn::TypeData::Float == type) {
        AddColumn<float>(name, type);
    } else if (GeneralColumn::TypeData::Bool == type) {
        AddColumn<bool>(name, type);
    }
}