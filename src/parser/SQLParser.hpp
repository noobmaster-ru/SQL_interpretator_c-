#ifndef __SQLPARSER_HPP

#define __SQLPARSER_HPP

#include <iostream>
#include <string>
#include <vector>
#include "../models/logfilter.hpp"
#include "../models/lognode.hpp"
#include "../models/field.hpp"
#include "../models/ret_models.hpp"

class SQLParser
{
public:
    int currentPosition;
    std::string query;

    SQLParser(const std::string &query) : currentPosition(0), query(query){};
    ParserResult parse();

private:
    CreateS *parseCreate();
    SelectS *parseSelect();
    // void parseInsert();
    // void parseDelete();
    // void parseDrop();
    // void parseUpdate();

    bool match(const std::string &exp);
    std::string parseIdentifier();
    LogExpressionNode parseLogicalExpression();
    LogFilter parseComparison();

    void printLogExpression(const std::vector<struct LogExpressionNode> &nodes);

    void skipWhitespace();
};

#endif