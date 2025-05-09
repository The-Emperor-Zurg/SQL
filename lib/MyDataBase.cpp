#include "MyDataBase.h"

void MyBD::DownloadFrom(const std::filesystem::path& file) {
    std::ifstream File(file, std::ios::in);

    char c;
    std::string s;
    while (File.get(c)) {
        if (c == '\n') {
            continue;
        } else if (c == ';') {
            RequestParsing(s);
            s = "";
            continue;
        }

        s += c;
    }

    File.close();
}

void MyBD::DownloadTo(const std::filesystem::path& file) {
    std::ofstream File(file, std::ios::out);

    for (auto& [name, table] : all_tables_) {
        File << "CREATE TABLE " + name + " (\n";

        for (auto& [_, column] : table->columns_) {
            File <<  "    " + column->GetStructure() + ",\n";
        }

        for (auto& [column_name, column] : table->columns_) {
            if (!column->GetForeignKey().empty()) {
                File << "    FOREIGN KEY (" + name + ") REFERENCES " + column->GetForeignKey() + ",\n";
            }
        }

        File << ");\n\n";
    }

    for (auto& [name, table] : all_tables_) {
        std::string cur = "INSERT INTO " + name + "(";

        for (auto& [column_name, column] : table->columns_) {
            cur += column_name + ", ";
        }

        cur += ") VALUES (";

        for (size_t i = 0 ; i < table->GetCounter(); ++i) {
            File << cur;

            for (auto& [_, column] : table->columns_) {
                File << column->GetData(i) + ", ";
            }

            File << ");\n";
        }

        File << '\n';
    }

    File.close();
}

void MyBD::RequestParsing(const std::string& request) {
    Request sql(request);

    if (sql.GetTypeRequest() == Request::TypeRequest::Create && all_tables_.contains(sql.GetTableName())) {
        throw "Table already exists";
    }

    if (sql.GetTypeRequest() != Request::TypeRequest::Create && !all_tables_.contains(sql.GetTableName())) {
        throw "No such table";
    }

    if (sql.GetTypeRequest() == Request::TypeRequest::Create) {
        all_tables_[sql.GetTableName()] = std::make_unique<DataTable>(sql, all_tables_);
    } else if (sql.GetTypeRequest() == Request::TypeRequest::Update) {
        all_tables_[sql.GetTableName()]->UpdateRowsValues(sql);
    } else if (sql.GetTypeRequest() == Request::TypeRequest::Drop) {
        all_tables_[sql.GetTableName()].reset();
        all_tables_.erase(sql.GetTableName());
    } else if (sql.GetTypeRequest() == Request::TypeRequest::Insert) {
        all_tables_[sql.GetTableName()]->AddNewRow(sql);
    } else if (sql.GetTypeRequest() == Request::TypeRequest::Delete) {
        all_tables_[sql.GetTableName()]->DeleteRow(sql);
    }
}

Result MyBD::RequestQuery(const std::string& request) {
    Request sql(request);

    if (sql.GetTypeRequest() == Request::TypeRequest::Select) {
        return Result(*all_tables_[sql.GetTableName()], sql);
    } else if (sql.GetTypeRequest() == Request::TypeRequest::Join) {
        return Join(sql);
    }
}

Result MyBD::Join(Request& sql) {
    if (sql.GetTypeRequest() != Request::TypeRequest::Join) {
        return {};
    }

    Result left_table = RequestQuery("SELECT * FROM " + sql.GetTableName());
    Result right_table = RequestQuery("SELECT * FROM " + sql.GetData()["TABLE JOIN"]);

    return Result::Join(left_table, right_table, sql);
}

std::vector<std::string> MyBD::GetTableNames() const {
    std::vector<std::string> table_names;
    table_names.reserve(all_tables_.size());
    for (auto& [table_name, _] : all_tables_) {
        table_names.emplace_back(table_name);
    }

    return table_names;
}