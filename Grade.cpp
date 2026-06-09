#include "Grade.h"
#include <iostream>
Grade::Grade()
{
}
Grade::~Grade()
{
}
Grade::Grade(string name, double score, double credit)
{
	this->name = name;
	this->score = score;
	this->credit = credit;
	if (score >= 90) {
		gpa = 4.0;
	}
	else if (score >= 80) {
		gpa = 3.0;
	}
	else if (score >= 70) {
		gpa = 2.0;
	}
	else if (score >= 60) {
		gpa = 1.0;
	}
	else {
		gpa = 0.0;
	}
}
double Grade::getCredit() const
{
	return credit;
}
double Grade::getGPA() const
{
	return gpa;
}
