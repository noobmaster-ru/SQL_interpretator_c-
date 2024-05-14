#include "SQLParser.hpp"

ParserResult *SQLParser::parse()
{
    ParserResult *parserResult = new ParserResult();
    parserResult->create = NULL;
    parserResult->select = NULL;
    parserResult->drop = NULL;
    parserResult->update = NULL;
    parserResult->insert = NULL;
    parserResult->deletee = NULL;
    this->skipWhitespace();

    if (match("SELECT"))
    {
        SelectS *s = this->parseSelect();
        parserResult->type = Select;
        parserResult->select = s;
    }
    else if (match("CREATE"))
    {
        this->skipWhitespace();
        if (match("TABLE"))
        {
            CreateS *c = this->parseCreate();
            parserResult->create = c;
            parserResult->type = Create;
        }
    }
    else if (match("DROP"))
    {
        this->skipWhitespace();
        if (match("TABLE"))
        {
            DropS *d = this->parseDrop();
            parserResult->drop = d;
            parserResult->type = Drop;
        }
    }
    else if (match("UPDATE"))
    {
        UpdateS *u = this->parseUpdate();
        parserResult->type = Update;
        parserResult->update = u;
    }
    else if (match("INSERT")){
        InsertS *i =this->parseInsert();
        parserResult->type = Insert;
        parserResult->insert = i;
    }
    else if (match("DELETE")){
        DeleteS *d = this->parseDelete();
        parserResult->type = Delete;
        parserResult->deletee = d;
    } 

    return parserResult;
}

bool SQLParser::match(const std::string &exp)
{
    if (this->query.substr(this->currentPosition, exp.length()) == exp)
    {
        this->currentPosition += exp.length();
        return true;
    }
    return false;
}

CreateS *SQLParser::parseCreate()
{
    this->skipWhitespace();
    std::string tableName = this->parseIdentifier();

    std::vector<struct FieldModel> fields;

    this->skipWhitespace();

    if (match("("))
    {
        while (!match(")"))
        {
            this->skipWhitespace();
            struct FieldModel fieldData;
            std::string fieldName = this->parseIdentifier();

            this->skipWhitespace();

            fieldData.name = fieldName;

            std::string typeName = this->parseIdentifier();

            fieldData.type = typeName;

            this->skipWhitespace();
            if (match("(")) // it means it is text
            {
                int textLen = std::stoi(this->parseIdentifier());
                fieldData.len = textLen;
                match(")");
            }
            else
            {
                this->skipWhitespace();
                fieldData.len = sizeof(long);
            }

            fields.push_back(fieldData);

            if (this->query[this->currentPosition] == ',')
                ++this->currentPosition;
        }
        this->skipWhitespace();
    }

    CreateS *createS = new CreateS();
    createS->tableName = tableName;
    createS->fields = fields;

    return createS;
}

SelectS *SQLParser::parseSelect()
{
    // SELECT id, name, age FROM table
    // SELECT *
    std::vector<std::string> fields;
    std::string tableName;
    std::vector<struct LogExpressionNode> filters;
    bool allColumns = false;
    while (true)
    {
        this->skipWhitespace();
        std::string fieldName = this->parseIdentifier();
        if (fieldName == "*")
        {
            allColumns = true;
            break;
        }
        fields.push_back(fieldName);

        this->skipWhitespace();
        if (query[currentPosition] == ',')
            ++currentPosition;
        else
            break;
    }
    this->skipWhitespace();

    if (match("FROM"))
    {
        this->skipWhitespace();
        tableName = this->parseIdentifier();
        this->skipWhitespace();

        if (match("WHERE"))
        {
            this->skipWhitespace();

            while (this->currentPosition < this->query.length())
            {
                LogExpressionNode node;
                if (match("("))
                {
                    node.children.push_back(this->parseLogicalExpression());
                    match(")");
                }
                else
                    node.logFilters.push_back(this->parseComparison());
                this->skipWhitespace();
                if (match("AND"))
                    node.logicalOperator = "AND";
                else if (match("OR"))
                    node.logicalOperator = "OR";
                else
                {
                    filters.push_back(node);
                    break;
                }
                this->skipWhitespace();
                filters.push_back(node);
            }
        }
        this->printLogExpression(filters);
    }
    SelectS *select = new SelectS();
    select->allColumns = allColumns;
    select->fields = fields;
    select->filters = filters;
    select->tableName = tableName;
    return select;
}

std::string SQLParser::parseIdentifier()
{
    size_t start = currentPosition;
    while (isalnum(this->query[this->currentPosition]) || this->query[this->currentPosition] == '_' || this->query[this->currentPosition] == '*')
    {
        this->currentPosition++;
    }
    return this->query.substr(start, this->currentPosition - start);
}

void SQLParser::skipWhitespace()
{
    while (this->currentPosition < this->query.length() && std::isspace(this->query[this->currentPosition]))
        ++this->currentPosition;
}

LogFilter SQLParser::parseComparison()
{
    LogFilter logFilter;
    std::string fieldName = this->parseIdentifier();

    this->skipWhitespace();

    std::string op;
    if (match(">="))
        op = ">=";
    else if (match("<="))
        op = "<=";
    else if (match("!="))
        op = "!=";
    else if (match(">"))
        op = ">";
    else if (match("<"))
        op = "<";
    else if (match("="))
        op = "=";

    this->skipWhitespace();
    bool isString = false;
    std::string stringValue;
    long longValue;

    if (match("'"))
    {
        stringValue = this->parseIdentifier();
        isString = true;
        match("'");
    }
    else
    {
        std::string s = this->parseIdentifier();
        isString = false;
        longValue = std::stol(s);
    }

    logFilter.field = fieldName;
    logFilter.isString = isString;
    logFilter.longValue = longValue;
    logFilter.stringValue = stringValue;
    logFilter.op = op;
    return logFilter;
}

LogExpressionNode SQLParser::parseLogicalExpression()
{
    LogExpressionNode node;
    if (match("("))
    {
        node.children.push_back(this->parseLogicalExpression());
        match(")");
    }
    else
        node.logFilters.push_back(this->parseComparison());
    this->skipWhitespace();
    if (match("AND"))
        node.logicalOperator = "AND";
    else if (match("OR"))
        node.logicalOperator = "OR";
    else
        return node;
    this->skipWhitespace();

    LogExpressionNode nextNode = this->parseLogicalExpression();
    node.children.push_back(nextNode);
    return node;
}

DropS *SQLParser::parseDrop()
{
    this->skipWhitespace();
    std::string tableName = this->parseIdentifier();
    std::cout << tableName << std::endl;
    DropS *drop = new DropS();
    drop->tableName = tableName;
    return drop;
}

UpdateS *SQLParser::parseUpdate()
{

    this->skipWhitespace();
    std::string tableName = this->parseIdentifier();
    std::vector<struct UpdateData> updates;
    std::vector<struct LogExpressionNode> filters;
    this->skipWhitespace();
    if (match("SET"))
    {
        this->skipWhitespace();
        // create vector with struct updatedData: column1 = value, column2 = value2, ... until WHERE-statement
        while (true)
        {
            this->skipWhitespace();
            UpdateData сolumnWithValue;
            std::string field = this->parseIdentifier();

            this->skipWhitespace();

            std::string stringValue;
            long longValue;
            bool isString = false;
            if (match("="))
            {
                this->skipWhitespace();
                if (match("'"))
                {
                    stringValue = this->parseIdentifier();
                    сolumnWithValue.stringValue = stringValue;
                    isString = true;
                    match("'");
                }
                else
                {
                    std::string s = this->parseIdentifier();
                    longValue = std::stol(s);
                    сolumnWithValue.longValue = longValue;
                    isString = false;
                }
            }
            сolumnWithValue.field = field;
            сolumnWithValue.isString = isString;
            updates.push_back(сolumnWithValue);
            this->skipWhitespace();
            if (query[currentPosition] == ',')
                ++currentPosition;
            else
                break;
        }
        this->skipWhitespace();
        if (match("WHERE"))
        {
            this->skipWhitespace();

            while (this->currentPosition < this->query.length())
            {
                LogExpressionNode node;
                if (match("("))
                {
                    node.children.push_back(this->parseLogicalExpression());
                    match(")");
                }
                else
                    node.logFilters.push_back(this->parseComparison());
                this->skipWhitespace();
                if (match("AND"))
                    node.logicalOperator = "AND";
                else if (match("OR"))
                    node.logicalOperator = "OR";
                else
                {
                    filters.push_back(node);
                    break;
                }
                this->skipWhitespace();
                filters.push_back(node);
            }
            this->printLogExpression(filters);
        }
    }
    UpdateS *update = new UpdateS();
    update->tableName = tableName;
    update->updates = updates;
    update->filters = filters;
    return update;
}

InsertS *SQLParser::parseInsert()
{
    // INSERT INTO table_name(value1,value2,... ,valueN);
    this->skipWhitespace();
    std::string tableName;
    std::vector<struct ValuesInsert> allValues;
    if (match("INTO"))
    {
        this->skipWhitespace();
        tableName = this->parseIdentifier();
        this->skipWhitespace();
        if (match("("))
        {
            while (true)
            {
                this->skipWhitespace();
                ValuesInsert value;
                std::string stringValue;
                long longValue;
                bool isString = false;
                if (match("'"))
                {
                    stringValue = this->parseIdentifier();
                    isString = true;
                    match("'");
                }
                else
                {
                    std::string s = this->parseIdentifier();
                    longValue = std::stol(s);
                    isString = false;
                }   
                value.isString = isString;
                value.longValue = longValue;
                value.stringValue = stringValue;
                allValues.push_back(value);
                
                this->skipWhitespace();
                if (query[currentPosition] == ',')
                    ++currentPosition;
                else
                    break;
            }
        }
    }
    // for (const auto &f: allValues){
    //     if (f.isString == true)
    //     {
    //         std::cout << f.stringValue << " ";

    //     }
    //     else
    //     {
    //         std::cout << f.longValue << " ";
    //     }
    // }
    InsertS *insert = new InsertS();
    insert->tableName = tableName;
    insert->values = allValues;
    return insert;
}

DeleteS *SQLParser::parseDelete(){
    // DELETE FROM tableName WHERE ...
    this->skipWhitespace();
    std::vector<struct LogExpressionNode> filters;
    std::string tableName;
    if (match("FROM")){
        this->skipWhitespace();
        tableName = this->parseIdentifier();
        this->skipWhitespace();
        if (match("WHERE"))
        {
            this->skipWhitespace();

            while (this->currentPosition < this->query.length())
            {
                LogExpressionNode node;
                if (match("("))
                {
                    node.children.push_back(this->parseLogicalExpression());
                    match(")");
                }
                else
                    node.logFilters.push_back(this->parseComparison());
                this->skipWhitespace();
                if (match("AND"))
                    node.logicalOperator = "AND";
                else if (match("OR"))
                    node.logicalOperator = "OR";
                else
                {
                    filters.push_back(node);
                    break;
                }
                this->skipWhitespace();
                filters.push_back(node);
            }
            this->printLogExpression(filters);
        }
    }
    DeleteS *deletee = new DeleteS();
    deletee->filters = filters;
    deletee->tableName = tableName;
    return deletee;
}
void SQLParser::printLogExpression(const std::vector<LogExpressionNode> &nodes)
{
    for (const auto &node : nodes)
    {
        for (const auto &filter : node.logFilters)
        {
            std::cout << filter.field << " ";
            std::cout << filter.op << " ";
            if (filter.isString)
                std::cout << "'" << filter.stringValue << "'";
            else
                std::cout << filter.longValue;
            std::cout << " ";
        }
        std::cout << node.logicalOperator << " " << std::endl;
        printLogExpression(node.children);
    }
}