#include "SQLExecutor.hpp"

SQLExecutor::SQLExecutor()
{
}

SQLExecutor::~SQLExecutor()
{
}

bool SQLExecutor::execCreate(CreateS *createStruct)
{
    TableStruct tableStruct;
    tableStruct.numOfFields = createStruct->fields.size();

    FieldDef *fields = new FieldDef[tableStruct.numOfFields];
    for (int i = 0; i < tableStruct.numOfFields; i++)
    {
        fields[i] = new FieldDef();
        strcpy(fields[i].name, createStruct->fields[i].name.c_str());
        fields[i].len = createStruct->fields[i].len;
        fields[i].type = (createStruct->fields[i].type == "TEXT") ? Text : Long;
    }
    tableStruct.fieldsDef = fields;
    Errors res = createTable(createStruct->tableName.c_str(), &tableStruct);
    return res == OK;
}