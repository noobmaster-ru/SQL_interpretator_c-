#ifndef __UPDATE_DATA_HPP

#define __UPDATE_DATA_HPP
#include <string>
struct UpdateData
{
    std::string name_first_field; // <имя поля> куда меняем 
    std::string name_second_field; // <имя поля типа TEXT> | <имя поля типа LONG> откуда берем данные и потом делаем манипуляции
    
    std::string stringValue;
    long longValue;

    std::string operand; 
    bool isString;
};

#endif