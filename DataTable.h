#pragma once

#include "Column.h"
#include "Request.h"

#include <map>
#include <vector>
#include <memory>

class DataTable {
 public:
    DataTable(const Request& sql, const std::map<std::string, std::unique_ptr<DataTable>>& other_tables);

    [[nodiscard]] size_t Count() const;

    void InsertRow(const Request& sql);
    void UpdateRow(const Request& sql);
    void DeleteRow(const Request& sql);

    [[nodiscard]] std::vector<std::string> GetColumnNames() const {
        std::vector<std::string> column_names;
        column_names.reserve(columns_.size());

        for (auto& [colum_name, _] : columns_) {
            column_names.push_back(colum_name);
        }

        return column_names;
    }

 protected:
    DataTable() = default;
    DataTable(DataTable& table, const Request& sql);
    static std::vector<std::string> split(const std::string& str, char seperator = ' ');
    std::unordered_map<std::string, std::unique_ptr<GeneralColumn>> columns_;

 private:
    bool CheckCondition(const Request::Condition& condition, size_t index_row);
    bool CheckConditions(const std::vector<Request::Condition>& conditions, size_t index_row);
    void CreateForeignKey(const std::string& column, const std::string& foreign, const std::map<std::string, std::unique_ptr<DataTable>>& other_tables);

    template<typename T>
    void AddColumn(const std::string& name, const GeneralColumn::TypeData& type) {
        columns_[name] = std::make_unique<Column<T>>(name, type);
    }

    void AddColumnByType(const std::string& name, const GeneralColumn::TypeData& type);

    friend class ResultSet;
    friend class DB;
};
