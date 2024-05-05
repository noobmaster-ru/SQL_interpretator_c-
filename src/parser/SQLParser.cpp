#include "SQLParser.hpp"
#include "../models/logfilter.hpp"
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
    std::cout << allColumns << std::endl;

    if (match("FROM"))
    {
        this->skipWhitespace();
        std::string tableName = this->parseIdentifier();
        this->skipWhitespace();

        if (match("WHERE"))
        {
            this->skipWhitespace();

            std::vector<struct LogFilter> filters;
            while (true)
            {
                if (this->query.length() >= this->currentPosition)
                    break;
                this->skipWhitespace();

                std::string identifier = this->parseIdentifier();
                struct LogFilter logFilter;
                if (match(">"))
                    logFilter.op = GREATER;
                else if (match("="))
                    logFilter.op = EQUAL;
                else if (match("<"))
                    logFilter.op = LESS;
                else if (match("!="))
                    logFilter.op = NOTEQUAL;
                this->skipWhitespace();
                if (match("("))
                {
                    logFilter.isString = true;
                    std::string value = this->parseIdentifier();
                    logFilter.stringValue = value;
                }
                else
                {
                    long value = std::stol(this->parseIdentifier());
                    logFilter.longValue = value;
                }
            }
        }
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
    {
        ++this->currentPosition;
    }
}