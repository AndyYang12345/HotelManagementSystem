/**
 * @file gui/main.cpp
 * @brief Qt GUI 酒店管理系统入口
 */
#include <QApplication>
#include <iostream>
#include "main_window.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // 全局字体
    QFont defaultFont("SF Pro Text, Helvetica Neue, PingFang SC, sans-serif", 13);
    app.setFont(defaultFont);

    // 应用名称
    app.setApplicationName("HotelManagement");
    app.setApplicationVersion("1.0");

    // 查找配置文件
    std::string configPath = "config/hotel_config.xml";
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-c" && i + 1 < argc) {
            configPath = argv[++i];
        }
    }

    // 初始化主窗口
    MainWindow window;
    if (!window.Initialize(configPath)) {
        return 1;
    }

    window.show();
    return app.exec();
}
