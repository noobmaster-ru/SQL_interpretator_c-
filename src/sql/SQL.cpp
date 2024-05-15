#include "SQL.hpp"

SQL::SQL()
{
 
}

SQL::~SQL()
{
    delete this->parser;
    delete this->executor;
}


void SQL::run(std::string query)
{
    this->parser = new SQLParser(query);
    this->executor = new SQLExecutor();
    ParserResult *result = this->parser->parse();
    if(result->type == Create)
    {
        this->executor->execCreate(result->create);
    } else if (result->type == Select)
    {
        this->executor->execSelect(result->select);
    }
    // std::cout << result->create->tableName << std::endl;

    // for(const auto &f: result->create->fields)
    // {
    //     std::cout << f.name << f.type << std::endl;
    // }
}