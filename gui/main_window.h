#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <vector>
#include "hotel.h"
#include "room_card.h"

/**
 * @brief 酒店管理系统主窗口（苹果风格）
 *
 * 顶部：酒店名称 + 统计信息
 * 中部：房间卡片网格
 * 底部：操作按钮栏
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

    /// 初始化酒店数据
    bool Initialize(const std::string& configPath);

private slots:
    void OnCheckIn(int roomNumber);
    void OnCheckOut(int roomNumber);
    void OnQueryFee(int roomNumber);
    void OnRefreshAll();

private:
    void SetupUI();
    void RefreshStats();
    void RefreshAllCards();

    Hotel hotel_;

    // 顶部控件
    QLabel* hotel_name_label_;
    QLabel* stats_label_;

    // 卡片区域
    QWidget* cards_container_;
    QGridLayout* cards_grid_;
    std::vector<RoomCard*> cards_;

    // 底部按钮
    QPushButton* refresh_btn_;
};

#endif // MAIN_WINDOW_H
