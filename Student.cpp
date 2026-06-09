#include "Student.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <functional>
#include "InputHelper.h"
using namespace std;
Student::Student() {
}
Student::~Student() {
}
bool Student::login(std::string id, std::string password)
{
	std::ifstream in(stuPath);
	if (!in.is_open())
	{
		std::cout << "文件打开失败" << std::endl;
		return false;
	}
	std::string name, sid, spwd;
	while (in >> name >> sid >> spwd)
	{
		if (id == sid && password == spwd)
		{
			this->name = name;
			this->id = id;
			this->gradePath = "data/" + id + "_Grade.txt";
			this->coursePath = "data/" + id + "_Courses.txt";
			this->gpa = this->getGPA();
			std::cout << "登录成功" << std::endl;
			return true;
		}
	}
	std::cout << "学号或密码错误" << std::endl;
	return false;
}
Student::Student(string name, string id) : User(name, id) {
	this->gradePath = "data/" + id + "_Grade.txt";
	this->gpa = this->getGPA();
	this->coursePath = "data/" + id + "_Courses.txt";
}
void Student::menu() {
	int choice = 0;
	while (true) {
		cout << "====== 学生菜单 ======" << endl;
		cout << "1. 查看GPA" << endl;
		cout << "2. 查看课程成绩" << endl;
		cout << "3. 查看课程管理" << endl;
		cout << "4. 添加课程" << endl;
		cout << "5. 删除课程" << endl;
		cout << "6. 查看排名" << endl;
		cout << "0. 退出" << endl;
		cout << "请输入选项：" << endl;
		readValid(choice, "无效选项，请重新输入：");

		switch (choice) {
		case 1:
			cout << "你的GPA为：" << showGPA() << endl;
			break;
		case 2:
			showCourseGrade();
			break;
		case 3:
			showCourseManagment();
			break;
		case 4:
			addCourse();
			break;
		case 5:
			deleteCourse();
			break;
		case 6:
			showRanking();
			break;
		case 0:
			return;
		default:
			cout << "无效选项，请重新输入" << endl;
			break;
		}
	}
}
double Student::showGPA() {
	if (gpa == -1) gpa = getGPA();
	return gpa;
}
double Student::getGPA() {
	ifstream in(gradePath);
	if (!in.is_open()) {
		return 0;
	}
	string courseName;
	double score;
	double credit;
	double courseGpa = 0;
	double weightedSum = 0;
	double totalCredits = 0;
	while (in >> courseName >> score >> credit >> courseGpa) {
		weightedSum += courseGpa * credit;
		totalCredits += credit;
	}
	if (totalCredits == 0) return 0;
	return weightedSum / totalCredits;
}
void Student::showCourseGrade() {
	ifstream in(gradePath);
	if (!in.is_open()) {
		cout << "文件打开失败" << endl;
		return;
	}
	string courseName;
	double score;
	double credit;
	double gpa;
	bool flag = false;
	while (in >> courseName >> score >> credit >> gpa)
	{
		flag = true;
		cout << "课程名称：" << courseName << endl;
		cout << "成绩：" << score << endl;
		cout << "学分：" << credit << endl;
		cout << "绩点：" << gpa << endl;
	}
	if (!flag) cout << "成绩为空" << endl;
 	return;
}
void Student::showCourseManagment() {
	ifstream in(coursePath);
	if (!in.is_open()) {
		cout << "文件打开失败" << endl;
		return;
	}
	string courseName;
	string time;
	string location;
	bool flag = false;
	while (in >> courseName >> time >> location)
	{
		flag = true;
		cout << "课程名称：" << courseName << endl;
		cout << "上课时间：" << time << endl;
		cout << "上课地点：" << location << endl;
	}
	if (!flag) cout << "课程为空" << endl;
	return;
}
void Student::addCourse() {
	ofstream out(coursePath, ios::app);
	if (!out.is_open()) {
		cout << "文件打开失败" << endl;
		return;
	}
	string courseName;
	string time;
	string location;
	cout << "请输入课程名称：" << endl;
	readValid(courseName);
	cout << "请输入上课时间：" << endl;
	readValid(time);
	cout << "请输入上课地点：" << endl;
	readValid(location);
	out << courseName << " " << time << " " << location << endl;
	cout << "课程添加成功" << endl;
	return;
}
void Student::deleteCourse() {
	ifstream in(coursePath);
	if (!in.is_open()) {
		cout << "文件打开失败" << endl;
		return;
	}
	string courseName;
	cout << "请输入要退选的课程名称：" << endl;
	readValid(courseName);
	ofstream out("data/temp.txt");
	if (!out.is_open())
	{
		cout << "文件打开失败" << endl;
		return;
	}
	string name, time, location;
	bool found = false;
	while (in >> name >> time >> location)
	{
		if (name == courseName)
		{
			found = true;
			continue;
		}
		out << name << " " << time << " " << location << endl;
	}
	in.close();
	out.close();
	if (found) {
		remove(coursePath.c_str());
		rename("data/temp.txt", coursePath.c_str());
		cout << "课程退选成功" << endl;
	}
	else {
		remove("data/temp.txt");
		cout << "未找到该课程" << endl;
	}
	return;
}
void Student::showRanking() {
	if (!allStudents || allStudents->empty()) {
		cout << "暂无排名数据" << endl;
		return;
	}
	vector<Student> sorted = *allStudents;
	sort(sorted.begin(), sorted.end(), greater<Student>());
	int rank = find(sorted.begin(), sorted.end(), *this) - sorted.begin() + 1;
	cout << "你的排名：第 " << rank << " / " << sorted.size() << " 名" << endl;
	cout << "你的GPA：" << gpa << endl;
}
bool Student::operator==(const Student& s) const {
	return this->id == s.id;
}
bool Student::operator>(const Student& s) const{
	return this->gpa > s.gpa;
}
