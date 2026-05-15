#ifndef HOTEL_H
#define HOTEL_H

#include <string>
#include <vector>
#include <memory>
#include "room.h"
#include "config_loader.h"

/**
 * @brief 酒店管理类
 *
 * 负责管理所有房间的登记入住、状态查询、费用查询和结账退房。
 * 通过 ConfigLoader 读取 XML 配置，自动创建对应类型的房间实例。
 */
class Hotel {
public:
    Hotel() = default;
    ~Hotel() = default;

    // ========== 初始化 ==========
    /**
     * @brief 从 XML 配置文件初始化酒店
     * @param configPath XML 配置文件路径
     * @return true 成功，false 失败（可通过 GetLastError() 获取原因）
     */
    bool Initialize(const std::string& configPath);

    /// 获取初始化失败的错误信息
    const std::string& GetLastError() const { return last_error_; }

    // ========== 状态查询 ==========
    /// 打印所有房间信息
    void ShowAllRooms() const;

    /// 打印当前空闲房间
    void ShowAvailableRooms() const;

    /// 打印当前已入住房间
    void ShowOccupiedRooms() const;

    // ========== 核心操作 ==========
    /**
     * @brief 登记入住
     * @param roomNumber 房间号
     * @param mode 计费方式（按小时/按天）
     * @return true 成功，false 失败（房间不存在或已入住）
     */
    bool CheckIn(int roomNumber, BillingMode mode);

    /**
     * @brief 查询当前费用（不退房）
     * @param roomNumber 房间号
     * @return 费用；-1 表示未入住；-2 表示房间不存在
     */
    double QueryFee(int roomNumber) const;

    /**
     * @brief 结账退房
     * @param roomNumber 房间号
     * @return 总费用；-1 表示未入住；-2 表示房间不存在
     */
    double CheckOut(int roomNumber);

    // ========== 查找 ==========
    Room* FindRoom(int roomNumber);
    const Room* FindRoom(int roomNumber) const;

    // ========== 统计 ==========
    int GetTotalRooms() const { return static_cast<int>(rooms_.size()); }
    int GetAvailableCount() const;
    int GetOccupiedCount() const;

    const std::string& GetName() const { return name_; }

private:
    /// 根据类型名创建对应的房间实例（工厂方法）
    static std::unique_ptr<Room> CreateRoom(const RoomTypeConfig& cfg, int number);

    std::string name_;
    std::vector<std::unique_ptr<Room>> rooms_;
    std::string last_error_;
};

#endif // HOTEL_H
