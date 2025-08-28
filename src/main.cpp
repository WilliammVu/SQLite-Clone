#include <iostream>
#include <string>
#include "executor/executor.h"

void print_welcome() {
    std::cout << "=================================\n";
    std::cout << "  Mini SQLite Database Engine\n";
    std::cout << "=================================\n";
    std::cout << "Type SQL commands or '.help' for help\n";
    std::cout << "Type '.quit' to exit\n\n";
}

void print_help() {
    std::cout << "\nAvailable commands:\n";
    std::cout << "  CREATE TABLE table_name (column1 TYPE, column2 TYPE, ...);\n";
    std::cout << "  INSERT INTO table_name VALUES (value1, value2, ...);\n";
    std::cout << "  SELECT * FROM table_name [WHERE column = value];\n";
    std::cout << "  UPDATE table_name SET column = value WHERE column = value;\n";
    std::cout << "  DELETE FROM table_name WHERE column = value;\n";
    std::cout << "\nSupported data types: INTEGER, TEXT, REAL\n";
    std::cout << "Example:\n";
    std::cout << "  CREATE TABLE users (id INTEGER, name TEXT, age INTEGER);\n";
    std::cout << "  INSERT INTO users VALUES (1, 'Alice', 25);\n";
    std::cout << "  SELECT * FROM users WHERE id = 1;\n\n";
}

int main() {
    print_welcome();
    
    Executor executor("mini_sqlite.db");
    std::string input;
    
    while (true) {
        std::cout << "sqlite> ";
        std::getline(std::cin, input);
        
        if (input.empty()) continue;
        
        if (input == ".quit" || input == ".exit") {
            std::cout << "Goodbye!\n";
            break;
        }
        
        if (input == ".help") {
            print_help();
            continue;
        }
        
        if (input.back() != ';') {
            input += ';';
        }
        
        try {
            executor.execute_command(input);
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    return 0;
}