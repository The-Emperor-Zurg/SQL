#pragma once

#include <iostream>

class Cell {
 public:
    Cell() = default;

    Cell(std::string new_value_) {
        value_ = new_value_;
        is_null_ = false;
    }

    bool operator==(const Cell& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Cell& elem);

    bool IsNull() const;
    std::string GetVal() const;
    int GetIntVal() const;
    float GetFloatVal() const;
    double GetDoubleVal() const;
    bool GetBoolVal() const;
    std::string GetStringVal() const;

    void SetVal(const std::string& s);

 private:
    std::string value_;
    bool is_null_ = true;
};