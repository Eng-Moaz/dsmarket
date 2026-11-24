#pragma once
#include <string>
using namespace std;

// priority: 1 = Regular, 2 = Express, 3 = VIP
struct Customer {
    int id;
    string name;
    int priority;
    int arrivalTime;
};

inline string priorityName(int priority) {
    if (priority == 3) return "VIP";
    if (priority == 2) return "Express";
    return "Regular";
}
