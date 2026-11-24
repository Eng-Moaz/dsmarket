cmake_minimum_required(VERSION 3.16)
project(dsmarket LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(dsmarket
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp
    src/Supermarket.h
    src/Supermarket.cpp
    src/Product.h
    src/Customer.h
    src/HashTable.h
    src/Queue.h
    src/PriorityQueue.h
    src/Stack.h
    src/LinkedList.h
    src/BST.h
)

target_link_libraries(dsmarket PRIVATE Qt6::Widgets)
