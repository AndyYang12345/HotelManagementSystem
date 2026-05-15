#ifndef CLI_H
#define CLI_H

#include "hotel.h"

/**
 * @brief 命令行交互界面
 *
 * 提供用户友好的菜单式交互，封装所有输入输出逻辑。
 * 负责输入校验、边界处理、格式化展示。
 */
class HotelCLI {
public:
    explicit HotelCLI(Hotel& hotel);
    ~HotelCLI() = default;

    /**
     * @brief 启动 CLI 主循环
     *
     * 显示欢迎界面 → 进入菜单循环 → 直到用户选择退出。
     */
    void Run();

private:
    // ========== 界面展示 ==========
    void ShowBanner() const;
    void ShowMenu() const;
    void ClearScreen() const;
    void Pause() const;

    // ========== 操作处理 ==========
    void HandleCheckIn();
    void HandleQueryAllRooms();
    void HandleQueryAvailableRooms();
    void HandleQueryOccupiedRooms();
    void HandleQueryFee();
    void HandleCheckOut();

    // ========== 输入辅助 ==========
    int GetChoice(int min, int max) const;
    int GetRoomNumber() const;
    BillingMode GetBillingMode() const;
    void ShowError(const std::string& msg) const;
    void ShowSuccess(const std::string& msg) const;

    Hotel& hotel_;
    bool running_;
};

#endif // CLI_H
