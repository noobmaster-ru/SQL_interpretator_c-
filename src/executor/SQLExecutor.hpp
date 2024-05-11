#ifndef __SQLEXECUTOR_HPP
#define __SQLEXECUTOR_HPP

#include "../models/ret_models.hpp"
#include "../lib/Table.h"

class SQLExecutor
{
public:
    bool execCreate(CreateS createStruct);
    SQLExecutor();

    ~SQLExecutor();
};

#endif