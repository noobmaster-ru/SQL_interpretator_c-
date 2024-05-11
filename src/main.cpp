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

    ParserResult *p = sql.parse();

    if(p->type == Create)
    {
        for(const auto &f: p->create->fields)
        {
            std::cout << f.name << " " << f.type << std::endl;
        }
    }
}