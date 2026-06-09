# Vibe Coding 协作记录

> 项目：教务管理系统（学生成绩管理）  
> 项目路径：`D:\OneDrive\Desktop\Code\c++\BigWork2\项目`  
> 协作时间线：需求设计 → 代码审查 → Bug 修复 → 架构重构 → 细节打磨

---

# 第一阶段：需求分析与系统设计

## 教务管理系统 — 功能规划

### 1. 管理员端

直接添加用户和密码，将账户密码用 IO 流写出，保存在 txt 文件中。

**补充建议：**

- **删除用户** — 有添加就应该有删除
- **修改用户信息**（如重置密码）
- **查看所有用户列表**
- **查看所有课程列表** — 审核老师添加的课程

### 2. 学生端

- 查看自己各科成绩和绩点
- 查看自己排名
- 查看课表安排
- 选课

**补充建议：**

- **退课** — 选了课就应该能退
- **查看已选课程** — 确认自己选了哪些课
- **修改密码**

### 3. 教师端

- 添加学生成绩
- 查看学生成绩
- 查看课表安排
- 添加课程 → 写出到 txt 文件中

**补充建议：**

- **修改学生成绩** — 成绩录错了需要能改
- **查看自己课程的学生名单** — 知道谁选了你的课
- **修改密码**

### 4. 数据结构设计（建议）

用结构体来组织数据，避免散落的变量：

```cpp
// 用户
struct User {
    string username;   // 用户名
    string password;   // 密码
    int role;          // 0=管理员, 1=学生, 2=教师
};

// 课程
struct Course {
    string courseId;    // 课程编号
    string courseName;  // 课程名称
    string teacherId;   // 授课教师用户名
    int credit;         // 学分
    string schedule;    // 上课时间地点，如 "周一 1-2节 教学楼A101"
    int capacity;       // 选课容量上限
    int enrolled;       // 已选人数
};

// 成绩
struct Grade {
    string studentId;   // 学生用户名
    string courseId;    // 课程编号
    double score;       // 百分制分数
    double gpa;         // 对应绩点
};
```

### 5. 文件存储设计

每种数据单独存一个文件，结构清晰：

| 文件名 | 存储内容 |
|---|---|
| `users.txt` | 用户名 密码 角色 |
| `courses.txt` | 课程编号 课程名 教师用户名 学分 上课时间 容量 已选人数 |
| `grades.txt` | 学生用户名 课程编号 分数 绩点 |
| `enrollments.txt` | 学生用户名 课程编号 |

每条记录占一行，字段之间用空格分隔，方便用 `ifstream` / `ofstream` 读写。

### 6. 程序整体结构

```
启动 → 登录（验证用户名密码）
  ├── 管理员菜单
  │   ├── 添加用户
  │   ├── 删除用户
  │   ├── 修改用户信息
  │   ├── 查看所有用户
  │   └── 查看所有课程
  ├── 学生菜单
  │   ├── 查看成绩和绩点
  │   ├── 查看排名
  │   ├── 查看课表
  │   ├── 选课
  │   ├── 退课
  │   ├── 查看已选课程
  │   └── 修改密码
  └── 教师菜单
      ├── 添加/录入学生成绩
      ├── 修改学生成绩
      ├── 查看学生成绩
      ├── 查看课表
      ├── 查看课程学生名单
      ├── 添加课程
      └── 修改密码
```

每个功能写成一个函数，运行时所有数据从文件读入内存（用 `vector` 存储），操作完再写回文件。

### 7. 逻辑细节提醒

- **绩点换算规则**要提前定好。如：90-100→4.0，85-89→3.7，80-84→3.3 等。
- **排名规则**：按平均绩点从高到低，同分同名次。
- **选课冲突检查**：选课时需检查上课时间是否与已选课程冲突。
- **容量检查**：选课时需检查 `enrolled < capacity`，满员则不能选。
- **管理员预设**：首次运行系统时至少硬编码一个管理员账户（如 admin / admin123），否则无法登录。
- **输入验证**：菜单选择、成绩范围（0-100）等要做合法性检查。

### 8. 暂时可以忽略的内容

- 图形界面 — 控制台菜单完全够用
- 数据库 — txt 文件对大作业足够
- 网络/并发 — 不需要

---

# 第二阶段：代码审查（2026-05-22）

## 优先级定义

| 级别 | 含义 |
|------|------|
| P0 | 必须修复，否则功能异常或数据丢失 |
| P1 | 应该修复，影响程序正确性或代码质量 |
| P2 | 建议改进，锦上添花 |

---

## P0 — 必须修复

### 1. Teacher 构造函数每次都会清空课程文件

**文件：** `Teacher.cpp:13-16`

```cpp
Teacher::Teacher(string name, string id) : User(name, id) {
    this->path = "data\\" + name + "\\course.txt";
    ofstream out(path);  // 默认模式打开，直接清空文件内容
}
```

**问题：** `ofstream` 默认打开模式会清空已有文件。每次教师登录，之前存储的课程数据全部丢失。

**修复方向：** 去掉构造函数中的 `ofstream out(path);` 这一行，文件在需要写入时再以 `ios::app` 追加模式打开即可。如果担心文件不存在导致后续读取出错，可以在读取时判断 `is_open()` 后给提示。

### 2. getGPA() 每次都往 students.txt 追加重复数据

**文件：** `Student.cpp:81-83`

```cpp
ofstream out(stuPath, ios::app);
if (out.is_open()) out << name << " " << id << " " << ret / totalCredits << endl;
```

**问题：** 每次查看 GPA 都会往 `students.txt` 追加一行记录。多次查询后文件不断膨胀，同一学生的记录重复出现。

**修复方向：** 不应该在 `getGPA()` 中写文件。如果确实需要将 GPA 持久化到 students.txt，应该用一个独立的更新函数，并按学号去重替换而非追加。

### 3. Student::operator< 不是 const 成员函数

**文件：** `Student.h:28`

```cpp
bool operator<(const Student& s);  // 缺少 const 修饰
```

**问题：** `std::sort` 要求比较运算符是 const 的。当前写法在调用 `sort(v.begin(), v.end())` 时会编译失败（取决于编译器严格程度）。

**修复方向：** 改为 `bool operator<(const Student& s) const;`，同时将 `.cpp` 中的实现也加上 `const`。

### 4. Manager.cpp 中文编码乱码

**文件：** `Manager.cpp`

整个文件的中文字符串全部显示为乱码（如 `"��������Ҫִ�еĲ�����"`）。

**问题：** 文件以 GBK/GB2312 编码保存，而当前编译环境按 UTF-8 解析，导致运行时输出乱码。

**修复方向：** 在 Visual Studio 中：文件 → 高级保存选项 → 编码选择 "Unicode (UTF-8 无签名)" → 保存。VS2022 也可以在 工具 → 选项 → 环境 → 文档 中勾选"以 UTF-8 保存文档"。

### 5. 教师登录没有验证身份

**文件：** `main.cpp:63`

```cpp
User* user = new Teacher(name, id);  // 任何 name 和 id 都能直接进入
```

**问题：** 管理员登录调用了 `login()` 验证用户名密码，学生登录至少加载了已有学生数据，但教师端完全没有任何验证——随意输入即可登录。

**修复方向：** 参照 `Manager::login()` 的逻辑，在教师登录时读取 `data/teacher.txt` 验证工号和姓名是否匹配。

---

## P1 — 应该修复

### 6. Student 类存储了全体学生副本 vector\<Student\> v

**文件：** `Student.h:13`

```cpp
vector<Student> v;  // 每个学生对象都持有一份全体学生列表
```

**问题：**

- 是一种循环依赖设计——Student 包含 vector\<Student\>，每个元素又包含 vector\<Student\>...
- `main()` 中通过值传递 `vs`，导致大量不必要拷贝
- 一个学生对象不需要知道所有其他学生

**修复方向：** 将排名功能的排序逻辑移到外部（例如在 `main.cpp` 中排序），学生只持有自己的数据。`showRanking()` 改为接受 `const vector<Student>&` 参数。

### 7. using 声明写在头文件中

**文件：** `Student.h:6-7`, `Teacher.h:4-5`, `Grade.h:3-4`, `Manager.h`

```cpp
// Student.h
using std::string;
using std::vector;
```

**问题：** 头文件中引入 using 声明会污染所有包含此头文件的编译单元，可能导致命名冲突。

**修复方向：** 头文件中直接使用完全限定名 `std::string`、`std::vector`。using 声明只放在 `.cpp` 文件中。

### 8. 文件打开失败后没有阻止后续操作

**文件：** 多处

```cpp
ifstream in(coursePath);
if (!in.is_open()) {
    cout << "文件打开失败" << endl;
    return;  // 这里 return 了，但有些地方没有
}
```

**问题：** 某些函数在 `ifstream` 打开失败后继续执行后续的文件操作，可能导致未定义行为。

**修复方向：** 确保每个文件操作的失败分支都有 `return` 或类似的控制流跳过后面的代码。

### 9. 硬编码 Windows 路径分隔符

**文件：** 多处

```cpp
string stuPath = "data\\students.txt";
string gradePath = "data\\" + id + "_Grade.txt";
```

**问题：** `\\` 是 Windows 专有的路径分隔符，不能在 Linux/macOS 上运行。用 `/` 可以在所有平台上正常工作。

**修复方向：** 将 `\\` 改为 `/`。Windows API 和 C++ 标准库都接受 `/`。

### 10. getGPA() 中局部变量 gpa 与成员变量同名

**文件：** `Student.cpp:73`

```cpp
double gpa = 0;  // 局部变量，与类成员 Student::gpa 同名
```

**问题：** 虽然不影响正确性，但容易在后续维护中产生混淆和 bug（开发者可能以为在修改成员变量）。

**修复方向：** 将局部变量改名，例如 `double courseGpa = 0;`。

---

## P2 — 建议改进

### 11. 课程添加/删除没有与实际课程库关联

**文件：** `Student.cpp:128-185`

学生可以随意输入任何课程名称进行添加或删除，没有和教师端开设的课程数据做关联。

**建议：** 维护一个公共的课程文件（如 `data/course.txt`），学生只能从已开设课程中选择。教师端增加开设课程的功能，写入该公共文件。

### 12. 教师不能修改已录入的成绩

**文件：** `Teacher.cpp:82-88`

`addStudentGrade()` 始终以 `ios::app` 追加模式写入。同一门课录入两次会变成两条记录而非更新。

**建议：** 录入前先检查该学生该课程是否已有成绩，有则更新，无则追加。

### 13. 缺少输入有效性检查

**文件：** 所有接受 `cin >>` 的地方

输入非法字符（如字母输入到 int 类型变量）会导致 cin 进入失败状态。

**建议：** 参照 `main()` 中已有的输入检查逻辑，在各菜单输入处增加 `cin.fail()` 判断和清理。

### 14. 密码明文存储

**文件：** `data/user.txt`, `data/teacher.txt`, `data/student.txt`

所有密码以明文形式存放在 txt 文件中。

**建议（不强制）：** 作为大作业的加分项，可以使用简单的哈希算法（如 MD5）对密码做一层保护。

### 15. 缺少 `virtual` 析构函数的多态保障

**文件：** `User.h:12`

基类 `User` 已经有 `virtual ~User()`，这点是正确的。但派生类 `Manager`、`Teacher`、`Student` 的析构函数可以加上 `override` 关键字以增强可读性：

```cpp
~Manager() override;
~Teacher() override;
~Student() override;
```

---

## 修复顺序建议

```
P0 修复（确保程序正常运行）
  └→ 1. Teacher 构造函数清空文件
  └→ 2. Manager.cpp 编码乱码
  └→ 3. operator< 缺少 const
  └→ 4. 教师登录无验证
  └→ 5. getGPA 重复写入

P1 修复（提升代码质量）
  └→ 6. vector<Student> v 移出 Student 类
  └→ 7. 头文件中去掉 using 声明
  └→ 8. 文件打开失败的错误处理
  └→ 9. 路径分隔符改为 /
  └→ 10. 变量命名冲突

P2 改进（加分项）
  └→ 11~15. 功能增强 & 细节优化
```

---

# 第三阶段：致命 Bug 修复 + 多态重构

## 一、致命 Bug 修复

### 1. `Student.cpp` — 赋值运算符误写为比较 (`=` vs `==`)

```cpp
// 修改前 (第56行)
if (gpa = -1) gpa=getGPA();   // 赋值表达式永远为 true
// 修改后
if (gpa == -1) gpa=getGPA();
```

### 2. `Student.cpp` — `getGPA()` 局部变量未初始化 + 未除以总学分

```cpp
// 修改前 (第64行)
double gpa;            // 未初始化，值是垃圾数据
// ...
ret += gpa * credit;   // 使用垃圾值
return ret;            // 返回加权总分而非平均绩点

// 修改后
double gpa = 0;
double totalCredits = 0;
while (...) {
    ret += gpa * credit;
    totalCredits += credit;
}
return totalCredits > 0 ? ret / totalCredits : 0;
```

### 3. `Teacher.h` — 成员 `path` 初始化时机错误

`path` 在 `name` 为空字符串时就拼接完成，构造函数体中的赋值无法改变它。

```cpp
// 修改前 (Teacher.h 第10行)
string path = "data\\" + name + "\\course.txt";  // name 此时为空

// 修改后 (Teacher.h)
string path;  // 只声明，不初始化
// Teacher.cpp 构造函数体中
this->path = "data\\" + name + "\\course.txt";
```

### 4. `Student.cpp` / `Teacher.cpp` — 成绩文件名不一致

教师写入 `_Grade.txt`（单数），学生读取 `_Grades.txt`（复数），导致学生永远读不到成绩。

```
修改前: Teacher 写 → _Grade.txt  /  Student 读 → _Grades.txt
修改后: 统一为 _Grade.txt
```

### 5. `Teacher.cpp` — 成绩文件读写字段数不匹配

```cpp
// 修改前 — 写入4个字段
out << name << " " << score << " " << credit << " " << grade.getGPA() << endl;
// 读取只读3个字段
while (in >> name >> score >> credit)  // 少读了 gpa

// 修改后 — 读写都处理4个字段
while (in >> name >> score >> credit >> tmp)
```

### 6. `Student.cpp` — 构造函数中调用 `getGPA()` 时 `gradePath` 尚未赋值

```cpp
// 修改前
Student(...) {
    this->gpa = this->getGPA();                        // gradePath 还是空字符串
    this->gradePath = "data\\" + id + "_Grade.txt";    // 赋值得太晚
}

// 修改后
Student(...) {
    this->gradePath = "data\\" + id + "_Grade.txt";    // 先设置路径
    this->gpa = this->getGPA();                        // 再读取
}
```

---

## 二、稳健性改进

### 7. `main.cpp` — 自动创建 `data/` 目录

```cpp
#include <direct.h>
// main() 开头
_mkdir("data");
```

### 8. `main.cpp` — 修复非法输入导致死循环

```cpp
// 修改前
while (cin >> choice) {   // 输入字母会导致 cin 永久失败，死循环
// 修改后
while (true) {
    if (!(cin >> choice)) {
        cin.clear();                  // 清除错误状态
        cin.ignore(10000, '\n');      // 丢弃错误输入
        cout << "无效输入，请重新选择" << endl;
        menu();
        continue;
    }
```

### 9. `Student.cpp` — 补全文件打开检查

以下 5 个函数中打开文件后新增 `is_open()` 判断：

| 函数 | 文件操作 |
|------|----------|
| `getGPA()` | `ifstream in(gradePath)` |
| `showCourseGrade()` | `ifstream in(gradePath)` |
| `showCourseManagment()` | `ifstream in(coursePath)` |
| `addCourse()` | `ofstream out(coursePath, ios::app)` |
| `deleteCourse()` | `ifstream in(coursePath)` |

---

## 三、多态架构重构

### 10. 新增 `User` 抽象基类

**User.h:**

```cpp
class User {
protected:
    string name;
    string id;
public:
    User();
    User(string name, string id);
    virtual ~User();
    virtual void menu() = 0;  // 纯虚函数
};
```

**继承关系:**

```
         User (抽象基类)
           |
    ┌──────┼──────┐
    |      |      |
  Manager Teacher Student
```

### 11. 改造三个派生类

- `Teacher.h` / `Student.h` / `Manager.h`: 改为 `class Xxx : public User`
- `Teacher.h` / `Student.h`: 删除 `name` 和 `id` 成员（从基类继承）
- `Teacher.cpp` / `Student.cpp`: 构造函数改用初始化列表 `: User(name, id)`
- `Manager.h`: 新增继承，保留 `login()` 等特有方法

### 12. `main.cpp` — 多态调用

```cpp
// 统一使用基类指针操作
User* user = new Teacher(name, id);
user->menu();       // 运行时动态绑定到 Teacher::menu()
delete user;
```

管理员登录失败由 `return 0`（直接退出程序）改为 `break`（返回主菜单）。

---

## 四、代码质量

### 13. `Student.cpp` — 删除重复路径拼接

```cpp
// 修改前 — 手动重建与 coursePath 完全相同的字符串
string oldPath = "data\\" + id + "_Courses.txt";
remove(oldPath.c_str());
rename("data\\temp.txt", oldPath.c_str());

// 修改后 — 直接使用成员变量
remove(coursePath.c_str());
rename("data\\temp.txt", coursePath.c_str());
```

---

## 文件变更总览（第三阶段）

| 文件 | 状态 |
|------|------|
| `User.h` | 新增 |
| `User.cpp` | 新增 |
| `main.cpp` | 修改 |
| `Manager.h` | 修改 |
| `Student.h` | 修改 |
| `Student.cpp` | 修改 |
| `Teacher.h` | 修改 |
| `Teacher.cpp` | 修改 |
| `Grade.h` | 未改动 |
| `Grade.cpp` | 未改动 |
| `Manager.cpp` | 未改动 |

---

# 第四阶段：语义修正与深度分析（2026-06-08）

## 一、`operator<` 语义颠倒问题 ✅ 已修复

### 问题描述

`Student` 类重载了 `operator<`，但内部逻辑与运算符语义完全相反。

### 原始代码

**`Student.h` 第 28 行：**
```cpp
bool operator<(const Student& s) const;
```

**`Student.cpp` 第 230–232 行：**
```cpp
bool Student::operator<(const Student& s) const{
    return this->gpa > s.gpa;   // ← 用 > 实现 <，语义颠倒
}
```

### 唯一调用处

**`Student.cpp` 第 222 行：**
```cpp
sort(v.begin(), v.end());   // 默认升序，调用 operator<
```

### 问题分析

| 层面 | 实际情况 | 问题 |
|------|---------|------|
| 语义 | `a < b` 意为 "a 的 GPA **大于** b" | `<` 和 `>` 含义相反 |
| `sort` 行为 | 默认升序："小的"在前 | 升序 + 反逻辑 `<` = 碰巧降序 |
| 结果 | zhangsan(4.0) → lisi(3.0) → wangwu(2.0) | 排名正确，但原因错误 |

**"两个错误互相抵消"：**

```
错误①: operator< 用 > 实现（降序逻辑装在升序壳子里）
错误②: sort 默认升序（把 operator< 认为"小"的放前面）
─────────────────────────────────────────────────────
错误① + 错误② = 碰巧得到降序结果，排名看起来"对"
```

**隐患**：如果有人把 `operator<` "修复"为正确的 `this->gpa < s.gpa`，而忘记同步修改 `sort` 调用，排名就会变成升序（GPA 最低的排第 1），结果就是错的。

### 修改方案

将颠倒语义的 `operator<` 替换为语义正确的 `operator>`，`showRanking()` 中显式使用 `greater<Student>()` 表达降序意图。

### 修改后代码

**`Student.h` 第 28 行：**
```cpp
bool operator>(const Student& s) const;   // > 就是 >，语义正确
```

**`Student.cpp` 第 231–232 行：**
```cpp
bool Student::operator>(const Student& s) const{
    return this->gpa > s.gpa;    // "我的GPA大于别人的GPA" → > 返回 true，完全符合直觉
}
```

**`Student.cpp` 第 223 行：**
```cpp
sort(v.begin(), v.end(), greater<Student>());   // 显式降序：GPA 高的在前
```

### 修改前后对比

| | 修改前 | 修改后 |
|------|--------|--------|
| 比较运算符 | `operator<` 返回 `this->gpa > s.gpa` | `operator>` 返回 `this->gpa > s.gpa` |
| 排序调用 | `sort(v.begin(), v.end())` | `sort(v.begin(), v.end(), greater<>())` |
| `<` 语义 | "a < b" = a 的 GPA 大于 b | `<` 未定义（暂不需要） |
| `>` 语义 | `>` 未定义 | "a > b" = a 的 GPA 大于 b |
| 可读性 | 读到 `<` 却是 `>` 逻辑，困惑 | 代码意图一目了然 |

---

## 二、`Student` 对象冗余持有全体学生列表

### 问题描述

`Student` 类有一个成员变量 `vector<Student> v`，存放**全体学生**的副本。每个 `Student` 对象都携带一份完整的全班学生数据。

### 相关代码

**`Student.h` 第 11 行：**
```cpp
std::vector<Student> v;   // 每个 Student 对象都存一份全体学生
```

**`main.cpp` 第 85–95 行（数据灌入）：**
```cpp
vector<Student> vs;
ifstream in(stuPath);
if (in.is_open()) {
    string name, sid, password;
    while (in >> name >> sid >> password) {
        Student stu(name, sid);
        vs.push_back(stu);
    }
    in.close();
}
user->setStudentVector(vs);   // 把整个 vs 深拷贝进当前 Student 对象
```

**`Student.cpp` 第 39–42 行：**
```cpp
void Student::setStudentVector(const std::vector<Student>& vec)
{
    this->v = vec;   // 深拷贝：vec 里有多少学生，v 里就复制多少
}
```

### 问题分析

| 层面 | 问题 |
|------|------|
| **内存** | N 个学生 → 每个持有 N 个学生的副本 → O(N²) 内存 |
| **设计** | "全班学生列表"是系统级数据，不应该作为单个学生的成员变量 |
| **数据一致性** | `v` 在登录时快照一次，之后管理员增删学生或成绩变化，`v` 不会更新 |
| **职责混乱** | `Student` 的职责是描述一个学生，不应该"拥有"其他学生 |

### 改进建议（第四阶段分析时提出，第五阶段实施）

将 `v` 从成员变量中移除，改为 `showRanking()` 接收外部传入的 `const vector<Student>&` 引用：

```cpp
// 更合理的设计
void Student::showRanking(const std::vector<Student>& allStudents) {
    vector<Student> sorted = allStudents;   // 拷贝一份用于排序
    sort(sorted.begin(), sorted.end(), greater<Student>());
    int rank = find(sorted.begin(), sorted.end(), *this) - sorted.begin() + 1;
    // ...
}
```

这样子 `Student` 不再"拥有"全体学生数据，排名计算时从外部"借用"，职责更加纯粹。

---

## 三、`showRanking()` 就地排序破坏原数据

### 问题描述

`showRanking()` 中 `sort()` 直接对 `this->v` 排序，会永久改变 `v` 的元素顺序。

### 当前代码

```cpp
sort(v.begin(), v.end(), greater<Student>());   // 直接改写了 this->v
```

### 隐患

目前没有其他代码依赖 `v` 的原始顺序，所以不构成 bug。但如果将来有人依赖 `v` 的顺序（比如与 `main.cpp` 中读取的顺序一致），就会出问题。

### 建议修改

排序前先拷贝一份副本：

```cpp
vector<Student> sorted = v;                                    // 副本
sort(sorted.begin(), sorted.end(), greater<Student>());       // 排序副本
int rank = find(sorted.begin(), sorted.end(), *this) - sorted.begin() + 1;
```

---

## 四、缺少 `<functional>` 头文件 ✅ 已修复

### 问题描述

使用 `std::greater` 需要 `#include <functional>`，否则不保证编译通过。

### 修改

在 `Student.cpp` 第 8 行添加：
```cpp
#include <functional>
```

---

## 涉及修改的文件清单（第四阶段）

| 文件 | 修改内容 |
|------|---------|
| `Student.h` 第 28 行 | `operator<` → `operator>` |
| `Student.cpp` 第 8 行 | 新增 `#include <functional>` |
| `Student.cpp` 第 223 行 | `sort(v.begin(), v.end())` → `sort(v.begin(), v.end(), greater<>())` |
| `Student.cpp` 第 231–232 行 | `operator<` → `operator>`，语义不变 (`this->gpa > s.gpa`) |

---

## 其他观察（本阶段提出，后续阶段部分实施）

### 密码明文存储

所有密码以明文形式存储在 txt 文件中（`user.txt`、`teacher.txt`、`students.txt`），实际项目中应使用哈希存储。

### Windows 路径硬编码

代码中使用 `\\` 作为路径分隔符（如 `"data\\students.txt"`），跨平台兼容性差。可使用 `/` 或条件编译。

### `operator==` 仅比较学号

```cpp
bool Student::operator==(const Student& s) const {
    return this->id == s.id;
}
```

这使得 `find()` 仅凭学号匹配，忽略了 GPA 可能不同的情况。在排名场景中这恰巧是合理的（同一个人的 GPA 变化了仍能找到），但属于隐式假设，建议加注释说明。

---

# 第五阶段：输入抽象与编码修复（2026-06-08 当晚）

## 修改一：cin 输入验证抽象

### 背景

项目中原有 27 处 `cin >>` 调用，仅 `main.cpp` 中的菜单选择有输入验证（`cin.clear()` + `cin.ignore()`），其余均无保护，输入非预期类型会导致程序死循环。

### 方案

新建 `InputHelper.h`，提供两个模板函数：

| 函数 | 用途 | 使用场景 |
|---|---|---|
| `tryCin(T& value)` | 尝试读取，返回 bool，调用者自行控制流程 | `main.cpp` 菜单选择（失败需重显菜单） |
| `readValid(T& value, ...)` | 循环重试直到输入合法 | 其他所有 `cin >>` 调用 |

### 改动文件

| 文件 | 改动 | 说明 |
|---|---|---|
| `InputHelper.h` | **新建** | 两个模板函数 |
| `main.cpp` | 7 处 | 菜单选择用 `tryCin`，其余用 `readValid` |
| `Student.cpp` | 5 处 | 全部改用 `readValid` |
| `Teacher.cpp` | 6 处 | 全部改用 `readValid` |
| `Manager.cpp` | 9 处 | 全部改用 `readValid` |

所有 27 处 `cin >>` 现在均有输入保护。

---

## 修改二：Manager.cpp 编码修复

### 问题

`Manager.cpp` 原为 GBK 编码，经过多次编码转换后中文全部变为乱码（Unicode 替换字符 U+FFFD），完全无法阅读。

### 方案

原始 GBK 字节已丢失无法逆向恢复，通过代码上下文逐行推断每个字符串的原始含义，手动重写为正确中文，以 UTF-8 保存。

### 修复量

| 类别 | 数量 |
|---|---|
| 字符串 | 36 处 |
| 注释 | 11 处 |
| **合计** | **47 处** |

### 典型修复示例

```
- cout << "1.���ӽ�ʦ" << endl;       →  cout << "1.添加教师" << endl;
- cout << "�ļ���ʧ��" << endl;        →  cout << "文件打开失败" << endl;
- // ɾ��ԭ�ļ�                        →  // 删除原文件
```

修复后文件为干净 UTF-8 编码，321 个中文字符均正常显示。

---

# 第六阶段：架构最终打磨（2026-06-08 当晚第二轮）

> 日期：2026-06-08

## 修改一：`Student` 移除冗余的全体学生副本

### 问题

`Student` 类持有一个 `vector<Student> v` 成员，存放全班学生的深拷贝副本。每个学生登录时都复制一份，造成 O(N²) 内存浪费，且数据在登录时快照后不再更新，职责混乱（学生不应"拥有"全班同学）。

### 方案

将 `vector<Student> v`（深拷贝副本）改为 `const vector<Student>* allStudents = nullptr`（指针借用）。

- `main()` 持有 `vector<Student> allStudents`，通过 `setAllStudents(&allStudents)` 传递给 `Student` 一个指针
- `showRanking()` 从指针取数据，先拷贝到局部 `sorted` 再排序，不破坏原数据顺序

### 改动文件

| 文件 | 改动 |
|------|------|
| `Student.h:11` | `vector<Student> v` → `const vector<Student>* allStudents = nullptr` |
| `Student.h:15-17` | 删除三参数构造函数 + `setStudentVector`，替换为内联 `setAllStudents(ptr)` |
| `Student.cpp:40-48` | 删除 `setStudentVector` 实现 + 三参数构造函数实现 |
| `Student.cpp:208-217` | `showRanking()`：指针判空 → `vector<Student> sorted = *allStudents` → 对副本排序 |
| `main.cpp:85-95` | `vs` → `allStudents`，`setStudentVector(vs)` → `setAllStudents(&allStudents)` |

### 效果

| | 修改前 | 修改后 |
|------|--------|--------|
| Student 持有数据量 | N 个完整 Student 对象 | 1 个指针（8 字节） |
| 内存复杂度 | O(N²) | O(N) |
| 排序副作用 | 永久改变成员 `v` | 只在局部副本上排序 |
| 数据所有权 | 每个 Student 各持一份 | 系统（main）持有一份 |

---

## 修改二：派生类析构函数加 `override`

| 文件 | 改前 | 改后 |
|------|------|------|
| `Manager.h:11` | `~Manager();` | `~Manager() override;` |
| `Teacher.h:10` | `~Teacher();` | `~Teacher() override;` |
| `Student.h:14` | `~Student();` | `~Student() override;` |

基类 `User.h:12` 保持 `virtual ~User();` 不变。

---

## 修改三：路径分隔符 `\\` → `/`

Windows 反斜杠路径改为正斜杠，Windows API 和 C++ 标准库均可正确处理 `/`，同时提升跨平台兼容性。

| 文件 | 替换处数 |
|------|---------|
| `Student.h` | 1 |
| `Student.cpp` | 7 |
| `Teacher.cpp` | 3 |
| `Manager.cpp` | 15 |
| `main.cpp` | 1 |
| **合计** | **27** |

示例：

```cpp
// 修改前
"data\\students.txt"
"data\\" + id + "_Grade.txt"

// 修改后
"data/students.txt"
"data/" + id + "_Grade.txt"
```

---

## 全部改动文件清单（第六阶段）

| 文件 | 修改内容 |
|------|---------|
| `Student.h` | `v` → `allStudents` 指针、析构函数 `override`、`\\` → `/` |
| `Student.cpp` | 删除深拷贝相关代码、`showRanking()` 重构、`\\` → `/` |
| `main.cpp` | `allStudents` 由 main 持有、传指针、`\\` → `/` |
| `Manager.h` | 析构函数 `override` |
| `Teacher.h` | 析构函数 `override` |
| `Manager.cpp` | `\\` → `/`（15 处） |
| `Teacher.cpp` | `\\` → `/`（3 处） |

---

## 讨论但未实施（第六阶段）

- **教师成绩去重更新**：讨论了将 `addStudentGrade()` 从追加模式改为"读取→匹配课程名→替换或新增→覆盖写回"的方案，用户决定暂不修改。

---

# 附录：协作全过程时间线总览

```
第一阶段：需求分析与系统设计
  └── 功能规划、数据结构设计、文件存储方案、程序流程图

第二阶段：代码审查（2026-05-22）
  └── P0 致命问题 5 项 + P1 代码质量 5 项 + P2 建议改进 5 项

第三阶段：致命 Bug 修复 + 多态重构
  └── 修复 6 个致命 Bug（赋值/=、未初始化、文件名不一致、字段不匹配、初始化顺序）
  └── 新增 User 抽象基类，Manager/Teacher/Student 全部继承
  └── main.cpp 多态调用、自动创建 data 目录、输入死循环修复

第四阶段：语义修正与深度分析（2026-06-08）
  └── operator< → operator>（修复"两个错误互相抵消"的隐蔽问题）
  └── 分析 Student 冗余持有全体学生列表（O(N²) 内存问题）
  └── 添加 <functional> 头文件

第五阶段：输入抽象与编码修复（2026-06-08 当晚）
  └── 新建 InputHelper.h，27 处 cin >> 全部纳入保护
  └── Manager.cpp 编码修复（47 处乱码手动重写为 UTF-8 中文）

第六阶段：架构最终打磨（2026-06-08 当晚第二轮）
  └── vector<Student> v → const vector<Student>*（O(N²)→O(N)）
  └── 派生类析构函数加 override
  └── 路径分隔符 \\ → / （27 处）
```

---

> 📝 这份文档记录了从需求分析到代码审查、从 Bug 修复到架构重构、从细节打磨到最终完善的完整协作过程。每一次修改都建立在前一阶段的分析和讨论之上，逐步将一个有多个致命 Bug 的原型打磨为结构清晰、行为正确的教务管理系统。
