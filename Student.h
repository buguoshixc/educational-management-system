#pragma once
#include <iostream>
#include <string>
#include<vector>
#include "User.h"
class Student : public User {
	double gpa=-1;
	std::string coursePath;
	std::string gradePath;
	std::string stuPath = "data/students.txt";
	const std::vector<Student>* allStudents = nullptr;
public:
	Student();
	~Student() override;
	Student(std::string name, std::string id);
	bool login(std::string id, std::string password);
	void setAllStudents(const std::vector<Student>* ptr) { allStudents = ptr; }
	void menu();
	void showCourseGrade();
	void showCourseManagment();
	void addCourse();
	void deleteCourse();
	void showRanking();
	double getGPA();
	double showGPA();
	bool operator==(const Student& s) const;
	bool operator>(const Student& s) const;

};
