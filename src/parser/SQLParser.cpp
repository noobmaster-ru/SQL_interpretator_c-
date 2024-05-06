#include "SQLParser.hpp"
#include "../models/logfilter.hpp"
#include "../models/lognode.hpp"

void SQLParser::parse()
{
    this->skipWhitespace();
    if (match("SELECT"))
        this->parseSelect();
}

bool SQLParser::match(const std::string &exp)
{
    if (this->query.substr(this->currentPosition, exp.length()) == exp)
    {
        this->currentPosition += exp.length();
        return true;
    }
    return false;
}

void SQLParser::parseSelect()
{
    std::vector<std::string> fields;
    std::string tableName;
    std::vector<struct LogExpressionNode> filters;
    bool allColumns = false;
    while (true)
    {
        this->skipWhitespace();
        std::string fieldName = this->parseIdentifier();
        if (fieldName == "*")
        {
            allColumns = true;
            break;
        }
        fields.push_back(fieldName);

        this->skipWhitespace();
        if (query[currentPosition] == ',')
            ++currentPosition;
        else
            break;
    }
    this->skipWhitespace();

    if (match("FROM"))
    {
        this->skipWhitespace();
        tableName = this->parseIdentifier();
        this->skipWhitespace();

        if (match("WHERE"))
        {
            this->skipWhitespace();

            while (this->currentPosition < this->query.length())
            {
                LogExpressionNode node;
                if (match("("))
                {
                    node.children.push_back(this->parseLogicalExpression());
                    match(")");
                }
                else
                {
                    node.logFilters.push_back(this->parseComparison());
                }
                this->skipWhitespace();
                if (match("AND"))
                    node.logicalOperator = "AND";
                else if (match("OR"))
                    node.logicalOperator = "OR";
                else
                {
                    filters.push_back(node);
                    break;
                }
                this->skipWhitespace();
                filters.push_back(node);
            }
        }
        this->printLogExpression(filters);
    }
}

std::string SQLParser::parseIdentifier()
{
    size_t start = currentPosition;
    while (isalnum(this->query[this->currentPosition]) || this->query[this->currentPosition] == '_' || this->query[this->currentPosition] == '*')
    {
        this->currentPosition++;
    }
    return this->query.substr(start, this->currentPosition - start);
}

void SQLParser::skipWhitespace()
{
    while (this->currentPosition < this->query.length() && std::isspace(this->query[this->currentPosition]))
        ++this->currentPosition;
}

LogFilter SQLParser::parseComparison()
{
    LogFilter logFilter;
    std::string fieldName = this->parseIdentifier();

    this->skipWhitespace();

    std::string op;
    if (match(">="))
        op = ">=";
    else if (match("<="))
        op = "<=";
    else if (match("!="))
        op = "!=";
    else if (match(">"))
        op = ">";
    else if (match("<"))
        op = "<";
    else if (match("="))
        op = "=";

    this->skipWhitespace();
    bool isString = false;
    std::string stringValue;
    long longValue;

    if (match("'"))
    {
        stringValue = this->parseIdentifier();
        isString = true;
        match("'");
    }
    else
    {
        std::string s = this->parseIdentifier();
        isString = false;
        longValue = std::stol(s);
    }

    logFilter.field = fieldName;
    logFilter.isString = isString;
    logFilter.longValue = longValue;
    logFilter.stringValue = stringValue;
    logFilter.op = op;
    return logFilter;
}

LogExpressionNode SQLParser::parseLogicalExpression()
{
    LogExpressionNode node;
    if (match("("))
    {
        node.children.push_back(this->parseLogicalExpression());
        match(")");
    }
    else
        node.logFilters.push_back(this->parseComparison());
    this->skipWhitespace();
    if (match("AND"))
    {
        node.logicalOperator = "AND";
    }
    else if (match("OR"))
    {
        node.logicalOperator = "OR";
    }
    else
    {
        return node;
    }
    this->skipWhitespace();

    LogExpressionNode nextNode = this->parseLogicalExpression();
    node.children.push_back(nextNode);
    return node;
}

void SQLParser::printLogExpression(const std::vector<LogExpressionNode> &nodes)
{
    for (const auto &node : nodes)
    {
        for (const auto &filter : node.logFilters)
        {
            std::cout << filter.field << " ";
            std::cout << filter.op << " ";
            if (filter.isString)
            {
                std::cout << "'" << filter.stringValue << "'";
            }
            else
            {
                std::cout << filter.longValue;
            }
            std::cout << " ";
        }
        std::cout << node.logicalOperator << " " << std::endl;
        printLogExpression(node.children);
    }
}