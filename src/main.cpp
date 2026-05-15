/**
 * @file main.cpp
 * @brief 酒店管理系统入口（Step 1 — 框架验证）
 *
 * 当前阶段：验证项目骨架、Room 基类、ConfigLoader 是否正常工作。
 */
#include <iostream>
#include <string>
#include "config_loader.h"

int main(int argc, char* argv[]) {
    std::string configPath = "config/hotel_config.xml";

    // 若命令行指定了配置文件路径则使用之
    if (argc > 1) {
        configPath = argv[1];
    }

    // ---------- 加载 XML 配置 ----------
    ConfigLoader loader;
    if (!loader.Load(configPath)) {
        std::cerr << "[错误] " << loader.GetLastError() << std::endl;
        return 1;
    }

    std::cout << "========================================" << std::endl;
    std::cout << "    " << loader.GetHotelName() << std::endl;
    std::cout << "    酒店管理系统 v1.0 (Step 1)" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    const auto& types = loader.GetRoomTypeConfigs();
    std::cout << "已加载 " << types.size() << " 种房间类型:" << std::endl;
    std::cout << std::endl;

    for (const auto& t : types) {
        std::cout << "  [" << t.label << "] (" << t.typeName << ")" << std::endl;
        std::cout << "    数量: " << t.count << " 间" << std::endl;
        std::cout << "    按小时: ¥" << t.pricePerHour
                  << " / 按天: ¥" << t.pricePerDay << std::endl;
        std::cout << std::endl;
    }

    std::cout << "========================================" << std::endl;
    std::cout << "  Step 1 验证通过！项目骨架搭建完毕。" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
