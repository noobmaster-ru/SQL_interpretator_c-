#ifndef __RET_MODELS_HPP

#define __RET_MODELS_HPP

#include "field.hpp"
#include <vector>
#include <string>

typedef enum
{
    Select,
    Create,
    Drop,
    Update
} ClauseType;

typedef struct
{
    std::string tableName;
    std::vector<struct FieldModel> fields;
} CreateS;

typedef struct
{
    std::string tableName;
    std::vector<std::string> fields;
    bool allColumns;
    std::vector<struct LogExpressionNode> filters;

} SelectS;

typedef struct
{
    std::string tableName;
} DropS;

typedef struct
{
    std::string tableName;
    std::vector<struct UpdateData> updates;
    std::vector<struct LogExpressionNode> filters;
} UpdateS;

typedef struct
{
    ClauseType type;
    SelectS *select;
    CreateS *create;
    DropS *drop;
    UpdateS *update;
} ParserResult;

#endif