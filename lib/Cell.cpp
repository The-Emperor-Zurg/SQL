#include "Cell.h"

bool Cell::operator==(const Cell& other) const {
    return other.value_ == value_ && other.is_null_ == is_null_;
}

std::ostream& operator<<(std::ostream& os, const Cell& elem) {
    if (!elem.is_null_) {
        os << elem.value_;
    } else {
        os << "NULL";
    }

    return os;
}

bool Cell::IsNull() const {
    return is_null_;
}

std::string Cell::GetVal() const {
    return value_;
}

int Cell::GetIntVal() const {
    return std::strtol(value_.c_str(), nullptr, 10);
}

float Cell::GetFloatVal() const {
    return std::stof(value_);
}

double Cell::GetDoubleVal() const {
    return std::stod(value_);
}

bool Cell::GetBoolVal() const {
    if (value_ == "0" || value_ == "false") {
        return false;
    } else if (value_ == "1" || value_ == "true") {
        return true;
    }

    throw "The value is bad boolean";
}

std::string Cell::GetStringVal() const {
    return value_.substr(1, value_.length() - 2);
}

void Cell::SetVal(const std::string& s) {
    if (s == "NULL") {
        is_null_ = true;
    } else {
        value_ = s;
        is_null_ = false;
    }
}
