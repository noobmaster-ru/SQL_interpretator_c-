#include "SQLExecutor.hpp"
#include <unordered_map>
#include <iostream>

template <typename T, typename... Ts>
std::ostream &operator<<(std::ostream &os, const std::variant<T, Ts...> &v)
{
    std::visit([&os](auto &&arg)
               { os << arg; },
               v);
    return os;
}

SQLExecutor::SQLExecutor()
{
}

SQLExecutor::~SQLExecutor()
{
}

bool SQLExecutor::execCreate(CreateS *createStruct)
{
    struct TableStruct tableStruct;
    tableStruct.numOfFields = createStruct->fields.size();

    struct FieldDef *fields = new struct FieldDef[tableStruct.numOfFields];
    for (int i = 0; i < tableStruct.numOfFields; i++)
    {
        strcpy(fields[i].name, createStruct->fields[i].name.c_str());
        fields[i].len = createStruct->fields[i].len;
        fields[i].type = (createStruct->fields[i].type == "TEXT") ? Text : Long;
    }
    tableStruct.fieldsDef = fields;
    enum Errors res = createTable(const_cast<char *>(createStruct->tableName.c_str()), &tableStruct);
    return res == OK;
}

bool SQLExecutor::execInsert()

bool SQLExecutor::execSelect(SelectS *selectStruct)
{
    THandle tableHandle;

    enum Errors res = openTable(const_cast<char *>(selectStruct->tableName.c_str()), &tableHandle);
    enum FieldType fieldTypes[selectStruct->fields.size()];

    std::vector<std::string> &fields = selectStruct->fields;

    std::unordered_map<std::string, enum FieldType> map;

    if (selectStruct->allColumns)
    {
        unsigned int numFields;
        if (getFieldsNum(tableHandle, &numFields) != OK)
            return 1;
        std::vector<std::string> fieldNames(numFields);
        for (unsigned i = 0; i < numFields; ++i)
        {
            char *fieldName;
            if (getFieldName(tableHandle, i, &fieldName) != OK)
            {
                return 1;
            }
            fieldNames[i] = fieldName;
        }
        fields = fieldNames;
    }

    for (const auto &f : fields)
    {
        enum FieldType type;
        res = getFieldType(tableHandle, const_cast<char *>(f.c_str()), &type);
        map.insert(std::make_pair(f, type));
    }

    std::vector<std::vector<std::variant<std::string, long>>> result;

    moveFirst(tableHandle);
    std::vector<int> widths;
    std::cout << std::endl;
    for (const auto &s : fields)
    {
        std::cout << s << " | ";
    }
    std::cout << std::endl;
    std::cout << "--------------" << std::endl;
    while (!afterLast(tableHandle))
    {
        std::vector<std::variant<std::string, long>> v;
        for (auto &field : fields)
        {
            switch (map[field])
            {
            case Long:
                long l;
                res = getLong(tableHandle, const_cast<char *>(field.c_str()), &l);
                v.push_back(l);
                break;
            case Text:
                char *text;
                res = getText(tableHandle, const_cast<char *>(field.c_str()), &text);
                std::string t(text);
                v.push_back(t);
                break;
            }
        }
        for (const auto &f : v)
        {
            std::cout << f << " | ";
        }
        std::cout << std::endl;
        res = moveNext(tableHandle);
        if (res != OK)
        {
            printf("Ошибка при перемещении указателя на следующую запись.\n");
            closeTable(tableHandle);
            return 1;
        }
        result.push_back(v);
    }
    return true;
}
