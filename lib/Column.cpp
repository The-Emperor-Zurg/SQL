#include "Column.h"

const std::string& Column::GetName() const {
    return column_name;
}

std::string Column::GetData(size_t ind) const {
    if (values_[ind].IsNull()) {
        return "NULL";
    }

    return values_[ind].GetVal();
}

std::string Column::GetStructure() const {
    std::string res = column_name;
    if (type_ == Int) {
        res += " INT";
    } else if (type_ == Bool){
        res += " BOOL";
    } else if(type_ == Double){
        res += " DOUBLE";
    } else if(type_ == Float){
        res += " FLOAT";
    } else if (type_ == Varchar){
        res += " VARCHAR";
    }

    if (primary_key) {
        res += " PRIMARY KEY";
    }
    return res;
}

size_t Column::GetSize() const {
    return values_.size();
}

Column::DataType Column::GetType() const {
    return type_;
}

Cell Column::GetValueFrom(size_t index) {
    return values_[index];
}

void Column::AddValue(const std::string& value) {
    if (value == "NULL") {
        values_.emplace_back(default_);
    } else {
        Check(value);
        values_.emplace_back(value);
    }
}

void Column::AddDefault() {
    values_.emplace_back(default_);
}

void Column::SetValue(const std::string& value, size_t ind) {
    Check(value);
    values_[ind].SetVal(value);
}

void Column::Check(const std::string& value) {
    CheckAvailable(value);
    if (type_ == Varchar){
        if (!(value.front() == '\'' && value.back() == '\'')) {
            throw "Varchar type is incorrect";
        }
    } else if (type_ == Bool){
        if (value != "true" && value != "false" && value != "0" && value != "1") {
            throw "Bool type is incorrect";
        }
    }
}

void Column::CheckAvailable(const std::string& value) const {
    if (!CheckForeignKey(Cell(value)) || !CheckPrimaryKey(value)) {
        if(!CheckForeignKey(Cell(value))){
            throw std::invalid_argument("Incorrect foreign key");
        } else {
            throw std::invalid_argument("Incorrect primary key");
        }
    }
}

bool Column::CheckForeignKey(const Cell& value) const {
    if (foreign_key.second == nullptr) {
        return true;
    }

    for (auto tmp = foreign_key.second->values_.begin(); tmp != foreign_key.second->values_.end(); ++tmp) {
        if (tmp->GetVal() == value) {
            return true;
        }
    }

    return false;
}

bool Column::CheckPrimaryKey(const std::string& value) const {
    if (!primary_key) {
        return true;
    }

    for (auto tmp = values_.begin(); tmp != values_.end(); ++tmp) {
        if (value == tmp->GetVal()) {
            return true;
        }
    }

    return false;
}

void Column::DeleteValue(size_t index) {
    values_.erase(values_.cbegin() + index);
}

void Column::CopyValueFrom(Column* other, size_t index) {
    values_.emplace_back(other->values_[index]);
}

bool Column::Compare(const Column& other, const Request::Condition& operation, size_t ind1, size_t ind2) {
    if (ind2 == INT_MAX) {
        ind2 = ind1;
    }

    if (type_ == Int) {
        return operation.ConditionCompare(values_[ind1].GetIntVal(), other.values_[ind2].GetIntVal());
    } else if (type_ == Float) {
        return operation.ConditionCompare(values_[ind1].GetFloatVal(), other.values_[ind2].GetFloatVal());
    } else if (type_ == Double) {
        return operation.ConditionCompare(values_[ind1].GetDoubleVal(), other.values_[ind2].GetDoubleVal());
    } else if (type_ == Varchar) {
        return operation.ConditionCompare(values_[ind1].GetStringVal(), other.values_[ind2].GetStringVal());
    } else if (type_ == Bool) {
        return operation.ConditionCompare(values_[ind1].GetBoolVal(), other.values_[ind2].GetBoolVal());
    }

    return false;
}

bool Column::Compare(const std::string& other, const Request::Condition &operation, size_t ind) {
    Cell tmp = values_[ind];

    if (operation.GetOperation() == Request::Condition::IsNotNull) {
        return !tmp.IsNull();
    } else if (operation.GetOperation() == Request::Condition::IsNull) {
        return tmp.IsNull();
    } else if (tmp.IsNull()) {
        return false;
    }

    Cell tmp1(other);
    if (type_ == Int){
        return operation.ConditionCompare(tmp.GetIntVal(), tmp1.GetIntVal());
    } else if (type_ == Double){
        return operation.ConditionCompare(tmp.GetDoubleVal(), tmp1.GetDoubleVal());
    } else if (type_ == Bool){
        return operation.ConditionCompare(tmp.GetBoolVal(), tmp1.GetBoolVal());
    } else if (type_ == Float){
        return operation.ConditionCompare(tmp.GetFloatVal(), tmp1.GetFloatVal());
    } else if (type_ == Varchar){
        return operation.ConditionCompare(tmp.GetStringVal(), tmp1.GetStringVal());
    }

    return false;
}

void Column::SetForeignKey(const std::string& table, Column* tmp) {
    foreign_key = {table, tmp};
}

std::string Column::GetForeignKey() const {
    if (!foreign_key.second) {
        return "";
    }

    return foreign_key.first + "(" + foreign_key.second->column_name + ")";
}

void Column::SetPrimaryKeyFlag(bool value) {
    primary_key = value;
}