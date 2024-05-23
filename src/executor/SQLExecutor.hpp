#ifndef __SQLEXECUTOR_HPP
#define __SQLEXECUTOR_HPP

#include "../models/ret_models.hpp"
#include "../lib/Table.hpp"

class SQLExecutor
{
public:
    bool execCreate(CreateS *createStruct);
    bool execSelect(SelectS *selectStruct);
    bool execInsert(InsertS *insertStruct);
    SQLExecutor();  
    
    ~SQLExecutor();
};

#endif