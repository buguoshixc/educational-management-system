#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include "User.h"

class Manager : public User
{
public:
    Manager();
    ~Manager() override;
    bool login(std::string name, std::string password);
    void menu();
    void addTeacher();
    void addStudent();
    void showTeacher();
    void showStudent();
    void deleteTeacher();
    void deleteStudent();
};
