#pragma once

#include <map>
#include <sstream>

#include "Column.h"

class DataTable {
 public:
    explicit DataTable(const Request& request, const std::map<std::string, std::unique_ptr<DataTable>>& datatables);

    [[nodiscard]] size_t GetCounter() const;
    void AddNewRow(const Request& sql);
    void DeleteRow(const Request& request);
    void UpdateRowsValues(const Request& request);
    [[nodiscard]] std::vector<std::string> GetColumnNames() const;

    friend class Result;
    friend class MyBD;

 protected:
    std::unordered_map<std::string, std::unique_ptr<Column>> columns_;
    DataTable() = default;
    DataTable(DataTable& table, const Request& sql);
    static std::vector<std::string> Split(const std::string& s, char c);

 private:
    bool CheckConditions(const std::vector<Request::Condition>& conditions, size_t ind_row);
    bool CheckOneCondition(const Request::Condition& condition, size_t ind_row);
    void CreateForeignKey(const std::string& column, const std::string& foreign, const std::map<std::string, std::unique_ptr<DataTable>>& other_tables);
    void AddColumn(const std::string& name, const Column::DataType& type);
    Column::DataType GetType(const std::string& type);
};