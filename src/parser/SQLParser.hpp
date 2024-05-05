#ifndef __SQLPARSER_HPP

#define __SQLPARSER_HPP

#include <iostream>
#include <string>
#include <vector>


class SQLParser {
public:
    int currentPosition;
    std::string query;

    SQLParser(const std::string& query):  currentPosition(0), query(query) {};
    void parse();
private:
    // void parseCreate();
    void parseSelect();
    // void parseInsert();
    // void parseDelete();
    // void parseDrop();
    // void parseUpdate();


    bool match(const std::string &exp);
    std::string parseIdentifier();
    void skipWhitespace();
};

#endif