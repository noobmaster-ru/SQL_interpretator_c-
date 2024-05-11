#ifndef __SQL_HPP

#define __SQL_HPP

#include "../executor/SQLExecutor.hpp"
#include "../parser/SQLParser.hpp"


class SQL
{
private:
    SQLExecutor exec;
    SQLParser parse;

public:
    SQL();
    ~SQL();
};

#endif