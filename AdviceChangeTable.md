# 修改状态对照表

> 将 6 篇文档中所有问题和建议，逐条对照当前代码，标注是否已修改。  
> 检查日期：2026-06-08

---

## 一、代码审查报告（2026-05-22）— P0 致命问题

### P0-1: Teacher 构造函数每次都会清空课程文件

**原问题**：构造函数中有 `ofstream out(path);`，每次登录清空课程数据。

**当前代码**：
- `Teacher.h` 中已无 `path` 成员
- `Teacher.cpp` 构造函数 `Teacher::Teacher() {}` 无任何文件操作

**状态**：✅ 已修复

---

### P0-2: getGPA() 每次都往 students.txt 追加重复数据

**原问题**：`getGPA()` 中有 `ofstream out(stuPath, ios::app)`，每次查 GPA 追加一行。

**当前代码**：`Student.cpp:90-107`，`getGPA()` 只读取文件，无任何写操作。

**状态**：✅ 已修复

---

### P0-3: Student::operator< 不是 const 成员函数

**原问题**：`bool operator<(const Student& s);` 缺少 const，导致 sort 可能编译失败。

**当前代码**：`Student.h:26` → `bool operator>(const Student& s) const;`（改为语义正确的 operator>，且带 const）

**状态**：✅ 已修复

---

### P0-4: Manager.cpp 中文编码乱码

**原问题**：GBK 编码被按 UTF-8 解析，所有中文显示为 U+FFFD 乱码。

**当前代码**：`Manager.cpp` 321 个中文字符全部正常，UTF-8 编码。

**状态**：✅ 已修复

---

### P0-5: 教师登录没有验证身份

**原问题**：`User* user = new Teacher(name, id);` 直接进入，无身份验证。

**当前代码**：`main.cpp:62-66`
```cpp
Teacher* user = new Teacher();
if (!user->login(id, password)) {
    delete user;
    break;
}
```
`Teacher::login()` 从 `data/teacher.txt` 读取并验证工号和密码。

**状态**：✅ 已修复

---

## 二、代码审查报告（2026-05-22）— P1 应该修复

### P1-6: Student 类存储了全体学生副本

**原问题**：`vector<Student> v` 成员，每个 Student 深拷贝全量数据，O(N²) 内存。

**当前代码**：`Student.h:11` → `const std::vector<Student>* allStudents = nullptr;`（指针借用）

**状态**：✅ 已修复

---

### P1-7: using 声明写在头文件中

**原问题**：`Student.h`、`Teacher.h`、`Grade.h` 头文件中有 `using std::string;` 等，污染全局命名空间。

**当前代码**：

| 头文件 | 当前状态 |
|--------|----------|
| `Student.h` | 无 using 声明 ✅ |
| `Teacher.h` | 无 using 声明 ✅ |
| `Manager.h` | 无 using 声明 ✅ |
| `Grade.h:4` | **仍有** `using std::string;` ❌ |

**状态**：⚠️ 部分修复（Grade.h 遗漏）

---

### P1-8: 文件打开失败后没有阻止后续操作

**原问题**：部分函数打开文件失败后未 return，继续操作导致未定义行为。

**当前代码**：所有文件操作处均有 `is_open()` 检查和 `return` 分支：

| 函数 | 检查情况 |
|------|----------|
| `Student::getGPA()` | ✅ `if (!in.is_open()) return 0;` |
| `Student::showCourseGrade()` | ✅ `if (!in.is_open()) return;` |
| `Student::showCourseManagment()` | ✅ `if (!in.is_open()) return;` |
| `Student::addCourse()` | ✅ `if (!out.is_open()) return;` |
| `Student::deleteCourse()` | ✅ 两处均检查 |
| `Teacher::addStudentGrade()` | ✅ `if (!out.is_open()) return false;` |
| `Teacher::showStudentGrade()` | ✅ `if (!in.is_open()) return false;` |
| `Teacher::deleteStudentGrade()` | ✅ 两处均检查 |
| `Manager` 全部函数 | ✅ 均有检查 |

**状态**：✅ 已修复

---

### P1-9: 硬编码 Windows 路径分隔符

**原问题**：大量 `\\` 路径分隔符，不可跨平台。

**当前代码**：全部 27 处 `\\` 已改为 `/`。

**状态**：✅ 已修复

---

### P1-10: getGPA() 中局部变量与成员变量同名

**原问题**：`double gpa = 0;` 局部变量与 `Student::gpa` 成员同名。

**当前代码**：`Student.cpp:98` → `double courseGpa = 0;`（已改名）

**状态**：✅ 已修复

---

## 三、代码审查报告（2026-05-22）— P2 建议改进

### P2-11: 课程添加/删除没有与实际课程库关联

**原问题**：学生可随意输入任意课程名，不与教师开设的课程关联。

**当前代码**：`Student::addCourse()` 仍然直接追加到个人课程文件，无公共课程库校验。

**状态**：❌ 未修改

---

### P2-12: 教师不能修改已录入的成绩

**原问题**：`addStudentGrade()` 始终以 `ios::app` 追加，同一门课录入两次会变成两条记录。

**当前代码**：`Teacher.cpp:95` → 仍为 `ofstream out(p, ios::app);`，纯追加模式。

**状态**：❌ 未修改（第六阶段讨论后用户决定暂不改动）

---

### P2-13: 缺少输入有效性检查

**原问题**：除 `main.cpp` 外，所有 `cin >>` 无输入保护，输入字母会死循环。

**当前代码**：已新建 `InputHelper.h`，`tryCin<T>()` 和 `readValid<T>()` 覆盖全部 27 处输入。

**状态**：✅ 已修复

---

### P2-14: 密码明文存储

**原问题**：所有密码以明文存放在 txt 文件中。

**当前代码**：`data/user.txt`、`data/teacher.txt`、`data/students.txt` 中密码仍为明文。

**状态**：❌ 未修改（大作业场景够用，非必须）

---

### P2-15: 派生类析构函数缺少 override

**原问题**：`~Manager()`、`~Teacher()`、`~Student()` 未加 `override`。

**当前代码**：

| 文件 | 声明 |
|------|------|
| `Manager.h:11` | `~Manager() override;` ✅ |
| `Teacher.h:10` | `~Teacher() override;` ✅ |
| `Student.h:14` | `~Student() override;` ✅ |

**状态**：✅ 已修复

---

## 四、修改记录（第三阶段）— 致命 Bug 修复

### 3-1: 赋值运算符误写为比较 (`=` vs `==`) ✅

`Student.cpp:87` → `if (gpa == -1) gpa = getGPA();`（正确使用 `==`）

---

### 3-2: getGPA() 局部变量未初始化 + 未除以总学分 ✅

- `double courseGpa = 0;`（初始化为 0）
- 增加 `totalCredits` 累加
- `return weightedSum / totalCredits;`（正确计算加权平均）

---

### 3-3: Teacher.h path 成员初始化时机错误 ✅

`path` 成员已从 `Teacher` 类中完全移除，教师不再维护课程文件路径。

---

### 3-4: 成绩文件名不一致 (`_Grade.txt` vs `_Grades.txt`) ✅

教师写入和所有读取处已统一使用 `_Grade.txt`。

---

### 3-5: 成绩文件读写字段数不匹配 ✅

- `Teacher::addStudentGrade()` 写入 4 字段：`name score credit gpa`
- `Teacher::showStudentGrade()` 读取 4 字段：`name score credit tmp`
- `Student::getGPA()` 读取 4 字段：`courseName score credit courseGpa`
- `Student::showCourseGrade()` 读取 4 字段：`courseName score credit gpa`

读写完全匹配。

---

### 3-6: 构造函数中调用 getGPA() 时 gradePath 尚未赋值 ✅

```cpp
// login() 中 (line 30-32)
this->gradePath = "data/" + id + "_Grade.txt";  // 先赋值
this->coursePath = "data/" + id + "_Courses.txt";
this->gpa = this->getGPA();                      // 后调用
```

---

### 3-7: 自动创建 data/ 目录 ✅

`main.cpp:24` → `_mkdir("data");`

---

### 3-8: main.cpp 修复非法输入导致死循环 ✅

使用 `tryCin(choice)` 替代裸 `cin >> choice`，失败时自动 `cin.clear()` + `cin.ignore()`。

---

### 3-9: Student.cpp 补全文件打开检查 ✅

5 个函数均已补全 `is_open()` 判断。（详见 P1-8）

---

### 3-10: 新增 User 抽象基类 ✅

`User.h` 存在，定义纯虚函数 `virtual void menu() = 0;`。

---

### 3-11: 改造三个派生类 ✅

`Manager`、`Teacher`、`Student` 均继承 `User`。

---

### 3-12: main.cpp 多态调用

**原建议**：`User* user = new Teacher(...);`

**当前代码**：使用具体类型指针
```cpp
Manager* user = new Manager();   // line 44
Teacher* user = new Teacher();   // line 62
Student* user = new Student();   // line 80
```

**状态**：⚠️ 部分实现（继承体系 + 虚函数已就位，但 main 中未使用基类指针。功能正常，纯风格差异）

---

### 3-13: 删除重复路径拼接 ✅

`Student::deleteCourse()` 直接使用成员 `coursePath`，不再手动重建路径字符串。

---

## 五、代码问题分析与修改记录（第四阶段，2026-06-08）

### 4-1: operator< → operator> ✅

`Student.h:26` 声明为 `bool operator>(const Student& s) const;`，语义正确。

### 4-2: showRanking() 使用 greater 显式降序 ✅

`Student.cpp:214` → `sort(sorted.begin(), sorted.end(), greater<Student>());`

### 4-3: showRanking() 先拷贝再排序 ✅

`Student.cpp:213-214` → `vector<Student> sorted = *allStudents; sort(...);`

### 4-4: 添加 #include \<functional\> ✅

`Student.cpp:8` → `#include <functional>`

### 4-5: 密码明文存储 ❌

未修改（同 P2-14）。

### 4-6: Windows 路径硬编码 ✅

已全部改为 `/`（同 P1-9）。

### 4-7: operator== 仅比较学号 ❌

`Student.cpp:219-221` 仍仅比较 `id`。属于设计选择，非 bug。

---

## 六、今晚修改总结（第五阶段，2026-06-08 当晚）

### 5-1: 新建 InputHelper.h ✅

`tryCin<T>()` 和 `readValid<T>()` 两个模板函数已就位，覆盖全部 27 处输入。

### 5-2: Manager.cpp 编码修复 ✅

47 处乱码手动重写为正确中文，UTF-8 保存，321 个中文字符正常。

---

## 七、今晚修改总结2（第六阶段，2026-06-08 当晚第二轮）

### 6-1: vector\<Student\> v → const vector\<Student\>* ✅

O(N²) → O(N) 内存，职责分离。

### 6-2: 析构函数 override ✅

三个派生类均已添加。

### 6-3: 路径 \\ → / ✅

27 处全部替换。

### 6-4: 教师成绩去重更新 ❌

讨论后用户决定暂不修改。

---

## 八、教务管理系统设计建议（第一阶段）对照

### 管理员端

| 建议功能 | 状态 |
|----------|------|
| 添加用户 | ✅ 已实现 |
| 删除用户 | ✅ 已实现 |
| 修改用户信息 | ❌ 未实现 |
| 查看所有用户列表 | ✅ 已实现（显示教师/学生） |
| 查看所有课程列表 | ❌ 未实现 |

### 学生端

| 建议功能 | 状态 |
|----------|------|
| 查看成绩和绩点 | ✅ 已实现 |
| 查看排名 | ✅ 已实现 |
| 查看课表 | ✅ 已实现 |
| 选课 | ✅ 已实现 |
| 退课 | ✅ 已实现 |
| 查看已选课程 | ✅ 已实现 |
| 修改密码 | ❌ 未实现 |

### 教师端

| 建议功能 | 状态 |
|----------|------|
| 添加学生成绩 | ✅ 已实现 |
| 查看学生成绩 | ✅ 已实现 |
| 修改学生成绩 | ❌ 未实现 |
| 查看课表 | ❌ 未实现 |
| 查看课程学生名单 | ❌ 未实现 |
| 添加课程 | ❌ 未实现 |
| 修改密码 | ❌ 未实现 |

### 逻辑细节

| 建议 | 状态 |
|------|------|
| 绩点换算规则 | ✅ 已实现（90→4.0, 80→3.0, 70→2.0, 60→1.0, <60→0） |
| 排名规则（GPA 降序） | ✅ 已实现 |
| 选课冲突检查 | ❌ 未实现 |
| 容量检查 | ❌ 未实现 |
| 管理员预设账户 | ✅ 已实现（`data/user.txt`: aaa/123456） |
| 输入验证 | ✅ 已实现 |

---

## 总结

| 分类 | 总数 | 已修复 | 部分修复 | 未修改 |
|------|------|--------|----------|--------|
| P0 致命问题 | 5 | 5 | 0 | 0 |
| P1 应该修复 | 5 | 4 | 1 | 0 |
| P2 建议改进 | 5 | 3 | 0 | 2 |
| 第三阶段 Bug 修复 | 13 | 12 | 1 | 0 |
| 第四阶段 语义修正 | 7 | 5 | 0 | 2 |
| 第五阶段 输入+编码 | 2 | 2 | 0 | 0 |
| 第六阶段 架构打磨 | 4 | 3 | 0 | 1 |
| 第一阶段 功能建议 | 19 | 11 | 0 | 8 |
| **合计** | **60** | **45** | **2** | **13** |

### 未修改项清单

| 编号 | 问题 | 原因 |
|------|------|------|
| P1-7 | Grade.h 仍有 `using std::string;` | 遗漏 |
| P2-11 | 课程不与公共课程库关联 | 功能增强，非必须 |
| P2-12 | 教师不能修改已有成绩 | 用户决定暂不改动 |
| P2-14 | 密码明文存储 | 大作业够用 |
| 4-7 | operator== 仅比较学号 | 设计选择，非 bug |
| 3-12 | main.cpp 未使用基类指针 | 功能正常，纯风格差异 |
| 6-4 | 教师成绩去重更新 | 用户决定暂不改动 |
| 设计建议×8 | 修改密码、课程库、选课冲突检查等 | 功能增强/未在开发范围内 |
