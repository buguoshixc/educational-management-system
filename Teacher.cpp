#include "Teacher.h"
#include "Grade.h"
#include <iostream>
#include <string>
#include <fstream>
#include "InputHelper.h"
using namespace std;
Teacher::Teacher()
{
}
Teacher::~Teacher()
{
}
bool Teacher::login(std::string id, std::string password)
{
	std::ifstream in("data/teacher.txt");
	if (!in.is_open())
	{
		std::cout << "文件打开失败" << std::endl;
		return false;
	}
	std::string name, tid, tpwd;
	while (in >> name >> tid >> tpwd)
	{
		if (id == tid && password == tpwd)
		{
			this->name = name;
			this->id = id;
			std::cout << "登录成功" << std::endl;
			return true;
		}
	}
	std::cout << "教职工号或密码错误" << std::endl;
	return false;
}
Teacher::Teacher(string name, string id) : User(name, id) {
}
void Teacher::menu()
{
	int choice;
	while (true)
	{
		cout << "======老师菜单 ======" << endl;
		cout << "1. 添加学生成绩" << endl;
		cout << "2. 显示学生成绩" << endl;
		cout << "3. 删除学生成绩" << endl;
		cout << "4. 退出" << endl;
		readValid(choice, "无效选项，请重新输入：");
		switch (choice)
		{
		case 1: {
			string stu_id;
			double score;
			string name;
			double credit;
			cout << "请输入学生ID：" << endl;
			readValid(stu_id);
			cout << "请输入课程名称：" << endl;
			readValid(name);
			cout << "请输入课程学分：" << endl;
			readValid(credit);
			cout << "请输入学生成绩：" << endl;
			readValid(score);
			addStudentGrade(stu_id, score, name, credit);
			break;
		}
		case 2: {
			string stu_id;
			cout << "请输入学生ID：" << endl;
			readValid(stu_id);
			showStudentGrade(stu_id);
			break;
		}
		case 3: {
			string stu_id;
			string courseName;
			cout << "请输入学生ID：" << endl;
			readValid(stu_id);
			cout << "请输入要删除的课程名称：" << endl;
			readValid(courseName);
			deleteStudentGrade(stu_id, courseName);
			break;
		}
		case 4:
			return;
		default:
			cout << "无效选项，请重新输入" << endl;
		}
	}
}
bool Teacher::addStudentGrade(string stu_id, double score, string name,double credit)
{
	Grade grade(name, score, credit);
	string p = "data/" + stu_id + "_Grade.txt";
	ofstream out(p, ios::app);
	if (!out.is_open()) {
		cout << "文件打开失败，无法添加成绩" << endl;
		return false;
	}
	out << name << " " << score <<" "<<credit<<" "<<grade.getGPA()<< endl;
	return true;
}
bool Teacher::deleteStudentGrade(string stu_id, string courseName)
{
	string p = "data/" + stu_id + "_Grade.txt";
	ifstream in(p);
	if (!in.is_open()) {
		cout << "文件打开失败，学生成绩不存在" << endl;
		return false;
	}

	ofstream out("data/temp.txt");
	if (!out.is_open()) {
		cout << "文件打开失败" << endl;
		return false;
	}

	string name;
	double score, credit, gpa;
	bool found = false;
	while (in >> name >> score >> credit >> gpa) {
		if (name == courseName) {
			found = true;
			continue;             // 跳过匹配的课程
		}
		out << name << " " << score << " " << credit << " " << gpa << endl;
	}
	in.close();
	out.close();

	if (found) {
		remove(p.c_str());
		rename("data/temp.txt", p.c_str());
		cout << "成绩删除成功" << endl;
	} else {
		remove("data/temp.txt");
		cout << "未找到该课程的成绩" << endl;
	}
	return true;
}

bool Teacher::showStudentGrade(string stu_id)
{
	string p = "data/" + stu_id + "_Grade.txt";
	ifstream in(p);
	if (!in.is_open()) {
		cout << "文件打开失败，学生成绩不存在" << endl;
		return false;
	}
	string name;
	double score;
	double credit;
	double tmp;
	bool flag = false;
	while (in >> name >> score >> credit>>tmp)
	{
		flag = true;
		cout << "课程名称：" << name << endl;
		cout << "成绩：" << score << endl;
		cout << "学分：" << credit << endl;
	}
	if (!flag) cout << "学生成绩为空" << endl;
	return true;
}