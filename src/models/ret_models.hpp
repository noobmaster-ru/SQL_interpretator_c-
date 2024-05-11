#ifndef __RET_MODELS_HPP

#define __RET_MODELS_HPP

#include "field.hpp"

typedef enum
{
    Select,
    Create
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
    ClauseType type;
    SelectS *select;
    CreateS *create;
} ParserResult;

#endif