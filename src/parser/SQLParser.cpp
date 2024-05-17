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
    else if (match("INSERT"))
    {
        InsertS *i = this->parseInsert();
        parserResult->type = Insert;
        parserResult->insert = i;
    }
    else if (match("DELETE"))
    {
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
    Expression *whereClause;
    if (match("FROM"))
    {
        this->skipWhitespace();
        tableName = this->parseIdentifier();
        this->skipWhitespace();

        if (match("WHERE"))
        {
            this->skipWhitespace();

            whereClause = this->parseExpression();
        } else {
            whereClause = nullptr;
        }
    }
    SelectS *select = new SelectS();
    select->allColumns = allColumns;
    select->fields = fields;
    select->filters = whereClause;
    select->tableName = tableName;
    return select;
}

Expression *SQLParser::parseExpression()
{
    this->skipWhitespace();
    if (match("ALL"))
    {
    }
    else
    {
        return this->parseLogicalExpression();
    }
    return nullptr;
}

Expression *SQLParser::parseLogicalExpression()
{
    Expression *expr = parseLogicalTerm();
    this->skipWhitespace();
    while (match("OR"))
    {
        this->skipWhitespace();
        Expression *right = parseLogicalTerm();
        expr = new LRExpression(expr, right, "OR", false);
        this->skipWhitespace();
    }

    return expr;
}

Expression *SQLParser::parseLogicalTerm()
{
    Expression *expr = parseLogicalMultiplier();
    this->skipWhitespace();
    while (match("AND"))
    {
        this->skipWhitespace();
        Expression *right = parseLogicalMultiplier();
        expr = new LRExpression(expr, right, "AND", false);
        this->skipWhitespace();
    }
    return expr;
}

Expression *SQLParser::parseLogicalMultiplier()
{
    if (match("NOT"))
    {
        this->skipWhitespace();
        return parseComparisonExpression(true);
    }
    else if (match("( "))
    {
        this->skipWhitespace();
        Expression *expr = parseLogicalExpression();
        this->skipWhitespace();
        match(")");
        return expr;
    }
    else if (match("("))
    {
        this->skipWhitespace();
        Expression *expr = parseComparisonExpression(false);
        this->skipWhitespace();
        match(")");
        return expr;
    }
}

// while (match("AND"))
// {
//     this->skipWhitespace();

//     Expression * right = parseComparisonExpression();
//     expr = std::make_unique<LRExpression>(std::move(expr), std::move(right), "AND");
//     this->skipWhitespace();
// }

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

bool is_digits(const std::string &str)
{
    return std::all_of(str.begin(), str.end(), ::isdigit); // C++11
}

Expression *SQLParser::parseComparisonExpression(bool n = false)
{
    Expression *expr;
    this->skipWhitespace();
    std::string columnName = this->parseIdentifier();

    this->skipWhitespace();
    std::string op;

    if (match(">="))
        op = ">=";
    else if (match("<="))
        op = "<=";
    else if (match("="))
        op = "=";
    else if (match("<"))
        op = "<";
    else if (match(">"))
        op = ">";

    this->skipWhitespace();

    if (match("'"))
    {
        std::variant<long, std::string> v = this->parseIdentifier();
        Expression *p = new ComparisonExpression(columnName, op, v, n);
        match("'");
        return p;
    }
    else
    {
        std::variant<long, std::string> v = std::stol(this->parseIdentifier());
        Expression *p = new ComparisonExpression(columnName, op, v, n);
        return p;
    }
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

// long SQLParser::parseNumber()
// {
//     size_t start = currentPosition;
//     long result = 0;
//     while (isdigit(this->query[this->currentPosition]) )
//     {
//         result *= 10;
//         result = this->query[this->currentPosition] - '0';
//         this->currentPosition++;
        
//     }
//     return result;
// }

// // Функция для вычисления постфиксного выражения
// long evaluatePostfix(const std::string& postfixExpression) {
//     std::stack<int> operands;

//     for (char c : postfixExpression) {
//         if (std::isdigit(c)) {
//             operands.push(c - '0');
//         } else if (c != ' ') {
//             long operand2 = operands.top();
//             operands.pop();
//             long operand1 = operands.top();
//             operands.pop();

//             switch (c) {
//                 case '+':
//                     operands.push(operand1 + operand2);
//                     break;
//                 case '-':
//                     operands.push(operand1 - operand2);
//                     break;
//                 case '*':
//                     operands.push(operand1 * operand2);
//                     break;
//                 case '/':
//                     operands.push(operand1 / operand2);
//                     break;
//                 case '%':
//                     operands.push(operand1 % operand2);
//                     break;
//             }
//         }
//     }

//     return operands.top();
// }

// // Функция для определения приоритета оператора
// int precedence(char op) {
//     switch (op) {
//         case '+':
//         case '-':
//             return 1;
//         case '*':
//         case '/':
//             return 2;
//         case '%':
//             return 3;
//         default:
//             return 0;
//     }
// }

// // Функция для преобразования инфиксной записи в постфиксную запись
// std::string infixToPostfix(const std::string& infixExpression) {
//     std::stack<char> operators;
//     std::string postfix;

//     for (char c : infixExpression) {
//         if (std::isdigit(c)) {
//             postfix.push_back(c);
//         } else if (c == '(') {
//             operators.push(c);
//         } else if (c == ')') {
//             while (!operators.empty() && operators.top() != '(') {
//                 postfix.push_back(' ');
//                 postfix.push_back(operators.top());
//                 operators.pop();
//             }
//             if (!operators.empty()) {
//                 operators.pop(); // Удаляем '(' из стека
//             }
//         } else {
//             while (!operators.empty() && precedence(operators.top()) >= precedence(c)) {
//                 postfix.push_back(' ');
//                 postfix.push_back(operators.top());
//                 operators.pop();
//             }
//             operators.push(c);
//         }
//     }

//     while (!operators.empty()) {
//         postfix.push_back(' ');
//         postfix.push_back(operators.top());
//         operators.pop();
//     }

//     return postfix;
// }

UpdateS *SQLParser::parseUpdate()
{
    // UPDATE <имя таблицы> SET <имя поля> = <выражение> <WHERE-клауза>
    // <выражение> ::= <Long-выражение> | <Text-выражение>
    //
    // <Long-выражение> ::= <Long-слагаемое> { <+|-> <Long-слагаемое> }
    // <Text-выражение> ::= <имя поля типа TEXT> | <строка>

    // <Long-слагаемое> ::= <Long-множитель> { <*|/|%> <Long-множитель> }
    // <Long-множитель> ::= <Long-величина> | ( <Long-выражение> )
    // <Long-величина> ::= <имя поля типа LONG> | <длинное целое> 

    // Examples:
    // 1 Age = Age  +-/*%                       ok      done with poliz
    // 2 Age = 8*(3-2)+1                        ok      done with poliz
    // 3 Age = 2 - Age*10                                                   хз как делать
    // 4 Age = 100                              ok      done with poliz     
    // 5 AGE = PhoneNumber       ok                     done
    
    // 6 Name = 'string'   ok                           done 
    // 7 Name = Surname          ok                     done
    this->skipWhitespace();
    std::string tableName = this->parseIdentifier();
    std::vector<struct UpdateData> updates; // SET ... WHERE
    std::vector<struct LogExpressionNode> filters; // WHERE ...
    this->skipWhitespace();
    if (match("SET"))
    {
        this->skipWhitespace();
        // create vector with struct updatedData: column1 = value WHERE-statement
        while (true)
        {
            this->skipWhitespace();
            UpdateData valueStructure;
            std::string first_field = this->parseIdentifier(); // first_field - <имя поля> 
            valueStructure.name_first_field = first_field;


            this->skipWhitespace();

            std::string stringValue;
            long longValue;
            bool isString = false;
            if (match("="))
            {
                this->skipWhitespace();
                if (match("'"))
                {   
                    // this is example №6: Name = 'string'
                    stringValue = this->parseIdentifier();
                    valueStructure.stringValue = stringValue;
                    isString = true;
                    match("'");
                }
                else
                {
                    
                    if (isalpha(this->query[this->currentPosition])){ // если это буква
                        // example №5 or №7: Name = Surname or Age = PhoneNumber
                        std::string s = this->parseIdentifier(); // this is second identifier,откуда нужно получить long значение или text значение
                        valueStructure.name_second_field = s;
                        this->skipWhitespace();

                        // example №1: Age = Age +-*/% POLIZexpresion
                        if (match("+")){
                            this->skipWhitespace();
                            std::vector<std::string> tokens = POLIZ::tokenizeExpression(this->query.substr(currentPosition, this->query.length() - currentPosition));
                            std::vector<std::string> postfixExpression = POLIZ::infixToPostfix(tokens);
                            long resultOfPOLIZ = POLIZ::calculatePostfix(postfixExpression);
                            valueStructure.longValue = resultOfPOLIZ;
                            valueStructure.operand = "+";
                        }else if (match("-")){
                            this->skipWhitespace();
                            std::vector<std::string> tokens = POLIZ::tokenizeExpression(this->query.substr(currentPosition, this->query.length() - currentPosition));
                            std::vector<std::string> postfixExpression = POLIZ::infixToPostfix(tokens);
                            long resultOfPOLIZ = POLIZ::calculatePostfix(postfixExpression);
                            valueStructure.longValue = resultOfPOLIZ;
                            valueStructure.operand = "-";
                        }else if (match("*")){
                            this->skipWhitespace();
                            std::vector<std::string> tokens = POLIZ::tokenizeExpression(this->query.substr(currentPosition, this->query.length() - currentPosition));
                            std::vector<std::string> postfixExpression = POLIZ::infixToPostfix(tokens);
                            long resultOfPOLIZ = POLIZ::calculatePostfix(postfixExpression);
                            valueStructure.longValue = resultOfPOLIZ;
                            valueStructure.operand = "*";
                        }else if (match("/")){
                            this->skipWhitespace();
                            std::vector<std::string> tokens = POLIZ::tokenizeExpression(this->query.substr(currentPosition, this->query.length() - currentPosition));
                            std::vector<std::string> postfixExpression = POLIZ::infixToPostfix(tokens);
                            long resultOfPOLIZ = POLIZ::calculatePostfix(postfixExpression);
                            valueStructure.longValue = resultOfPOLIZ;
                            valueStructure.operand = "/";
                        }else if (match("%")){}
                            this->skipWhitespace();
                            std::vector<std::string> tokens = POLIZ::tokenizeExpression(this->query.substr(currentPosition, this->query.length() - currentPosition));
                            std::vector<std::string> postfixExpression = POLIZ::infixToPostfix(tokens);
                            long resultOfPOLIZ = POLIZ::calculatePostfix(postfixExpression);
                            valueStructure.longValue = resultOfPOLIZ;
                            valueStructure.operand = "%";
                    }
                    else{   
                        // examples  №2 and №4
                        this->skipWhitespace();
                        std::string restOfString = this->query.substr(currentPosition,this->query.length() - currentPosition);
                        std::vector<std::string> tokens = POLIZ::tokenizeExpression(restOfString);
                        std::vector<std::string> postfixExpression = POLIZ::infixToPostfix(tokens);
                        long resultOfPOLIZ = POLIZ::calculatePostfix(postfixExpression);
                        valueStructure.longValue = resultOfPOLIZ;
                        isString = false;
                        // std::string searchOperands = "+-/%*";
                        // // Перебор символов
                        // bool foundAny = false;
                        // for (char c: searchOperands){
                        //     size_t found = searchString.find(c);
                        //     if (found != std::string::npos) {
                        //         foundAny = true; // - poliz
                        //         break;
                        //     }
                        // }
                        // if (foundAny){
                        //     // example №2: Age = 8*(3-2)+1
                        //     this->skipWhitespace();
                        //     std::string infixExpression = this->query.substr(currentPosition,this->query.length() - currentPosition);
                        //     std::string postfixExpression = infixToPostfix(infixExpression);
                        //     long result = evaluatePostfix(postfixExpression);
                        //     valueStructure.longValue = result;
                        // }
                        // else{
                        //     // this is example №4 : Age = 100 
                        //     std::string s = this->parseIdentifier();
                        //     longValue = std::stol(s);
                        //     valueStructure.longValue = longValue;
                        //     isString = false;
                        // }
                    }
                }
            }
            valueStructure.isString = isString;
            updates.push_back(valueStructure);
            this->skipWhitespace();
            // это если у нас много условий будет, хотя всего одно должно быть
            if (query[currentPosition] == ',')
                ++currentPosition;
            else
                break;
        }
        this->skipWhitespace();
        if (match("WHERE"))
        {
            this->skipWhitespace();
            Expression *whereClause = this->parseExpression();
            std::cout << 1;
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

    InsertS *insert = new InsertS();
    insert->tableName = tableName;
    insert->values = allValues;
    return insert;
}

DeleteS *SQLParser::parseDelete()
{
    // DELETE FROM tableName WHERE ...
    this->skipWhitespace();
    std::vector<struct LogExpressionNode> filters;
    std::string tableName;
    if (match("FROM"))
    {
        this->skipWhitespace();
        tableName = this->parseIdentifier();
        this->skipWhitespace();
        if (match("WHERE"))
        {
            std::cout << "OK" << std::endl;
        }
    }
    DeleteS *deletee = new DeleteS();
    deletee->filters = filters;
    deletee->tableName = tableName;
    return deletee;
}
