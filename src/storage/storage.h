#ifndef STORAGE_H
#define STORAGE_H

#include "../types.h"
#include <unordered_map>
#include <fstream>

class Storage {
private:
    std::unordered_map<std::string, Table> tables;
    std::string db_file;

public:
    Storage(const std::string& filename = "database.db");
    ~Storage();

    bool create_table(const std::string& name, const std::vector<Column>& columns);
    bool insert_row(const std::string& table_name, const Row& row);
    std::vector<Row> select_all(const std::string& table_name);
    std::vector<Row> select_where(const std::string& table_name, const std::string& column, const Value& value);
    bool update_rows(const std::string& table_name, const std::string& set_column, const Value& set_value,
                     const std::string& where_column, const Value& where_value);
    bool delete_rows(const std::string& table_name, const std::string& where_column, const Value& where_value);
    
    Table* get_table(const std::string& name);
    void save_to_file();
    void load_from_file();
    void print_table(const std::string& table_name);
};

#endif // STORAGE_H