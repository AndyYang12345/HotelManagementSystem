/**
 * @file main.cpp
 * @brief 酒店管理系统入口（Step 3 — Hotel 管理类验证）
 *
 * 当前阶段：验证 Hotel 类的登记入住、查询、费用计算、结账退房。
 */
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "hotel.h"

int main(int argc, char* argv[]) {
    std::string configPath = "config/hotel_config.xml";
    if (argc > 1) {
        configPath = argv[1];
    }

    // ========== 初始化酒店 ==========
    Hotel hotel;
    if (!hotel.Initialize(configPath)) {
        std::cerr << "[严重错误] " << hotel.GetLastError() << std::endl;
        return 1;
    }

    std::cout << "========================================" << std::endl;
    std::cout << "    " << hotel.GetName() << std::endl;
    std::cout << "    酒店管理系统 v1.0 (Step 3)" << std::endl;
    std::cout << "========================================" << std::endl;

    // ========== 展示所有房间初始状态 ==========
    hotel.ShowAllRooms();

    // ========== 测试：登记入住 ==========
    std::cout << "\n===== 操作：登记入住 =====" << std::endl;

    // 101 标间 — 按天
    std::cout << "> 标准间 101 按天入住" << std::endl;
    if (hotel.CheckIn(101, BillingMode::DAILY)) {
        auto* r = hotel.FindRoom(101);
        std::cout << "  ✅ 入住成功 — " << r->GetCheckInTimeStr() << std::endl;
    } else {
        std::cout << "  ❌ 入住失败: " << hotel.GetLastError() << std::endl;
    }

    // 111 大床房 — 按小时
    std::cout << "> 大床房 111 按小时入住" << std::endl;
    if (hotel.CheckIn(111, BillingMode::HOURLY)) {
        auto* r = hotel.FindRoom(111);
        std::cout << "  ✅ 入住成功 — " << r->GetCheckInTimeStr() << std::endl;
    } else {
        std::cout << "  ❌ 入住失败: " << hotel.GetLastError() << std::endl;
    }

    // 116 豪华套房 — 按天
    std::cout << "> 豪华套房 116 按天入住" << std::endl;
    if (hotel.CheckIn(116, BillingMode::DAILY)) {
        auto* r = hotel.FindRoom(116);
        std::cout << "  ✅ 入住成功 — " << r->GetCheckInTimeStr() << std::endl;
    } else {
        std::cout << "  ❌ 入住失败: " << hotel.GetLastError() << std::endl;
    }

    // ========== 测试：边界情况 ==========
    std::cout << "\n===== 边界测试 =====" << std::endl;

    // 重复入住
    std::cout << "> 标准间 101 重复登记: ";
    if (hotel.CheckIn(101, BillingMode::HOURLY)) {
        std::cout << "❌ 不应成功！" << std::endl;
    } else {
        std::cout << "✅ 正确拒绝 — " << hotel.GetLastError() << std::endl;
    }

    // 不存在的房间
    std::cout << "> 不存在的房间 999: ";
    if (hotel.CheckIn(999, BillingMode::DAILY)) {
        std::cout << "❌ 不应成功！" << std::endl;
    } else {
        std::cout << "✅ 正确拒绝 — " << hotel.GetLastError() << std::endl;
    }

    // ========== 展示入住后状态 ==========
    hotel.ShowOccupiedRooms();

    // ========== 测试：查询费用 ==========
    std::cout << "===== 操作：查询当前费用 =====" << std::endl;
    double fee101 = hotel.QueryFee(101);
    double fee111 = hotel.QueryFee(111);
    double fee105 = hotel.QueryFee(105);  // 空闲房间
    double fee999 = hotel.QueryFee(999);  // 不存在

    std::cout << "  标准间 101: ¥" << fee101 << std::endl;
    std::cout << "  大床房 111: ¥" << fee111 << std::endl;
    std::cout << "  标准间 105（空闲）: " << (fee105 == -1 ? "未入住" : "错误") << std::endl;
    std::cout << "  房间 999（不存在）: " << (fee999 == -2 ? "房间不存在" : "错误") << std::endl;

    // ========== 测试：结账退房 ==========
    std::cout << "\n...模拟入住2秒后退房..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "\n===== 操作：结账退房 =====" << std::endl;

    double total101 = hotel.CheckOut(101);
    std::cout << "> 标准间 101 退房，总费用: ¥" << total101 << std::endl;
    std::cout << "  退房时间: " << Room::NowString() << std::endl;

    double total111 = hotel.CheckOut(111);
    std::cout << "> 大床房 111 退房，总费用: ¥" << total111 << std::endl;

    // 二次退房（应失败）
    std::cout << "> 标准间 101 二次退房: ";
    double dup = hotel.CheckOut(101);
    if (dup == -1.0) {
        std::cout << "✅ 正确拒绝（未入住）" << std::endl;
    }

    // 不存在的房间退房
    std::cout << "> 房间 999 退房: ";
    double bad = hotel.CheckOut(999);
    if (bad == -2.0) {
        std::cout << "✅ 正确拒绝（房间不存在）" << std::endl;
    }

    // ========== 最终状态 ==========
    std::cout << "\n===== 统计 =====" << std::endl;
    std::cout << "  总房间数: " << hotel.GetTotalRooms() << std::endl;
    std::cout << "  已入住: " << hotel.GetOccupiedCount()
              << " / 空闲: " << hotel.GetAvailableCount() << std::endl;

    hotel.ShowAllRooms();

    std::cout << "========================================" << std::endl;
    std::cout << "  Step 3 验证通过！Hotel 管理类全部正常。" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}


