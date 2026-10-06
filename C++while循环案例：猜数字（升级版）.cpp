#include <iostream>
#include <conio.h> // 用于 _kbhit() 和 _getch()
#include <random>
#include <chrono>
#include <string>
#include <iomanip>

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 100);
    int num = distrib(gen);

    int val = 0;
    const int maxAttempts = 7;
    int validAttempts = 0; // 改为记录“有效”的猜测次数

    std::cout << "请猜一个1到100之间的数字，你只有 " << maxAttempts << " 次有效机会哦！" << std::endl;
    std::cout << "（注意：输入无效字符或超出范围的数字，不计入次数；但超时会计入次数）" << std::endl;

    while (validAttempts < maxAttempts) {
        // --- 核心：使用 _kbhit() 实现非阻塞式输入和倒计时 ---
        std::string inputBuffer = "";
        bool inputComplete = false;
        bool timeoutOccurred = false;

        auto startTime = std::chrono::steady_clock::now();
        const int timeLimitSeconds = 5;

        int remainingAttempts = maxAttempts - validAttempts;

        // 打印初始提示
        std::cout << "\n[剩余 " << remainingAttempts << " 次] 第 " << (validAttempts + 1) << " 次猜测 (" << timeLimitSeconds << "秒内): ";
        std::cout.flush();

        while (true) {
            // 1. 检查是否有按键按下
            if (_kbhit()) {
                char ch = _getch(); // 获取按下的字符

                if (ch == '\r') { // 如果按下了回车键
                    inputComplete = true;
                    std::cout << std::endl; // 换行
                    break;
                }
                else if (ch == '\b') { // 如果按下了退格键
                    if (!inputBuffer.empty()) {
                        inputBuffer.pop_back();
                        std::cout << "\b \b"; // 在屏幕上删除一个字符
                        std::cout.flush();
                    }
                }
                else if ((ch >= '0' && ch <= '9') || ch == '-') { // 只接受数字和负号
                    inputBuffer += ch;
                    std::cout << ch; // 将字符打印到屏幕上
                    std::cout.flush();
                }
            }

            // 2. 检查是否超时
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
            int remainingTime = timeLimitSeconds - static_cast<int>(elapsed);

            if (remainingTime <= 0) {
                timeoutOccurred = true;
                std::cout << std::endl; // 超时后换行
                break;
            }

            // 3. 动态更新倒计时显示
            std::cout << "\r[剩余 " << remainingAttempts << " 次] 第 " << (validAttempts + 1) << " 次猜测 (" << remainingTime << "秒内): " << inputBuffer << "   ";
            std::cout.flush();

            Sleep(50); // 短暂休眠，降低CPU占用
        }

        // --- 处理超时 ---
        if (timeoutOccurred) {
            std::cout << "【警告】输入超时！本次猜测无效，但计入总次数。" << std::endl;
            validAttempts++; // 超时会消耗一次机会

            if (validAttempts >= maxAttempts) {
                std::cout << "很遗憾，你的机会已经用完了。正确答案是: " << num << std::endl;
                break;
            }
            continue;
        }

        // --- 处理输入合法性 ---
        bool validInput = true;
        try {
            if (inputBuffer.empty()) throw std::invalid_argument("empty");
            val = std::stoi(inputBuffer);
        }
        catch (...) {
            validInput = false;
        }

        // 1. 检查是否为有效整数
        if (!validInput) {
            std::cout << "【错误】输入无效！请输入一个有效的整数。（本次输入不计入次数）" << std::endl;
            continue; // 不增加 validAttempts
        }

        // 2. 检查是否在 1-100 范围内
        if (val < 1 || val > 100) {
            std::cout << "【错误】数字 " << val << " 超出范围！必须在1到100之间。（本次输入不计入次数）" << std::endl;
            continue; // 不增加 validAttempts
        }

        // --- 只有输入完全合法，才计入有效次数 ---
        validAttempts++;

        // --- 判断大小 ---
        if (val > num) {
            std::cout << "猜大了！" << std::endl;
        }
        else if (val < num) {
            std::cout << "猜小了！" << std::endl;
        }
        else {
            std::cout << "恭喜你猜对了！你一共使用了 " << validAttempts << " 次机会。" << std::endl;
            break;
        }

        // 检查机会是否用完
        if (validAttempts >= maxAttempts) {
            std::cout << "很遗憾，你的机会已经用完了。正确答案是: " << num << std::endl;
            break;
        }
    }

    system("pause");
    return 0;
}
