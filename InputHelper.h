#pragma once
#include <iostream>
#include <limits>
#include <string>

// 读取输入到 value，成功返回 true，失败时清理流并丢弃当前行后返回 false
template<typename T>
bool tryCin(T& value) {
    if (std::cin >> value) return true;
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return false;
}

template<typename T>
void readValid(T& value,const std::string& errorMsg = "无效输入，请重新输入：") {
    while (!tryCin(value)) {
        std::cout << errorMsg << std::endl;
    }
}
