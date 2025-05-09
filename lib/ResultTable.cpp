#include "ResultTable.h"

bool Result::Next() {
    return ++current_ < GetCounter();
}

void Result::Reset() {
    current_ = 0;
}

Cell Result::Get(const std::string& name_column) {
    if (current_ == SIZE_MAX) {
        ++current_;
    }

    if (current_ >= GetCounter()) {
        return {};
    }

    for (const auto& [name, column] : columns_) {
        if (name == name_column) {
            return column.get()->GetValueFrom(current_);
        }
    }

    return {};
}

Result Result::Join(DataTable& table1, DataTable& table2, const Request& sql) {
    std::unordered_map<std::string, std::string> rows = sql.GetData();
    std::string left_name = sql.GetTableName() + '.';
    std::string right_name = rows["TABLE JOIN"] + '.';
    DataTable result;
    std::string table_column_name;
    for (auto& [name, column] : table1.columns_) {
        table_column_name = left_name + name;
        if (rows.contains(table_column_name) || rows.contains("*")) {
            result.AddColumn(table_column_name, column->GetType());
        }
    }

    for (auto& [name, column] : table2.columns_) {
        table_column_name = right_name + name;
        if (rows.contains(table_column_name) || rows.contains("*")) {
            result.AddColumn(table_column_name, column->GetType());
        }
    }

    DataTable* left_table = &table1;
    DataTable* right_table = &table2;

    if (rows["TYPE JOIN"] == "RIGHT") {
        std::swap(left_table, right_table);
        std::swap(left_name, right_name);
    }

    bool Inner;
    if (rows["TYPE JOIN"] == "INNER") {
        Inner = true;
    } else {
        Inner = false;
    }

    std::vector<std::pair<Column*, Column*>> ON_conditions;
    ON_conditions.reserve(sql.GetConditions().size());
    for (auto& condition : sql.GetConditions()) {
        if (condition.GetCondition() == Request::Condition::Type::WHERE) {
            break;
        }

        std::string left_column = Split(condition.GetLhs(), '.')[1];
        std::string right_column = Split(condition.GetRhs(), '.')[1];

        if (left_table->columns_.contains(left_column) && right_table->columns_.contains(right_column)) {
            ON_conditions.emplace_back(left_table->columns_[left_column].get(),
                                       right_table->columns_[right_column].get());
        } else if (right_table->columns_.contains(left_column) && left_table->columns_.contains(right_column)) {
            ON_conditions.emplace_back(left_table->columns_[right_column].get(),
                                       right_table->columns_[left_column].get());
        } else {
            throw "Incorrect JOIN condition";
        }
    }

    Request::Condition equals_objects{};
    equals_objects.SetOperation("=");
    for (size_t i = 0; i < left_table->GetCounter(); ++i) {
        bool joined_flag = false;
        for (auto& [column1, column2] : ON_conditions) {
            for (size_t j = 0; j < right_table->GetCounter(); ++j) {
                bool equals = true;
                for (auto& [lhs, rhs] : ON_conditions) {
                    if (!lhs->Compare(*rhs, equals_objects, i, j)) {
                        equals = false;
                        break;
                    }
                }

                if (equals) {
                    joined_flag = true;
                    for (auto& [name, column]: left_table->columns_) {
                        if (result.columns_.contains(left_name + name)) {
                            result.columns_[left_name + name]->CopyValueFrom(column.get(), i);
                        }
                    }

                    for (auto& [name, column]: right_table->columns_) {
                        if (result.columns_.contains(right_name + name)) {
                            result.columns_[right_name + name]->CopyValueFrom(column.get(), j);
                        }
                    }
                }
            }
        }

        if (!joined_flag && !Inner) {
            for (auto& [name, column]: left_table->columns_) {
                if (result.columns_.contains(left_name + name)) {
                    result.columns_[left_name + name]->CopyValueFrom(column.get(), i);
                }
            }

            for (auto& [name, _]: right_table->columns_) {
                result.columns_[right_name + name]->AddValue("NULL");
            }
        }
    }

    return Result(result, sql);
}