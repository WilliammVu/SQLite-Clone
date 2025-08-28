# Mini SQLite Database Engine

A miniature SQLite-compatible database engine implemented in C++. This project provides core database functionality with enough features to be considered "a working mini database engine" supporting all basic SQLite commands.

## Features

### Core Database Operations
- **CREATE TABLE**: Create tables with column definitions and data types
- **INSERT**: Insert rows of data into tables  
- **SELECT**: Query data with optional WHERE clauses for filtering
- **UPDATE**: Modify existing data with WHERE clause conditions
- **DELETE**: Remove rows based on WHERE clause conditions

### Data Types Supported
- **INTEGER**: 64-bit signed integers
- **TEXT**: Variable-length strings
- **REAL**: Double-precision floating point numbers

### Storage Engine
- File-based persistent storage (`mini_sqlite.db`)
- Automatic save/load on startup and shutdown
- Binary format for efficient storage

### Interactive CLI
- SQLite-style command prompt
- Built-in help system (`.help`)
- Sample queries and examples provided

## Building and Running

### Prerequisites
- C++17 compatible compiler (GCC, Clang, or MSVC)
- CMake 3.10 or higher

### Build Instructions
```bash
mkdir build
cd build
cmake ..
make
```

### Running the Database Engine
```bash
./mini_sqlite
```

## Usage Examples

```sql
-- Create a table
CREATE TABLE users (id INTEGER, name TEXT, age INTEGER);

-- Insert data
INSERT INTO users VALUES (1, 'Alice', 25);
INSERT INTO users VALUES (2, 'Bob', 30);

-- Query data
SELECT * FROM users;
SELECT * FROM users WHERE id = 1;

-- Update data  
UPDATE users SET age = 31 WHERE id = 2;

-- Delete data
DELETE FROM users WHERE id = 1;
```

### CLI Commands
- `.help` - Show available commands and syntax
- `.quit` or `.exit` - Exit the database engine

## Architecture

The mini database engine consists of four main components:

1. **Parser** (`src/parser/`): Parses SQL commands into structured data
2. **Storage** (`src/storage/`): Handles data persistence and table management  
3. **Executor** (`src/executor/`): Executes parsed commands against the storage engine
4. **Main CLI** (`src/main.cpp`): Interactive command-line interface

### File Structure
```
src/
├── types.h           # Core data structures and types
├── main.cpp          # CLI interface and main application
├── parser/
│   ├── parser.h      # SQL parsing declarations
│   └── parser.cpp    # SQL parsing implementation
├── storage/  
│   ├── storage.h     # Storage engine declarations
│   └── storage.cpp   # Storage engine implementation
└── executor/
    ├── executor.h    # Query executor declarations  
    └── executor.cpp  # Query executor implementation
```

## Testing

Sample queries are provided in `examples/sample_queries.txt`. You can test the database engine by running these queries manually or using them as a reference.

## Limitations

This is a educational/demonstration project with the following intentional limitations:
- No support for JOINs between tables
- No indexing for query optimization  
- No transaction support or ACID guarantees
- Simple regex-based SQL parsing (not a full parser)
- No support for complex data types or constraints
- Single-user, single-threaded operation

## License

This project is for educational purposes and demonstrates core database engine concepts.