#include "storage.h"
#include <iostream>
#include <sstream>
#include <iomanip>

Storage::Storage(const std::string& filename) : db_file(filename) {
    load_from_file();
}

Storage::~Storage() {
    save_to_file();
}

bool Storage::create_table(const std::string& name, const std::vector<Column>& columns) {
    if (tables.find(name) != tables.end()) {
        std::cout << "Table '" << name << "' already exists\n";
        return false;
    }
    
    Table table;
    table.name = name;
    table.columns = columns;
    tables[name] = table;
    
    std::cout << "Table '" << name << "' created successfully\n";
    return true;
}

bool Storage::insert_row(const std::string& table_name, const Row& row) {
    auto it = tables.find(table_name);
    if (it == tables.end()) {
        std::cout << "Table '" << table_name << "' does not exist\n";
        return false;
    }
    
    if (row.values.size() != it->second.columns.size()) {
        std::cout << "Column count mismatch\n";
        return false;
    }
    
    it->second.rows.push_back(row);
    std::cout << "Row inserted successfully\n";
    return true;
}

std::vector<Row> Storage::select_all(const std::string& table_name) {
    auto it = tables.find(table_name);
    if (it == tables.end()) {
        std::cout << "Table '" << table_name << "' does not exist\n";
        return {};
    }
    
    return it->second.rows;
}

std::vector<Row> Storage::select_where(const std::string& table_name, const std::string& column, const Value& value) {
    auto it = tables.find(table_name);
    if (it == tables.end()) {
        std::cout << "Table '" << table_name << "' does not exist\n";
        return {};
    }
    
    std::vector<Row> result;
    const Table& table = it->second;
    
    int column_index = -1;
    for (size_t i = 0; i < table.columns.size(); ++i) {
        if (table.columns[i].name == column) {
            column_index = i;
            break;
        }
    }
    
    if (column_index == -1) {
        std::cout << "Column '" << column << "' does not exist\n";
        return {};
    }
    
    for (const Row& row : table.rows) {
        if (row.values[column_index] == value) {
            result.push_back(row);
        }
    }
    
    return result;
}

bool Storage::update_rows(const std::string& table_name, const std::string& set_column, const Value& set_value,
                         const std::string& where_column, const Value& where_value) {
    auto it = tables.find(table_name);
    if (it == tables.end()) {
        std::cout << "Table '" << table_name << "' does not exist\n";
        return false;
    }
    
    Table& table = it->second;
    
    int set_col_idx = -1, where_col_idx = -1;
    for (size_t i = 0; i < table.columns.size(); ++i) {
        if (table.columns[i].name == set_column) set_col_idx = i;
        if (table.columns[i].name == where_column) where_col_idx = i;
    }
    
    if (set_col_idx == -1 || where_col_idx == -1) {
        std::cout << "Column not found\n";
        return false;
    }
    
    int updated_count = 0;
    for (Row& row : table.rows) {
        if (row.values[where_col_idx] == where_value) {
            row.values[set_col_idx] = set_value;
            updated_count++;
        }
    }
    
    std::cout << "Updated " << updated_count << " rows\n";
    return true;
}

bool Storage::delete_rows(const std::string& table_name, const std::string& where_column, const Value& where_value) {
    auto it = tables.find(table_name);
    if (it == tables.end()) {
        std::cout << "Table '" << table_name << "' does not exist\n";
        return false;
    }
    
    Table& table = it->second;
    
    int where_col_idx = -1;
    for (size_t i = 0; i < table.columns.size(); ++i) {
        if (table.columns[i].name == where_column) {
            where_col_idx = i;
            break;
        }
    }
    
    if (where_col_idx == -1) {
        std::cout << "Column '" << where_column << "' not found\n";
        return false;
    }
    
    auto original_size = table.rows.size();
    table.rows.erase(
        std::remove_if(table.rows.begin(), table.rows.end(),
            [where_col_idx, &where_value](const Row& row) {
                return row.values[where_col_idx] == where_value;
            }),
        table.rows.end()
    );
    
    int deleted_count = original_size - table.rows.size();
    std::cout << "Deleted " << deleted_count << " rows\n";
    return true;
}

Table* Storage::get_table(const std::string& name) {
    auto it = tables.find(name);
    return (it != tables.end()) ? &it->second : nullptr;
}

void Storage::save_to_file() {
    std::ofstream file(db_file, std::ios::binary);
    if (!file.is_open()) return;
    
    size_t table_count = tables.size();
    file.write(reinterpret_cast<const char*>(&table_count), sizeof(table_count));
    
    for (const auto& [name, table] : tables) {
        size_t name_length = name.length();
        file.write(reinterpret_cast<const char*>(&name_length), sizeof(name_length));
        file.write(name.c_str(), name_length);
        
        size_t column_count = table.columns.size();
        file.write(reinterpret_cast<const char*>(&column_count), sizeof(column_count));
        
        for (const Column& col : table.columns) {
            size_t col_name_length = col.name.length();
            file.write(reinterpret_cast<const char*>(&col_name_length), sizeof(col_name_length));
            file.write(col.name.c_str(), col_name_length);
            file.write(reinterpret_cast<const char*>(&col.type), sizeof(col.type));
        }
        
        size_t row_count = table.rows.size();
        file.write(reinterpret_cast<const char*>(&row_count), sizeof(row_count));
        
        for (const Row& row : table.rows) {
            for (size_t i = 0; i < row.values.size(); ++i) {
                const Value& val = row.values[i];
                DataType type = table.columns[i].type;
                
                if (type == DataType::INTEGER) {
                    int64_t int_val = std::get<int64_t>(val);
                    file.write(reinterpret_cast<const char*>(&int_val), sizeof(int_val));
                } else if (type == DataType::TEXT) {
                    const std::string& str_val = std::get<std::string>(val);
                    size_t str_length = str_val.length();
                    file.write(reinterpret_cast<const char*>(&str_length), sizeof(str_length));
                    file.write(str_val.c_str(), str_length);
                } else if (type == DataType::REAL) {
                    double real_val = std::get<double>(val);
                    file.write(reinterpret_cast<const char*>(&real_val), sizeof(real_val));
                }
            }
        }
    }
    
    file.close();
}

void Storage::load_from_file() {
    std::ifstream file(db_file, std::ios::binary);
    if (!file.is_open()) return;
    
    size_t table_count;
    file.read(reinterpret_cast<char*>(&table_count), sizeof(table_count));
    
    for (size_t t = 0; t < table_count; ++t) {
        size_t name_length;
        file.read(reinterpret_cast<char*>(&name_length), sizeof(name_length));
        
        std::string table_name(name_length, '\0');
        file.read(&table_name[0], name_length);
        
        Table table;
        table.name = table_name;
        
        size_t column_count;
        file.read(reinterpret_cast<char*>(&column_count), sizeof(column_count));
        
        for (size_t c = 0; c < column_count; ++c) {
            size_t col_name_length;
            file.read(reinterpret_cast<char*>(&col_name_length), sizeof(col_name_length));
            
            std::string col_name(col_name_length, '\0');
            file.read(&col_name[0], col_name_length);
            
            DataType type;
            file.read(reinterpret_cast<char*>(&type), sizeof(type));
            
            Column col;
            col.name = col_name;
            col.type = type;
            table.columns.push_back(col);
        }
        
        size_t row_count;
        file.read(reinterpret_cast<char*>(&row_count), sizeof(row_count));
        
        for (size_t r = 0; r < row_count; ++r) {
            Row row;
            for (const Column& col : table.columns) {
                if (col.type == DataType::INTEGER) {
                    int64_t int_val;
                    file.read(reinterpret_cast<char*>(&int_val), sizeof(int_val));
                    row.values.push_back(int_val);
                } else if (col.type == DataType::TEXT) {
                    size_t str_length;
                    file.read(reinterpret_cast<char*>(&str_length), sizeof(str_length));
                    std::string str_val(str_length, '\0');
                    file.read(&str_val[0], str_length);
                    row.values.push_back(str_val);
                } else if (col.type == DataType::REAL) {
                    double real_val;
                    file.read(reinterpret_cast<char*>(&real_val), sizeof(real_val));
                    row.values.push_back(real_val);
                }
            }
            table.rows.push_back(row);
        }
        
        tables[table_name] = table;
    }
    
    file.close();
}

void Storage::print_table(const std::string& table_name) {
    auto it = tables.find(table_name);
    if (it == tables.end()) {
        std::cout << "Table '" << table_name << "' does not exist\n";
        return;
    }
    
    const Table& table = it->second;
    
    for (const Column& col : table.columns) {
        std::cout << std::setw(15) << col.name;
    }
    std::cout << "\n";
    
    for (size_t i = 0; i < table.columns.size(); ++i) {
        std::cout << std::setw(15) << std::string(10, '-');
    }
    std::cout << "\n";
    
    for (const Row& row : table.rows) {
        for (size_t i = 0; i < row.values.size(); ++i) {
            const Value& val = row.values[i];
            std::cout << std::setw(15);
            
            if (std::holds_alternative<int64_t>(val)) {
                std::cout << std::get<int64_t>(val);
            } else if (std::holds_alternative<std::string>(val)) {
                std::cout << std::get<std::string>(val);
            } else if (std::holds_alternative<double>(val)) {
                std::cout << std::get<double>(val);
            }
        }
        std::cout << "\n";
    }
}