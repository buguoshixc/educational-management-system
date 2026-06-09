#include <string>
#include <iostream>
#include <vector>
#include <direct.h>
#include "User.h"
#include "Manager.h"
#include "Teacher.h"
#include "Student.h"
#include "Grade.h"
#include "InputHelper.h"
using namespace std;
const string stuPath = "data/students.txt";
void menu() {
    cout << "请输入你登录的身份：" << endl;
    cout << "1.管理员" << endl;
    cout << "2.教师" << endl;
    cout << "3.学生" << endl;
    cout << "4.退出" << endl;
    return;
}

int main()
{
    _mkdir("data");
    int choice;
    menu();
    while (true) {
        if (!tryCin(choice)) {
            cout << "无效输入，请重新选择" << endl;
            menu();
            continue;
        }
        switch (choice)
        {
        case 1:
        {
            cout << "你选择了管理员身份" << endl;
            cout << "请输入管理员用户名：" << endl;
            string name;
            readValid(name);
            cout << "请输入管理员密码：" << endl;
            string password;
            readValid(password);
            Manager* user = new Manager();
            if (!user->login(name, password)) {
                delete user;
                break;
            }
            user->menu();
            delete user;
            break;
        }
        case 2:
        {
            cout << "你选择了教师身份" << endl;
            cout << "请输入你的教职工号：" << endl;
            string id;
            readValid(id);
            cout << "请输入你的密码：" << endl;
            string password;
            readValid(password);
            Teacher* user = new Teacher();
            if (!user->login(id, password)) {
                delete user;
                break;
            }
            user->menu();
            delete user;
            break;
        }
        case 3:
        {
            cout << "你选择了学生身份" << endl;
            cout << "请输入你的学号：" << endl;
            string id;
            readValid(id);
            cout << "请输入你的密码：" << endl;
            string password;
            readValid(password);
            Student* user = new Student();
            if (!user->login(id, password)) {
                delete user;
                break;
            }
            vector<Student> allStudents;
            ifstream in(stuPath);
            if (in.is_open()) {
                string name, sid, password;
                while (in >> name >> sid >> password) {
                    Student stu(name, sid);
                    allStudents.push_back(stu);
                }
                in.close();
            }
            user->setAllStudents(&allStudents);
            user->menu();
            delete user;
            break;
        }
        case 4:
        {
            cout << "你选择了退出" << endl;
            return 0;
        }
        default:
            cout << "无效的选择" << endl;
        }
        menu();
    }

    return 0;
}
