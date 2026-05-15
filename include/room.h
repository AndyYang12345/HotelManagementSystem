#ifndef ROOM_H
#define ROOM_H

#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

/**
 * @brief 计费方式枚举
 */
enum class BillingMode {
    HOURLY,  // 按小时计费
    DAILY    // 按天计费
};

/**
 * @brief 获取计费方式的字符串表示
 */
inline std::string BillingModeToString(BillingMode mode) {
    return (mode == BillingMode::HOURLY) ? "按小时计费" : "按天计费";
}

/**
 * @brief 房间抽象基类
 *
 * 所有房间类型的共同抽象，维护入住时间、型号、计费方式等属性。
 * 派生类需要实现 GetDescription() 返回具体房间描述。
 */
class Room {
protected:
    int room_number_;             ///< 房间号
    std::string room_type_name_;  ///< 房间类型名称（如 "标准间"）
    bool is_occupied_;            ///< 是否已入住
    std::time_t check_in_time_;   ///< 入住时间戳
    std::time_t check_out_time_;  ///< 退房时间戳（退房前为 0）
    BillingMode billing_mode_;    ///< 计费方式
    double price_per_hour_;       ///< 每小时单价
    double price_per_day_;        ///< 每天单价

public:
    Room(int number, const std::string& typeLabel,
         double pricePerHour, double pricePerDay);
    virtual ~Room() = default;

    // ========== 纯虚函数 ==========
    /// 返回房间的具体描述（如床型、设施等）
    virtual std::string GetDescription() const = 0;

    // ========== 核心操作 ==========
    /// 登记入住
    /// @param mode 计费方式（按小时/按天）
    /// @return true 成功，false 失败（已有人入住）
    virtual bool CheckIn(BillingMode mode);

    /// 结账退房
    /// @return 总费用；若未入住返回 -1
    virtual double CheckOut();

    /// 查询当前已产生的费用（不退房也能查看）
    /// @return 当前费用；若未入住返回 -1
    virtual double QueryCurrentFee() const;

    // ========== 状态查询 ==========
    bool IsOccupied() const { return is_occupied_; }
    int GetRoomNumber() const { return room_number_; }
    std::string GetTypeName() const { return room_type_name_; }
    BillingMode GetBillingMode() const { return billing_mode_; }
    double GetPricePerHour() const { return price_per_hour_; }
    double GetPricePerDay() const { return price_per_day_; }

    /// 获取入住时间字符串
    std::string GetCheckInTimeStr() const;
    /// 获取退房时间字符串
    std::string GetCheckOutTimeStr() const;

    // ========== 静态工具 ==========
    /// 将 time_t 转为可读字符串 "YYYY-MM-DD HH:MM:SS"
    static std::string TimeToString(std::time_t t);
    /// 获取当前时间字符串
    static std::string NowString();

protected:
    /// 计算从 check_in_time_ 到 当前时间/退房时间 的费用
    double CalculateFee(std::time_t endTime) const;
};

#endif // ROOM_H
