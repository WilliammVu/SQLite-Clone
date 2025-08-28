#include "parser.h"
#include <iostream>
#include <regex>

ParsedCommand Parser::parse_command(const std::string& sql) {
    ParsedCommand cmd;
    cmd.type = SQLCommandType::INVALID;
    
    std::string upper_sql = to_upper(trim(sql));
    
    if (upper_sql.find("CREATE TABLE") == 0) {
        cmd.type = SQLCommandType::CREATE_TABLE;
        
        std::regex create_regex(R"(CREATE TABLE\s+(\w+)\s*\(([^)]+)\))");
        std::smatch matches;
        
        if (std::regex_search(sql, matches, create_regex)) {
            cmd.table_name = matches[1].str();
            std::string columns_str = matches[2].str();
            
            auto column_defs = split(columns_str, ',');
            for (const std::string& col_def : column_defs) {
                auto parts = split(trim(col_def), ' ');
                if (parts.size() >= 2) {
                    cmd.column_names.push_back(parts[0]);
                }
            }
        }
    }
    else if (upper_sql.find("INSERT INTO") == 0) {
        cmd.type = SQLCommandType::INSERT;
        
        std::regex insert_regex(R"(INSERT INTO\s+(\w+)\s*(?:\(([^)]+)\))?\s*VALUES\s*\(([^)]+)\))");
        std::smatch matches;
        
        if (std::regex_search(sql, matches, insert_regex)) {
            cmd.table_name = matches[1].str();
            
            if (matches[2].matched) {
                std::string columns_str = matches[2].str();
                auto cols = split(columns_str, ',');
                for (const std::string& col : cols) {
                    cmd.column_names.push_back(trim(col));
                }
            }
            
            std::string values_str = matches[3].str();
            auto vals = split(values_str, ',');
            for (const std::string& val : vals) {
                cmd.values.push_back(parse_value(trim(val)));
            }
        }
    }
    else if (upper_sql.find("SELECT") == 0) {
        cmd.type = SQLCommandType::SELECT;
        
        std::regex select_regex(R"(SELECT\s+([^FROM]+)\s+FROM\s+(\w+)(?:\s+WHERE\s+(\w+)\s*=\s*(.+))?)");
        std::smatch matches;
        
        if (std::regex_search(sql, matches, select_regex)) {
            std::string columns_str = trim(matches[1].str());
            cmd.table_name = matches[2].str();
            
            if (columns_str != "*") {
                auto cols = split(columns_str, ',');
                for (const std::string& col : cols) {
                    cmd.column_names.push_back(trim(col));
                }
            }
            
            if (matches[3].matched && matches[4].matched) {
                cmd.has_where = true;
                cmd.where_column = matches[3].str();
                cmd.where_value = parse_value(trim(matches[4].str()));
            }
        }
    }
    else if (upper_sql.find("UPDATE") == 0) {
        cmd.type = SQLCommandType::UPDATE;
        
        std::regex update_regex(R"(UPDATE\s+(\w+)\s+SET\s+(\w+)\s*=\s*(.+?)\s+WHERE\s+(\w+)\s*=\s*(.+))");
        std::smatch matches;
        
        if (std::regex_search(sql, matches, update_regex)) {
            cmd.table_name = matches[1].str();
            cmd.column_names.push_back(matches[2].str());
            cmd.values.push_back(parse_value(trim(matches[3].str())));
            cmd.has_where = true;
            cmd.where_column = matches[4].str();
            cmd.where_value = parse_value(trim(matches[5].str()));
        }
    }
    else if (upper_sql.find("DELETE FROM") == 0) {
        cmd.type = SQLCommandType::DELETE;
        
        std::regex delete_regex(R"(DELETE FROM\s+(\w+)\s+WHERE\s+(\w+)\s*=\s*(.+))");
        std::smatch matches;
        
        if (std::regex_search(sql, matches, delete_regex)) {
            cmd.table_name = matches[1].str();
            cmd.has_where = true;
            cmd.where_column = matches[2].str();
            cmd.where_value = parse_value(trim(matches[3].str()));
        }
    }
    
    return cmd;
}

std::string Parser::to_upper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::vector<std::string> Parser::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    
    return tokens;
}

std::string Parser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

Value Parser::parse_value(const std::string& str) {
    std::string trimmed = trim(str);
    
    if (trimmed.front() == '\'' && trimmed.back() == '\'') {
        return trimmed.substr(1, trimmed.length() - 2);
    }
    
    if (trimmed.find('.') != std::string::npos) {
        try {
            return std::stod(trimmed);
        } catch (...) {
            return trimmed;
        }
    }
    
    try {
        return static_cast<int64_t>(std::stoll(trimmed));
    } catch (...) {
        return trimmed;
    }
}

DataType Parser::parse_data_type(const std::string& type_str) {
    std::string upper_type = to_upper(type_str);
    
    if (upper_type == "INTEGER" || upper_type == "INT") {
        return DataType::INTEGER;
    } else if (upper_type == "TEXT" || upper_type == "VARCHAR") {
        return DataType::TEXT;
    } else if (upper_type == "REAL" || upper_type == "FLOAT" || upper_type == "DOUBLE") {
        return DataType::REAL;
    }
    
    return DataType::TEXT;
}