#pragma once

#include "DataTable.h"

class Result : public DataTable {
 public:

    Result() = default;
    explicit Result(DataTable& table, const Request& sql) : DataTable(table, sql) {}


    bool Next();
    void Reset();
    Cell Get(const std::string& name_column);
    static Result Join(DataTable& table1, DataTable& table2, const Request& sql);
 private:
    size_t current_ = SIZE_MAX;
};