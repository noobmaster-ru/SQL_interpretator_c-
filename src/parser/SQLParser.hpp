#ifndef __SQLPARSER_HPP

#define __SQLPARSER_HPP

#include <iostream>
#include <string>
#include <vector>
#include "../models/logfilter.hpp"
#include "../models/lognode.hpp"
#include "../models/field.hpp"
#include "../models/ret_models.hpp"
#include "../models/update_data.hpp"
#include "../models/values_insert.hpp"

class SQLParser
{
public:
    int currentPosition;
    std::string query;

    SQLParser(const std::string &query) : currentPosition(0), query(query){};
    ParserResult *parse();

private:
    CreateS *parseCreate();
    SelectS *parseSelect();
    InsertS *parseInsert();
    DeleteS *parseDelete();
    DropS *parseDrop();
    UpdateS *parseUpdate();

    bool match(const std::string &exp);
    std::string parseIdentifier();
    LogExpressionNode parseLogicalExpression();
    LogFilter parseComparison();

    void printLogExpression(const std::vector<struct LogExpressionNode> &nodes);

    void skipWhitespace();
};

#endif