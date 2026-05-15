#include "standard_room.h"

StandardRoom::StandardRoom(int number, double pricePerHour, double pricePerDay)
    : Room(number, "标准间", pricePerHour, pricePerDay)
{}

std::string StandardRoom::GetDescription() const {
    return "标准间 [" + std::to_string(room_number_) + "]"
           " — 两张 1.2m×2.0m 单人床，经济实惠双人房"
           " | 单价: ¥" + std::to_string(static_cast<int>(price_per_hour_)) + "/小时"
           " / ¥" + std::to_string(static_cast<int>(price_per_day_)) + "/天"
           " | 状态: " + (is_occupied_ ? "已入住" : "空闲");
}
