#include "luxury_room.h"

LuxuryRoom::LuxuryRoom(int number, double pricePerHour, double pricePerDay)
    : Room(number, "豪华套房", pricePerHour, pricePerDay)
{}

std::string LuxuryRoom::GetDescription() const {
    return "豪华套房 [" + std::to_string(room_number_) + "]"
           " — 独立客厅+卧室，2.0m×2.0m 特大床，高端入住体验"
           " | 单价: ¥" + std::to_string(static_cast<int>(price_per_hour_)) + "/小时"
           " / ¥" + std::to_string(static_cast<int>(price_per_day_)) + "/天"
           " | 状态: " + (is_occupied_ ? "已入住" : "空闲");
}
