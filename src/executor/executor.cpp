#include "executor.h"
#include <iostream>
#include <regex>

Executor::Executor(const std::string& db_file) : storage(db_file) {
}

bool Executor::execute_command(const std::string& sql) {
    ParsedCommand cmd = parser.parse_command(sql);
    
    switch (cmd.type) {
        case SQLCommandType::CREATE_TABLE:
            return execute_create_table(cmd, sql);
        case SQLCommandType::INSERT:
            return execute_insert(cmd);
        case SQLCommandType::SELECT:
            return execute_select(cmd);
        case SQLCommandType::UPDATE:
            return execute_update(cmd);
        case SQLCommandType::DELETE:
            return execute_delete(cmd);
        case SQLCommandType::INVALID:
            std::cout << "Invalid SQL command\n";
            return false;
    }
    return false;
}

bool Executor::execute_create_table(const ParsedCommand& cmd, const std::string& original_sql) {
    std::vector<Column> columns = parse_create_table_columns(original_sql);
    return storage.create_table(cmd.table_name, columns);
}

bool Executor::execute_insert(const ParsedCommand& cmd) {
    Row row;
    row.values = cmd.values;
    return storage.insert_row(cmd.table_name, row);
}

bool Executor::execute_select(const ParsedCommand& cmd) {
    std::vector<Row> results;
    
    if (cmd.has_where) {
        results = storage.select_where(cmd.table_name, cmd.where_column, cmd.where_value);
    } else {
        results = storage.select_all(cmd.table_name);
    }
    
    if (results.empty()) {
        std::cout << "No rows found\n";
        return true;
    }
    
    storage.print_table(cmd.table_name);
    return true;
}

bool Executor::execute_update(const ParsedCommand& cmd) {
    if (!cmd.has_where || cmd.column_names.empty() || cmd.values.empty()) {
        std::cout << "Invalid UPDATE command\n";
        return false;
    }
    
    return storage.update_rows(cmd.table_name, cmd.column_names[0], cmd.values[0],
                              cmd.where_column, cmd.where_value);
}

bool Executor::execute_delete(const ParsedCommand& cmd) {
    if (!cmd.has_where) {
        std::cout << "DELETE without WHERE clause not supported\n";
        return false;
    }
    
    return storage.delete_rows(cmd.table_name, cmd.where_column, cmd.where_value);
}

std::vector<Column> Executor::parse_create_table_columns(const std::string& sql) {
    std::vector<Column> columns;
    
    std::regex create_regex(R"(CREATE TABLE\s+\w+\s*\(([^)]+)\))", std::regex_constants::icase);
    std::smatch matches;
    
    if (std::regex_search(sql, matches, create_regex)) {
        std::string columns_str = matches[1].str();
        
        std::stringstream ss(columns_str);
        std::string column_def;
        
        while (std::getline(ss, column_def, ',')) {
            Column col;
            std::stringstream col_ss(column_def);
            std::string token;
            std::vector<std::string> tokens;
            
            while (col_ss >> token) {
                tokens.push_back(token);
            }
            
            if (tokens.size() >= 2) {
                col.name = tokens[0];
                std::string type_str = tokens[1];
                std::transform(type_str.begin(), type_str.end(), type_str.begin(), ::toupper);
                
                if (type_str == "INTEGER" || type_str == "INT") {
                    col.type = DataType::INTEGER;
                } else if (type_str == "TEXT" || type_str == "VARCHAR") {
                    col.type = DataType::TEXT;
                } else if (type_str == "REAL" || type_str == "FLOAT" || type_str == "DOUBLE") {
                    col.type = DataType::REAL;
                } else {
                    col.type = DataType::TEXT;
                }
                
                for (size_t i = 2; i < tokens.size(); ++i) {
                    std::string modifier = tokens[i];
                    std::transform(modifier.begin(), modifier.end(), modifier.begin(), ::toupper);
                    
                    if (modifier == "PRIMARY" && i + 1 < tokens.size()) {
                        std::string key = tokens[i + 1];
                        std::transform(key.begin(), key.end(), key.begin(), ::toupper);
                        if (key == "KEY") {
                            col.primary_key = true;
                            i++;
                        }
                    } else if (modifier == "NOT" && i + 1 < tokens.size()) {
                        std::string null_kw = tokens[i + 1];
                        std::transform(null_kw.begin(), null_kw.end(), null_kw.begin(), ::toupper);
                        if (null_kw == "NULL") {
                            col.not_null = true;
                            i++;
                        }
                    }
                }
                
                columns.push_back(col);
            }
        }
    }
    
    return columns;
}