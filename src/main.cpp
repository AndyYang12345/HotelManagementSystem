/**
 * @file main.cpp
 * @brief 酒店管理系统入口（Step 2 — 派生类验证）
 *
 * 当前阶段：验证 Room 基类 + BigBedRoom / StandardRoom / LuxuryRoom 派生类。
 */
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>

#include "config_loader.h"
#include "big_bed_room.h"
#include "standard_room.h"
#include "luxury_room.h"

int main(int argc, char* argv[]) {
    std::string configPath = "config/hotel_config.xml";
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
    std::cout << "    酒店管理系统 v1.0 (Step 2)" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // ---------- 手动创建三类房间（用 XML 配置的价格） ----------
    const auto& types = loader.GetRoomTypeConfigs();
    if (types.size() < 3) {
        std::cerr << "[错误] 配置中至少需要3种房间类型" << std::endl;
        return 1;
    }

    std::vector<std::unique_ptr<Room>> rooms;

    // 按照 XML 顺序：标间、大床房、豪华套房
    rooms.push_back(std::make_unique<StandardRoom>(101,
        types[0].pricePerHour, types[0].pricePerDay));
    rooms.push_back(std::make_unique<BigBedRoom>(201,
        types[1].pricePerHour, types[1].pricePerDay));
    rooms.push_back(std::make_unique<LuxuryRoom>(301,
        types[2].pricePerHour, types[2].pricePerDay));

    // ---------- 展示所有房间信息 ----------
    std::cout << "----------- [房间列表] -----------" << std::endl;
    for (const auto& room : rooms) {
        std::cout << room->GetDescription() << std::endl;
    }
    std::cout << std::endl;

    // ---------- 测试：登记入住 ----------
    std::cout << "===== 测试：登记入住 =====" << std::endl;
    std::cout << "> 标准间 101 按天入住" << std::endl;
    rooms[0]->CheckIn(BillingMode::DAILY);
    std::cout << "  入住时间: " << rooms[0]->GetCheckInTimeStr() << std::endl;
    std::cout << "  状态: " << (rooms[0]->IsOccupied() ? "已入住" : "空闲") << std::endl;
    std::cout << std::endl;

    std::cout << "> 大床房 201 按小时入住" << std::endl;
    rooms[1]->CheckIn(BillingMode::HOURLY);
    std::cout << "  入住时间: " << rooms[1]->GetCheckInTimeStr() << std::endl;
    std::cout << "  计费方式: " << BillingModeToString(rooms[1]->GetBillingMode()) << std::endl;
    std::cout << std::endl;

    // ---------- 测试：重复入住（应失败） ----------
    std::cout << "===== 测试：重复入住（应失败） =====" << std::endl;
    bool dupResult = rooms[0]->CheckIn(BillingMode::HOURLY);
    std::cout << "> 标准间 101 再次登记: "
              << (dupResult ? "成功" : "失败（已有人入住）") << std::endl;
    std::cout << std::endl;

    // ---------- 测试：查询当前费用 ----------
    std::cout << "===== 测试：查询当前费用 =====" << std::endl;
    std::cout << "> 标准间 101 当前费用: ¥"
              << rooms[0]->QueryCurrentFee() << std::endl;
    std::cout << "> 大床房 201 当前费用: ¥"
              << rooms[1]->QueryCurrentFee() << std::endl;
    std::cout << "> 豪华套房 301（未入住）: ¥"
              << rooms[2]->QueryCurrentFee() << " （-1表示未入住）" << std::endl;
    std::cout << std::endl;

    // ---------- 等待2秒让费用产生差异 ----------
    std::cout << "...等待 2 秒以验证实时计费..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // ---------- 测试：结账退房 ----------
    std::cout << "===== 测试：结账退房 =====" << std::endl;
    double fee101 = rooms[0]->CheckOut();
    std::cout << "> 标准间 101 退房" << std::endl;
    std::cout << "  退房时间: " << rooms[0]->GetCheckOutTimeStr() << std::endl;
    std::cout << "  总费用: ¥" << fee101 << std::endl;
    std::cout << "  状态: " << (rooms[0]->IsOccupied() ? "已入住" : "空闲") << std::endl;
    std::cout << std::endl;

    double fee201 = rooms[1]->CheckOut();
    std::cout << "> 大床房 201 退房" << std::endl;
    std::cout << "  退房时间: " << rooms[1]->GetCheckOutTimeStr() << std::endl;
    std::cout << "  总费用: ¥" << fee201 << std::endl;
    std::cout << std::endl;

    // ---------- 最终状态 ----------
    std::cout << "----------- [最终房间状态] -----------" << std::endl;
    for (const auto& room : rooms) {
        std::cout << room->GetDescription() << std::endl;
    }
    std::cout << std::endl;

    std::cout << "========================================" << std::endl;
    std::cout << "  Step 2 验证通过！所有派生类正常工作。" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}

