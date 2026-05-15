#include "room.h"
#include <cmath>
#include <stdexcept>

// ============================================================================
// 构造
// ============================================================================
Room::Room(int number, const std::string& typeLabel,
           double pricePerHour, double pricePerDay)
    : room_number_(number)
    , room_type_name_(typeLabel)
    , is_occupied_(false)
    , check_in_time_(0)
    , check_out_time_(0)
    , billing_mode_(BillingMode::HOURLY)
    , price_per_hour_(pricePerHour)
    , price_per_day_(pricePerDay)
{}

// ============================================================================
// CheckIn — 登记入住
// ============================================================================
bool Room::CheckIn(BillingMode mode) {
    if (is_occupied_) {
        return false;  // 已有人入住
    }
    is_occupied_ = true;
    billing_mode_ = mode;
    check_in_time_ = std::time(nullptr);
    check_out_time_ = 0;
    return true;
}

// ============================================================================
// RestoreState — 从数据库恢复状态
// ============================================================================
void Room::RestoreState(std::time_t checkInTime, BillingMode mode) {
    is_occupied_    = true;
    check_in_time_  = checkInTime;
    check_out_time_ = 0;
    billing_mode_   = mode;
}

// ============================================================================
// CheckOut — 结账退房
// ============================================================================
double Room::CheckOut() {
    if (!is_occupied_) {
        return -1.0;  // 未入住
    }
    std::time_t now = std::time(nullptr);
    double fee = CalculateFee(now);
    // 重置状态
    is_occupied_ = false;
    check_out_time_ = now;
    // check_in_time_ 保留以便查询历史
    return fee;
}

// ============================================================================
// QueryCurrentFee — 查询当前费用（不退房）
// ============================================================================
double Room::QueryCurrentFee() const {
    if (!is_occupied_) {
        return -1.0;
    }
    return CalculateFee(std::time(nullptr));
}

// ============================================================================
// CalculateFee — 核心费用计算
// ============================================================================
double Room::CalculateFee(std::time_t endTime) const {
    if (check_in_time_ == 0 || endTime < check_in_time_) {
        return 0.0;
    }

    double totalSeconds = std::difftime(endTime, check_in_time_);

    if (billing_mode_ == BillingMode::HOURLY) {
        // 按小时计费：不足1小时按1小时算
        double hours = std::ceil(totalSeconds / 3600.0);
        if (hours < 1.0) hours = 1.0;
        return hours * price_per_hour_;
    } else {
        // 按天计费：不足1天按1天算
        double days = std::ceil(totalSeconds / 86400.0);
        if (days < 1.0) days = 1.0;
        return days * price_per_day_;
    }
}

// ============================================================================
// 时间格式化
// ============================================================================
std::string Room::GetCheckInTimeStr() const {
    return TimeToString(check_in_time_);
}

std::string Room::GetCheckOutTimeStr() const {
    return TimeToString(check_out_time_);
}

std::string Room::TimeToString(std::time_t t) {
    if (t == 0) return "N/A";
    std::tm* local = std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(local, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Room::NowString() {
    return TimeToString(std::time(nullptr));
}
