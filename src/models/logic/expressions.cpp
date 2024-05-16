#include "expressions.hpp"
#include <iostream>

bool ComparisonExpression::eval(std::vector<std::string> &col, std::vector<std::variant<long, std::string>> &row) const
{

    for (int i = 0; i < col.size(); i++)
    {
        if (col[i] == this->columnName)
        {
            bool res;
            if (op == ">")
                res = (row[i] > value);
            else if (op == "<")
                res = (row[i] < value);
            else if (op == "=")
                res = (row[i] == value);
            else if (op == "!=")
                res = (row[i] != value);
            else if (op == ">=")
                res = (row[i] >= value);
            else if (op == "<=")
                res = (row[i] <= value);
            return this->n ? !res : res;
        }
    }
    return false;
}

bool LRExpression::eval(std::vector<std::string> &col, std::vector<std::variant<long, std::string>> &row) const
{

    if (op == "AND")
    {
        bool res = (left->eval(col, row) && right->eval(col, row));
        return this->n ? !res : res;
    }
    else if (op == "OR")
    {
        bool res = left->eval(col, row) || right->eval(col, row);
        return this->n ? !res : res;
    }
}