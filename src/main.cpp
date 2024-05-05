#include "lib/Table.h"
#include "parser/SQLParser.hpp"
#include <iostream>
#include <string>
#include <vector>

int main()
{
    std::string query;

    SQLParser sql("SELECT * FROM table");

    sql.parse();
}