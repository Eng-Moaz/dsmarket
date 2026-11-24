# dsmarket

Supermarket Management System - Data Structures project.

## Data structures

| File | Data structure | Used for |
|------|----------------|----------|
| `src/HashTable.h` | Hash Table (chaining) | Product lookup by ID |
| `src/Queue.h` | Circular Queue | Customer service line and restocking |
| `src/PriorityQueue.h` | Max-Heap | Express / VIP checkout lane |
| `src/Stack.h` | Stack (linked list) | Undo operations |
| `src/LinkedList.h` | Singly Linked List | Shopping cart |
| `src/BST.h` | Binary Search Tree | Products sorted by ID |

`src/Supermarket.h` / `src/Supermarket.cpp` use all of them together.

## Branches

- `main` - data structures and the supermarket logic
- `tui` - console (terminal) version
- `gui` - Qt desktop version

## Build and run (TUI)

```
g++ -std=c++17 src/*.cpp -o dsmarket
./dsmarket
```

On Windows (MinGW) the output file is `dsmarket.exe`. You can also just add all the files in `src/` to a Code::Blocks / Visual Studio project.
