#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <sqlite3.h>
#include "room.h"

/**
 * @brief SQLite 数据库管理器
 *
 * 负责持久化房间状态，每次状态变更实时写入数据库。
 * 程序启动时从数据库恢复上次的房间入住状态。
 *
 * 数据库表结构：
 *   rooms (room_number, type_name, price_per_hour, price_per_day,
 *           is_occupied, check_in_time, billing_mode)
 */
class DatabaseManager {
public:
    DatabaseManager();
    ~DatabaseManager();

    // ========== 生命周期 ==========
    /**
     * @brief 打开/创建数据库文件并初始化表结构
     * @param dbPath 数据库文件路径（如 "config/hotel_data.db"）
     * @return true 成功
     */
    bool Open(const std::string& dbPath);

    /// 关闭数据库连接
    void Close();

    /// 获取最后的错误信息
    const std::string& GetLastError() const { return last_error_; }

    // ========== 批量操作 ==========
    /**
     * @brief 将所有房间写入数据库（初始化时调用，用 INSERT OR IGNORE）
     * @param rooms 房间列表
     * @return true 成功
     */
    bool SaveAllRooms(const std::vector<std::unique_ptr<Room>>& rooms);

    /**
     * @brief 从数据库加载入住状态到 rooms 中
     *
     * 遍历每个房间，查找数据库中的记录：
     * - 如果 is_occupied=1，恢复入住状态（时间、计费方式）
     * - 如果数据库中无记录，保持 rooms 的默认状态（空闲）
     *
     * @param rooms 房间列表（会被修改）
     * @return true 成功
     */
    bool LoadRoomStates(std::vector<std::unique_ptr<Room>>& rooms);

    // ========== 单条实时写入 ==========
    /**
     * @brief 保存单个房间的当前状态（CheckIn / CheckOut 后调用）
     * @param room 房间引用
     * @return true 成功
     */
    bool SaveRoomState(const Room& room);

private:
    /// 执行单条 SQL（无结果集）
    bool ExecuteSQL(const std::string& sql);

    /// 创建表结构
    bool CreateTables();

    sqlite3* db_;
    std::string last_error_;
};

#endif // DATABASE_MANAGER_H
