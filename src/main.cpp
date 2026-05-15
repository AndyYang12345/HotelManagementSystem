/**
 * @file main.cpp
 * @brief 酒店管理系统入口
 *
 * 初始化 Hotel → 启动 CLI 交互界面。
 */
#include <iostream>
#include <string>
#include "hotel.h"
#include "cli.h"

int main(int argc, char* argv[]) {
    std::string configPath = "config/hotel_config.xml";
    if (argc > 1) {
        configPath = argv[1];
    }

    // ========== 初始化酒店 ==========
    Hotel hotel;
    if (!hotel.Initialize(configPath)) {
        std::cerr << "[严重错误] " << hotel.GetLastError() << std::endl;
        std::cerr << "请确保配置文件 config/hotel_config.xml 存在且格式正确。"
                  << std::endl;
        return 1;
    }

    // ========== 启动 CLI ==========
    HotelCLI cli(hotel);
    cli.Run();

    return 0;
}


