#include "Manager.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include "InputHelper.h"
using namespace std;
Manager::Manager()
{
}
Manager::~Manager()
{
}
void Manager::menu()
{
    int choice;
    while (true)
    {
        cout << "请输入需要执行的操作：" << endl;
        cout << "1.添加教师" << endl;
        cout << "2.添加学生" << endl;
        cout << "3.显示教师" << endl;
        cout << "4.显示学生" << endl;
        cout << "5.删除教师" << endl;
        cout << "6.删除学生" << endl;
        cout << "7.退出" << endl;
        readValid(choice, "无效选项，请重新输入：");
        switch (choice)
        {
        case 1:
            addTeacher();
            break;
        case 2:
            addStudent();
            break;
        case 3:
            showTeacher();
            break;
        case 4:
            showStudent();
            break;
        case 5:
            deleteTeacher();
            break;
        case 6:
            deleteStudent();
            break;
        case 7:
            return;
        default:
            cout << "无效的选择" << endl;
        }
    }
}
void Manager::addTeacher()
{
    string name, id, password;
    cout << "请输入教师姓名：" << endl;
    readValid(name);
    cout << "请输入教师编号：" << endl;
    readValid(id);

    // 查重
    ifstream check("data/teacher.txt");
    if (check.is_open()) {
        string tname, tid, tpwd;
        while (check >> tname >> tid >> tpwd) {
            if (tid == id) {
                cout << "该教师编号已存在，添加失败" << endl;
                return;
            }
        }
        check.close();
    }

    cout << "请输入教师密码：" << endl;
    readValid(password);

    ofstream out("data/teacher.txt", ios::app);
    if (!out.is_open())
    {
        cout << "文件打开失败" << endl;
        return;
    }
    out << name << " " << id << " " << password << endl;
    out.close();
    cout << "教师添加成功" << endl;
}
void Manager::addStudent()
{
    string name, id, password;
    cout << "请输入学生姓名：" << endl;
    readValid(name);
    cout << "请输入学生学号：" << endl;
    readValid(id);

    // 查重
    ifstream check("data/students.txt");
    if (check.is_open()) {
        string sname, sid, spwd;
        while (check >> sname >> sid >> spwd) {
            if (sid == id) {
                cout << "该学号已存在，添加失败" << endl;
                return;
            }
        }
        check.close();
    }

    cout << "请输入学生密码：" << endl;
    readValid(password);

    ofstream out("data/students.txt", ios::app);
    if (!out.is_open())
    {
        cout << "文件打开失败" << endl;
        return;
    }
    out << name << " " << id << " " << password << endl;
    out.close();
    cout << "学生添加成功" << endl;
}
void Manager::showTeacher()
{
    ifstream in("data/teacher.txt");
    if (!in.is_open())
    {
        cout << "文件打开失败" << endl;
        return;
    }
    string name, id, password;
    while (in >> name >> id >> password)
    {
        cout << "教师姓名：" << name << " 教师编号：" << id << " 教师密码：" << password << endl;
    }
    in.close();
}
void Manager::showStudent()
{
    ifstream in("data/students.txt");
    if (!in.is_open())
    {
        cout << "文件打开失败" << endl;
        return;
    }
    string name, id, password;
    while (in >> name >> id >> password)
    {
        cout << "学生姓名：" << name << " 学生学号：" << id << " 学生密码：" << password << endl;
    }
    in.close();
}
void Manager::deleteTeacher()
{
    string id;
    cout << "请输入要删除的教师编号：" << endl;
    readValid(id);
    ifstream in("data/teacher.txt");
    if (!in.is_open())
    {
        cout << "文件打开失败" << endl;
        return;
    }
    ofstream out("data/temp.txt");
    if (!out.is_open())
    {
        cout << "文件打开失败" << endl;
        return;
    }
    string name, tid, password;
    bool found = false;
    while (in >> name >> tid >> password)
    {
        if (tid == id)
        {
            found = true;
            continue;
        }
        out << name << " " << tid << " " << password << endl; 
    }
    in.close();
    out.close();
    if (found)
    {
        remove("data/teacher.txt");                 
        rename("data/temp.txt", "data/teacher.txt"); 
        cout << "教师删除成功" << endl;
    }
    else
    {
        remove("data/temp.txt");
        cout << "未找到该教师编号" << endl;
    }
}
void Manager::deleteStudent()
{
    string id;
    cout << "请输入要删除的学生学号：" << endl;
    readValid(id);
    ifstream in("data/students.txt");
    if (!in.is_open())
    {
        cout << "文件打开失败" << endl;
        return;
    }
    ofstream out("data/temp.txt");
    if (!out.is_open())
    {
        cout << "文件打开失败" << endl;
        return;
    }
    string name, sid, password;
    bool found = false;
    while (in >> name >> sid >> password)
    {
        if (sid == id)
        {
            found = true;
            continue; 
        }
        out << name << " " << sid << " " << password << endl; 
    }
    in.close();
    out.close();
    if (found)
    {
        remove("data/students.txt");
        rename("data/temp.txt", "data/students.txt");

        // 同时删除该学生的成绩文件和课程文件（孤儿文件清理）
        string gradePath = "data/" + id + "_Grade.txt";
        string coursePath = "data/" + id + "_Courses.txt";
        remove(gradePath.c_str());
        remove(coursePath.c_str());

        cout << "学生删除成功" << endl;
    }
    else
    {
        remove("data/temp.txt"); 
        cout << "未找到该学生学号" << endl;
    }
}
bool Manager::login(string name,string password)
{
    ifstream in("data/user.txt");
    if (!in.is_open())
    {
        cout << "文件打开失败" << endl;
        return false;
    }
    string username, userpassword;
    in >> username >> userpassword;
    if (name == username && password == userpassword) {
        cout << "登录成功" << endl;
        return true;
    }
    cout << "账号或密码错误" << endl;
    return false;

}
