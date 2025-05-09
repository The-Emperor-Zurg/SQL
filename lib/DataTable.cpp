#include "DataTable.h"

DataTable::DataTable(const Request& request, const std::map<std::string, std::unique_ptr<DataTable>>& datatables) {
    if (request.type_ != Request::TypeRequest::Create) {
        return;
    }

    for (auto& x : request.GetData()) {
        std::vector<std::string> param = Split(x.second, ' ');
        AddColumn(x.first, GetType(param[0]));
        if (std::find(param.begin(), param.end(), "PRIMARY") != param.end()) {
            columns_[x.first]->SetPrimaryKeyFlag(true);
        }
    }

    for (auto& x : request.GetData()) {
        if (x.first[0] == 'F' && x.first[1] == ' ') {
            CreateForeignKey(x.first.substr(2), x.second, datatables);
            continue;
        }
    }
}

size_t DataTable::GetCounter() const {
    return columns_.begin()->second->GetSize();
}

void DataTable::AddNewRow(const Request& sql) {
    if (sql.type_ != Request::TypeRequest::Insert) {
        return;
    }

    auto tmp = sql.GetData();
    for (auto& x : columns_) {
        if (tmp.contains(x.second->GetName())) {
            x.second->AddValue(tmp[x.second->GetName()]);
        } else {
            x.second->AddDefault();
        }
    }
}

void DataTable::DeleteRow(const Request& request) {
    if (request.type_ != Request::TypeRequest::Delete) {
        return;
    }

    std::vector<Request::Condition> conditions = request.GetConditions();
    for (int i = GetCounter() - 1; i >= 0; --i) {
        if (CheckConditions(conditions, i)) {
            for (auto& x: columns_) {
                x.second->DeleteValue(i);
            }
        }
    }
}

void DataTable::UpdateRowsValues(const Request& request) {
    if (request.type_ != Request::TypeRequest::Update) {
        return;
    }

    std::vector<Request::Condition> conditions = request.GetConditions();
    for (size_t i = 0; i < GetCounter(); ++i) {
        if (CheckConditions(conditions, i)) {
            for (auto& x: request.columns_values_) {
                if (!columns_.contains(x.first)) {
                    throw "The update request is incorrect";
                }

                columns_[x.first]->SetValue(x.second, i);
            }
        }
    }
}

std::vector<std::string> DataTable::GetColumnNames() const {
    std::vector<std::string> names;
    for (auto& x : columns_) {
        names.emplace_back(x.first);
    }

    return names;
}

DataTable::DataTable(DataTable& table, const Request& sql) {
    if (sql.GetTypeRequest() != Request::TypeRequest::Select && sql.GetTypeRequest() != Request::TypeRequest::Join) {
        return;
    }

    std::unordered_map<std::string, std::string> row = sql.GetData();
    for (auto& x : table.columns_) {
        if (row.contains(x.second->GetName()) || row.contains("*")) {
            AddColumn(x.second->GetName(), x.second->GetType());
        }
    }

    std::vector<Request::Condition> conditions = sql.GetConditions();
    for (size_t i = 0; i < table.GetCounter(); ++i) {
        if (table.CheckConditions(conditions, i)) {
            for (auto& x: columns_) {
                x.second->CopyValueFrom(table.columns_[x.first].get(), i);
            }
        }
    }
}

std::vector<std::string> DataTable::Split(const std::string& s, char c) {
    std::vector<std::string> res;
    std::stringstream ss(s);
    std::string s1;
    while (std::getline(ss, s1, c)) {
        res.emplace_back(s1);
    }

    return res;
}

bool DataTable::CheckConditions(const std::vector<Request::Condition>& conditions, size_t ind_row) {
    if (conditions.empty()) {
        return true;
    }

    size_t index = 0;
    while (index < conditions.size() && conditions[index].GetCondition() != Request::Condition::Type::WHERE) {
        index++;
    }

    if (index >= conditions.size()) {
        return true;
    }

    bool res = CheckOneCondition(conditions[index], ind_row);
    index++;
    for (; index < conditions.size(); ++index) {
        bool tmp = CheckOneCondition(conditions[index], ind_row);
        if (conditions[index].GetCondition() == Request::Condition::AND) {
            res = res && tmp;
        } else if (conditions[index].GetCondition() == Request::Condition::OR) {
            res = res || tmp;
        } else if (conditions[index].GetCondition() == Request::Condition::ON) {
            throw "Incorrect operand condition";
        }
    }

    return res;
}

bool DataTable::CheckOneCondition(const Request::Condition& condition, size_t ind_row) {
    if (columns_.contains(condition.GetLhs()) && columns_.contains(condition.GetRhs())) {
        return columns_[condition.GetRhs()]->Compare(*columns_[condition.GetLhs()], condition, ind_row);
    } else if (columns_.contains(condition.GetLhs())) {
        return columns_[condition.GetLhs()]->Compare(condition.GetRhs(), condition, ind_row);
    } else if (columns_.contains(condition.GetRhs())) {
        return columns_[condition.GetRhs()]->Compare(condition.GetLhs(), condition, ind_row);
    }

    return true;
}

void DataTable::CreateForeignKey(const std::string& column, const std::string& foreign, const std::map<std::string, std::unique_ptr<DataTable>>& other_tables) {
    std::vector<std::string> parameters = Split(foreign, ' ');
    columns_[column].get()->SetForeignKey(parameters[0], other_tables.at(parameters[0])->columns_.at(parameters[1]).get());
}

void DataTable::AddColumn(const std::string& name, const Column::DataType& type) {
    columns_[name] = std::make_unique<Column>(name, type);
}

Column::DataType DataTable::GetType(const std::string& type) {
    if (type == "INT") {
        return Column::DataType::Int;
    } else if (type == "VARCHAR") {
        return  Column::DataType::Varchar;
    } else if (type == "BOOL") {
        return Column::DataType::Bool;
    } else if (type == "DOUBLE") {
        return Column::DataType::Double;
    } else if (type == "FLOAT") {
        return Column::DataType::Float;
    }
}