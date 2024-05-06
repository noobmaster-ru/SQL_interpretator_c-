#include "lib/Table.h"
#include "parser/SQLParser.hpp"
#include <iostream>
#include <string>
#include <vector>

int main()
{
    std::string query;
    std::getline(std::cin, query);
    SQLParser sql(query);

    sql.parse();
}