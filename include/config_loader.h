#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <string>
#include <vector>
#include <memory>
#include "room.h"

/**
 * @brief 单个房间类型的配置信息（从 XML 读取）
 */
struct RoomTypeConfig {
    std::string typeName;     ///< 类名标识（如 "StandardRoom"）
    std::string label;        ///< 中文显示名（如 "标准间"）
    double pricePerHour;      ///< 每小时单价
    double pricePerDay;       ///< 每天单价
    int count;                ///< 该类型房间数量
};

/**
 * @brief XML 配置文件加载器
 *
 * 负责解析 hotel_config.xml，提取酒店名称和房间类型配置列表。
 * 使用 tinyxml2 进行 XML 解析。
 */
class ConfigLoader {
public:
    ConfigLoader() = default;

    /**
     * @brief 加载 XML 配置文件
     * @param filePath XML 文件路径
     * @return true 加载成功，false 失败
     */
    bool Load(const std::string& filePath);

    /// 获取酒店名称
    const std::string& GetHotelName() const { return hotel_name_; }

    /// 获取所有房间类型配置
    const std::vector<RoomTypeConfig>& GetRoomTypeConfigs() const { return room_types_; }

    /// 获取解析过程中的错误信息
    const std::string& GetLastError() const { return last_error_; }

private:
    std::string hotel_name_;
    std::vector<RoomTypeConfig> room_types_;
    std::string last_error_;
};

#endif // CONFIG_LOADER_H
