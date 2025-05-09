#pragma once

#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

class Request {
 public:
    Request(const std::string& request);

    class Condition {
     public:

        enum Type {
            WHERE, ON, AND, OR
        };

        enum Operation {
            More, MoreOrEquals, Lower, LowerOrEquals, Equals, NotEquals, IsNull, IsNotNull
        };

        template<typename T>
        bool ConditionCompare(const T& lhs, const T& rhs) const {
            if (operation_ == More) {
                return lhs > rhs;
            } else if (operation_ == MoreOrEquals) {
                return lhs >= rhs;
            } else if (operation_ == Lower) {
                return lhs < rhs;
            } else if (operation_ == LowerOrEquals) {
                return lhs <= rhs;
            } else if (operation_ == Equals) {
                return lhs == rhs;
            } else if (operation_ == NotEquals) {
                return lhs != rhs;
            } else if (operation_ == IsNull) {
                return false;
            } else {
                return true;
            }
        }

        void SetCondition(const std::string& condition);
        void SetOperation(const std::string& operation);
        void SetLhs(const std::string& lhs);
        void SetRhs(const std::string& rhs);

        [[nodiscard]] Type GetCondition() const;
        [[nodiscard]] Operation GetOperation() const;
        [[nodiscard]] std::string GetLhs() const;
        [[nodiscard]] std::string GetRhs() const;

     private:
        Type condition_;
        Operation operation_;
        std::string lhs_;
        std::string rhs_;
    };

    enum TypeRequest {
        Create, Drop, Select, Insert, Update, Delete, None, Join
    };

    [[nodiscard]] TypeRequest GetTypeRequest() const;
    [[nodiscard]] std::unordered_map<std::string, std::string> GetData() const;
    [[nodiscard]] std::vector<Condition> GetConditions() const;
    [[nodiscard]] std::string GetTableName() const;

    void SetCondition(const std::string& condition);
    static void SetColumnsValues(const std::string& str, std::vector<std::string>& mas);
    void SetType(const std::string& s);

    bool SelectHandler(const std::string& request);
    bool DeleteHandler(const std::string& request);
    bool InsertHandler(const std::string& request);
    bool UpdateHandler(const std::string& request);
    bool CreateHandler(const std::string& request);
    bool DropHandler(const std::string& request);
    bool JoinHandler(const std::string& request);

    friend class DataTable;

 private:
    TypeRequest type_ = TypeRequest::None;
    std::string name_table_;

    std::vector<Condition> conditions_;
    std::unordered_map<std::string, std::string> columns_values_;
};