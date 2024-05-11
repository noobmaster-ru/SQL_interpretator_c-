#include "lib/Table.h"
#include "sql/SQL.hpp"
#include <iostream>
#include <string>
#include <vector>

int main()
{
    std::string query;
    std::getline(std::cin, query);
    SQL sql;
    sql.run(query);
}