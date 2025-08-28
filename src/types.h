#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>
#include <variant>
#include <memory>

enum class DataType {
    INTEGER,
    TEXT,
    REAL
};

using Value = std::variant<int64_t, std::string, double>;

struct Column {
    std::string name;
    DataType type;
    bool primary_key = false;
    bool not_null = false;
};

struct Row {
    std::vector<Value> values;
};

struct Table {
    std::string name;
    std::vector<Column> columns;
    std::vector<Row> rows;
};

enum class SQLCommandType {
    CREATE_TABLE,
    INSERT,
    SELECT,
    UPDATE,
    DELETE,
    INVALID
};

struct ParsedCommand {
    SQLCommandType type;
    std::string table_name;
    std::vector<std::string> column_names;
    std::vector<Value> values;
    std::string where_column;
    Value where_value;
    bool has_where = false;
};

#endif // TYPES_H