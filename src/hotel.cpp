#include "hotel.h"
#include "big_bed_room.h"
#include "standard_room.h"
#include "luxury_room.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

// ============================================================================
// Initialize — 从 XML 配置初始化所有房间
// ============================================================================
bool Hotel::Initialize(const std::string& configPath) {
    ConfigLoader loader;
    if (!loader.Load(configPath)) {
        last_error_ = loader.GetLastError();
        return false;
    }

    name_ = loader.GetHotelName();
    rooms_.clear();

    const auto& types = loader.GetRoomTypeConfigs();
    int globalNumber = 101;  // 起始房间号

    for (const auto& typeCfg : types) {
        for (int i = 0; i < typeCfg.count; ++i) {
            auto room = CreateRoom(typeCfg, globalNumber);
            if (room) {
                rooms_.push_back(std::move(room));
                ++globalNumber;
            }
        }
    }

    if (rooms_.empty()) {
        last_error_ = "未能创建任何房间";
        return false;
    }

    return true;
}

// ============================================================================
// CreateRoom — 工厂方法
// ============================================================================
std::unique_ptr<Room> Hotel::CreateRoom(const RoomTypeConfig& cfg, int number) {
    if (cfg.typeName == "StandardRoom") {
        return std::make_unique<StandardRoom>(number, cfg.pricePerHour, cfg.pricePerDay);
    } else if (cfg.typeName == "BigBedRoom") {
        return std::make_unique<BigBedRoom>(number, cfg.pricePerHour, cfg.pricePerDay);
    } else if (cfg.typeName == "LuxuryRoom") {
        return std::make_unique<LuxuryRoom>(number, cfg.pricePerHour, cfg.pricePerDay);
    }
    return nullptr;
}

// ============================================================================
// FindRoom
// ============================================================================
Room* Hotel::FindRoom(int roomNumber) {
    auto it = std::find_if(rooms_.begin(), rooms_.end(),
        [roomNumber](const auto& r) { return r->GetRoomNumber() == roomNumber; });
    return (it != rooms_.end()) ? it->get() : nullptr;
}

const Room* Hotel::FindRoom(int roomNumber) const {
    auto it = std::find_if(rooms_.begin(), rooms_.end(),
        [roomNumber](const auto& r) { return r->GetRoomNumber() == roomNumber; });
    return (it != rooms_.end()) ? it->get() : nullptr;
}

// ============================================================================
// CheckIn
// ============================================================================
bool Hotel::CheckIn(int roomNumber, BillingMode mode) {
    Room* room = FindRoom(roomNumber);
    if (!room) {
        last_error_ = "房间 " + std::to_string(roomNumber) + " 不存在";
        return false;
    }
    if (!room->CheckIn(mode)) {
        last_error_ = "房间 " + std::to_string(roomNumber) + " 已有人入住";
        return false;
    }
    return true;
}

// ============================================================================
// QueryFee
// ============================================================================
double Hotel::QueryFee(int roomNumber) const {
    const Room* room = FindRoom(roomNumber);
    if (!room) return -2.0;   // 房间不存在
    return room->QueryCurrentFee();
}

// ============================================================================
// CheckOut
// ============================================================================
double Hotel::CheckOut(int roomNumber) {
    Room* room = FindRoom(roomNumber);
    if (!room) return -2.0;   // 房间不存在
    return room->CheckOut();
}

// ============================================================================
// 统计
// ============================================================================
int Hotel::GetAvailableCount() const {
    int count = 0;
    for (const auto& r : rooms_) {
        if (!r->IsOccupied()) ++count;
    }
    return count;
}

int Hotel::GetOccupiedCount() const {
    return GetTotalRooms() - GetAvailableCount();
}

// ============================================================================
// 打印输出
// ============================================================================
static void PrintSeparator(const std::string& ch = "-", int width = 72) {
    for (int i = 0; i < width; ++i) std::cout << ch;
    std::cout << std::endl;
}

void Hotel::ShowAllRooms() const {
    std::cout << std::endl;
    PrintSeparator("=");
    std::cout << "  " << name_ << " -- 全部房间 (共 " << GetTotalRooms()
              << " 间 / 空闲 " << GetAvailableCount()
              << " / 入住 " << GetOccupiedCount() << ")" << std::endl;
    PrintSeparator("=");

    std::cout << std::left
              << std::setw(6)  << "房号"
              << std::setw(12) << "类型"
              << std::setw(14) << "时价/天价"
              << std::setw(12) << "计费方式"
              << std::setw(24) << "入住时间"
              << "状态" << std::endl;
    PrintSeparator("-");

    for (const auto& r : rooms_) {
        std::string priceStr = "\u00a5" + std::to_string(static_cast<int>(r->GetPricePerHour()))
                             + "/\u00a5" + std::to_string(static_cast<int>(r->GetPricePerDay()));
        std::cout << std::left
                  << std::setw(6)  << r->GetRoomNumber()
                  << std::setw(12) << r->GetTypeName()
                  << std::setw(14) << priceStr
                  << std::setw(12) << (r->IsOccupied() ? BillingModeToString(r->GetBillingMode()) : "-")
                  << std::setw(24) << (r->IsOccupied() ? r->GetCheckInTimeStr() : "-")
                  << (r->IsOccupied() ? "已入住" : "空闲") << std::endl;
    }
    PrintSeparator("=");
    std::cout << std::endl;
}

void Hotel::ShowAvailableRooms() const {
    std::cout << std::endl;
    PrintSeparator("-");
    std::cout << "  空闲房间列表" << std::endl;
    PrintSeparator("-");

    bool found = false;
    for (const auto& r : rooms_) {
        if (!r->IsOccupied()) {
            std::cout << "  " << r->GetDescription() << std::endl;
            found = true;
        }
    }
    if (!found) {
        std::cout << "  （暂无空闲房间）" << std::endl;
    }
    PrintSeparator("-");
    std::cout << std::endl;
}

void Hotel::ShowOccupiedRooms() const {
    std::cout << std::endl;
    PrintSeparator("-");
    std::cout << "  已入住房间列表" << std::endl;
    PrintSeparator("-");

    bool found = false;
    for (const auto& r : rooms_) {
        if (r->IsOccupied()) {
            std::cout << "  " << r->GetDescription() << std::endl;
            std::cout << "    计费方式: " << BillingModeToString(r->GetBillingMode())
                      << " | 当前费用: ¥" << r->QueryCurrentFee() << std::endl;
            found = true;
        }
    }
    if (!found) {
        std::cout << "  （暂无入住房间）" << std::endl;
    }
    PrintSeparator("-");
    std::cout << std::endl;
}
