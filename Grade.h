#pragma once
#include <iostream>
#include <string>
using std::string;
class Grade {
	string name;
	double score;
	double gpa;
	double credit;
public:
	Grade();
	Grade(string name, double score, double credit);
	~Grade();
	double getGPA() const;
	double getCredit() const;
};
