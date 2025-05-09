#include "Cell.h"
#include "Request.h"

class Column {
 public:

    enum DataType {
        Int, Float, Double, Varchar, Bool
    };

    explicit Column(const std::string& name,DataType type){
        column_name = name;
        type_ = type;
    }

    ~Column() = default;

    [[nodiscard]] const std::string& GetName() const;
    [[nodiscard]] std::string GetData(size_t ind) const;
    [[nodiscard]] std::string GetStructure() const;
    [[nodiscard]] size_t GetSize() const;
    [[nodiscard]]DataType GetType() const;

    Cell GetValueFrom(size_t index);
    void AddValue(const std::string& value);
    void AddDefault();
    void SetValue(const std::string& value, size_t index);
    void Check(const std::string& value);
    void DeleteValue(size_t ind);
    void CopyValueFrom(Column* other, size_t index);

    bool Compare(const Column& other, const Request::Condition& operation, size_t ind1, size_t ind2 = INT_MAX);
    bool Compare(const std::string& other, const Request::Condition& operation, size_t ind);

    void SetForeignKey(const std::string& table, Column* tmp);
    [[nodiscard]] std::string GetForeignKey() const;
    void SetPrimaryKeyFlag(bool value);
    [[nodiscard]] bool CheckForeignKey(const Cell& value) const;
    [[nodiscard]] bool CheckPrimaryKey(const std::string& value) const;
    void CheckAvailable(const std::string& value) const;

    friend class ResultSet;

 protected:
    std::string column_name;
    DataType type_;
    std::vector<Cell> values_;
    std::pair<std::string, Column*> foreign_key = {"", nullptr};
    Cell default_ = Cell();
    bool primary_key = false;
};