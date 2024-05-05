#ifndef __LOGFILTER_HPP
#define __LOGFILTER_HPP

#include <string>


enum Operator
{
    EQUAL,
    GREATER,
    LESS,
    NOTEQUAL
};


struct LogFilter
{
    bool isString = false;
    std::string field;
    enum Operator op;
    std::string stringValue;
    long longValue;
};


#endif