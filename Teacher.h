#pragma once
#include <iostream>
#include <string>
#include "User.h"

class Teacher : public User
{
public:
	Teacher();
	~Teacher() override;
	Teacher(std::string name, std::string id);
	bool login(std::string id, std::string password);
	void menu();
	bool addStudentGrade(std::string stu_id, double score, std::string name, double credit);
	bool showStudentGrade(std::string stu_id);
	bool deleteStudentGrade(std::string stu_id, std::string courseName);
};
