cmake_minimum_required(VERSION 3.10)
project(MiniSQLite)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

include_directories(src)

set(SOURCES
    src/main.cpp
    src/parser/parser.cpp
    src/storage/storage.cpp
    src/executor/executor.cpp
)

add_executable(mini_sqlite ${SOURCES})

target_include_directories(mini_sqlite PRIVATE src)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(mini_sqlite PRIVATE DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()