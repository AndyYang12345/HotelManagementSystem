#include "big_bed_room.h"

BigBedRoom::BigBedRoom(int number, double pricePerHour, double pricePerDay)
    : Room(number, "大床房", pricePerHour, pricePerDay)
{}

std::string BigBedRoom::GetDescription() const {
    return "大床房 [" + std::to_string(room_number_) + "]"
           " — 1.8m×2.0m 大床，适合单人商务或情侣入住"
           " | 单价: ¥" + std::to_string(static_cast<int>(price_per_hour_)) + "/小时"
           " / ¥" + std::to_string(static_cast<int>(price_per_day_)) + "/天"
           " | 状态: " + (is_occupied_ ? "已入住" : "空闲");
}
