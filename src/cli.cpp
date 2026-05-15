#include "cli.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include <unistd.h>    // isatty

// ============================================================================
// 辅助：检测是否为交互式终端
// ============================================================================
static bool IsInteractive() {
    return isatty(STDIN_FILENO) != 0;
}

// ============================================================================
// 构造 & 主循环
// ============================================================================
HotelCLI::HotelCLI(Hotel& hotel) : hotel_(hotel), running_(false) {}

void HotelCLI::Run() {
    running_ = true;

    ClearScreen();
    ShowBanner();

    while (running_) {
        ShowMenu();
        int choice = GetChoice(1, 6);

        ClearScreen();
        switch (choice) {
            case 1: HandleCheckIn();           break;
            case 2: HandleQueryAllRooms();      break;
            case 3: HandleQueryAvailableRooms(); break;
            case 4: HandleQueryFee();           break;
            case 5: HandleCheckOut();           break;
            case 6:
                std::cout << "\n  👋 感谢使用 " << hotel_.GetName()
                          << "，再见！\n" << std::endl;
                running_ = false;
                break;
        }
        if (running_) Pause();
    }
}

// ============================================================================
// 界面展示
// ============================================================================
void HotelCLI::ClearScreen() const {
    if (!IsInteractive()) return;
    #ifdef _WIN32
        std::system("cls");
    #else
        std::system("clear");
    #endif
}

void HotelCLI::ShowBanner() const {
    std::cout << std::endl;
    std::cout << "  ╔══════════════════════════════════════╗" << std::endl;
    std::cout << "  ║        " << hotel_.GetName() << "        ║" << std::endl;
    std::cout << "  ║         酒店管理系统 v1.0            ║" << std::endl;
    std::cout << "  ╚══════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    std::cout << "  总房间: " << hotel_.GetTotalRooms()
              << " 间 | 空闲: " << hotel_.GetAvailableCount()
              << " | 已入住: " << hotel_.GetOccupiedCount() << std::endl;
    std::cout << std::endl;
}

void HotelCLI::ShowMenu() const {
    std::cout << "  ┌──────────────────────────────────────┐" << std::endl;
    std::cout << "  │  [1] 登记入住                        │" << std::endl;
    std::cout << "  │  [2] 查看全部房间状态                │" << std::endl;
    std::cout << "  │  [3] 查看空闲房间                    │" << std::endl;
    std::cout << "  │  [4] 查询当前费用                    │" << std::endl;
    std::cout << "  │  [5] 结账退房                        │" << std::endl;
    std::cout << "  │  [6] 退出系统                        │" << std::endl;
    std::cout << "  └──────────────────────────────────────┘" << std::endl;
    std::cout << "  请输入选项 (1-6): ";
}

void HotelCLI::Pause() const {
    if (!IsInteractive()) return;
    std::cout << "\n  按 Enter 键返回主菜单...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
    ClearScreen();
    ShowBanner();
}

// ============================================================================
// 输入辅助函数
// ============================================================================
int HotelCLI::GetChoice(int min, int max) const {
    int choice;
    while (true) {
        std::cin >> choice;
        if (std::cin.fail() || choice < min || choice > max) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "  ⚠️  输入无效，请输入 " << min << "-" << max
                      << " 之间的数字: ";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return choice;
        }
    }
}

int HotelCLI::GetRoomNumber() const {
    int number;
    while (true) {
        std::cout << "  请输入房间号: ";
        std::cin >> number;
        if (std::cin.fail() || number <= 0) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "  ⚠️  房间号必须为正整数，请重新输入。" << std::endl;
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return number;
        }
    }
}

BillingMode HotelCLI::GetBillingMode() const {
    std::cout << std::endl;
    std::cout << "  请选择计费方式:" << std::endl;
    std::cout << "    [1] 按小时计费" << std::endl;
    std::cout << "    [2] 按天计费" << std::endl;
    std::cout << "  请输入 (1-2): ";

    int mode = GetChoice(1, 2);
    return (mode == 1) ? BillingMode::HOURLY : BillingMode::DAILY;
}

void HotelCLI::ShowError(const std::string& msg) const {
    std::cout << std::endl;
    std::cout << "  ❌ 操作失败: " << msg << std::endl;
}

void HotelCLI::ShowSuccess(const std::string& msg) const {
    std::cout << std::endl;
    std::cout << "  ✅ " << msg << std::endl;
}

// ============================================================================
// 操作处理
// ============================================================================
void HotelCLI::HandleCheckIn() {
    std::cout << std::endl;
    std::cout << "  ═══ 登记入住 ═══" << std::endl;

    // 先展示空闲房间供选择
    if (hotel_.GetAvailableCount() == 0) {
        ShowError("所有房间已满，无法办理入住！");
        return;
    }

    hotel_.ShowAvailableRooms();

    int roomNumber = GetRoomNumber();
    BillingMode mode = GetBillingMode();

    if (hotel_.CheckIn(roomNumber, mode)) {
        const Room* r = hotel_.FindRoom(roomNumber);
        ShowSuccess("房间 " + std::to_string(roomNumber)
                    + " (" + r->GetTypeName() + ") 登记成功！");
        std::cout << "  计费方式: " << BillingModeToString(mode) << std::endl;
        std::cout << "  入住时间: " << r->GetCheckInTimeStr() << std::endl;
        std::cout << "  单价: ¥" << static_cast<int>(r->GetPricePerHour())
                  << "/小时  |  ¥" << static_cast<int>(r->GetPricePerDay())
                  << "/天" << std::endl;
    } else {
        ShowError(hotel_.GetLastError());
    }
}

void HotelCLI::HandleQueryAllRooms() {
    hotel_.ShowAllRooms();
}

void HotelCLI::HandleQueryAvailableRooms() {
    hotel_.ShowAvailableRooms();
}

void HotelCLI::HandleQueryOccupiedRooms() {
    hotel_.ShowOccupiedRooms();
}

void HotelCLI::HandleQueryFee() {
    std::cout << std::endl;
    std::cout << "  ═══ 查询当前费用 ═══" << std::endl;

    int roomNumber = GetRoomNumber();
    double fee = hotel_.QueryFee(roomNumber);

    if (fee == -2.0) {
        ShowError("房间 " + std::to_string(roomNumber) + " 不存在。");
    } else if (fee == -1.0) {
        std::cout << std::endl;
        std::cout << "  ℹ️  房间 " << roomNumber << " 当前未入住，无费用。" << std::endl;
    } else {
        const Room* r = hotel_.FindRoom(roomNumber);
        std::cout << std::endl;
        std::cout << "  ══════════════════════" << std::endl;
        std::cout << "   房间号: " << roomNumber
                  << " (" << r->GetTypeName() << ")" << std::endl;
        std::cout << "   计费方式: " << BillingModeToString(r->GetBillingMode()) << std::endl;
        std::cout << "   入住时间: " << r->GetCheckInTimeStr() << std::endl;
        std::cout << "   当前费用: ¥" << fee << std::endl;
        std::cout << "  ══════════════════════" << std::endl;
    }
}

void HotelCLI::HandleCheckOut() {
    std::cout << std::endl;
    std::cout << "  ═══ 结账退房 ═══" << std::endl;

    if (hotel_.GetOccupiedCount() == 0) {
        std::cout << std::endl;
        std::cout << "  ℹ️  当前没有已入住的房间。" << std::endl;
        return;
    }

    // 展示已入住房间供参考
    hotel_.ShowOccupiedRooms();

    int roomNumber = GetRoomNumber();
    double fee = hotel_.CheckOut(roomNumber);

    if (fee == -2.0) {
        ShowError("房间 " + std::to_string(roomNumber) + " 不存在。");
    } else if (fee == -1.0) {
        ShowError("房间 " + std::to_string(roomNumber) + " 当前未入住，无法退房。");
    } else {
        std::cout << std::endl;
        std::cout << "  ══════════════════════" << std::endl;
        std::cout << "  ✅ 退房成功！" << std::endl;
        std::cout << "   房间号: " << roomNumber << std::endl;
        std::cout << "   退房时间: " << Room::NowString() << std::endl;
        std::cout << "   总费用: ¥" << fee << std::endl;
        std::cout << "  ══════════════════════" << std::endl;
    }
}
