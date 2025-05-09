#pragma once

#include <fstream>

#include "ResultTable.h"

class MyBD {
 public:
    void DownloadFrom(const std::filesystem::path& file);
    void DownloadTo(const std::filesystem::path& file);
    void RequestParsing(const std::string& request);
    Result RequestQuery(const std::string& request);

    [[nodiscard]] std::vector<std::string> GetTableNames() const;

 private:
    Result Join(Request& sql);
    std::map<std::string, std::unique_ptr<DataTable>> all_tables_;

    friend class DataTable;
};