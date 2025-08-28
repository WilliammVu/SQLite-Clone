#ifndef PARSER_H
#define PARSER_H

#include "../types.h"
#include <string>
#include <sstream>
#include <algorithm>

class Parser {
public:
    Parser() = default;
    ~Parser() = default;

    ParsedCommand parse_command(const std::string& sql);
    
private:
    std::string to_upper(const std::string& str);
    std::vector<std::string> split(const std::string& str, char delimiter);
    std::string trim(const std::string& str);
    Value parse_value(const std::string& str);
    DataType parse_data_type(const std::string& type_str);
};

#endif //PARSER_H