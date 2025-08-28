#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "../types.h"
#include "../storage/storage.h"
#include "../parser/parser.h"

class Executor {
private:
    Storage storage;
    Parser parser;

public:
    Executor(const std::string& db_file = "database.db");
    ~Executor() = default;

    bool execute_command(const std::string& sql);
    
private:
    bool execute_create_table(const ParsedCommand& cmd, const std::string& original_sql);
    bool execute_insert(const ParsedCommand& cmd);
    bool execute_select(const ParsedCommand& cmd);
    bool execute_update(const ParsedCommand& cmd);
    bool execute_delete(const ParsedCommand& cmd);
    
    std::vector<Column> parse_create_table_columns(const std::string& sql);
};

#endif // EXECUTOR_H