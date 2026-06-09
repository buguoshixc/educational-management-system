#pragma once
#include <string>
using std::string;

class User {
protected:
    string name;
    string id;
public:
    User();
    User(string name, string id);
    virtual ~User();
    virtual void menu() = 0;
};
