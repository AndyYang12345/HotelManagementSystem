#ifndef ROOM_CARD_H
#define ROOM_CARD_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include "room.h"

/**
 * @brief 房间卡片组件（苹果风格）
 *
 * 每个卡片展示一个房间的完整信息：
 * - 房间号 + 类型图标
 * - 状态指示（空闲/已入住）
 * - 价格信息
 * - 已入住时：入住时间、计费方式、当前费用
 * - 操作按钮（入住/退房/查费）
 */
class RoomCard : public QFrame {
    Q_OBJECT

public:
    explicit RoomCard(Room* room, QWidget* parent = nullptr);
    ~RoomCard() override = default;

    /// 刷新卡片显示（当房间状态变化时调用）
    void Refresh();

    int GetRoomNumber() const { return room_->GetRoomNumber(); }

signals:
    /// 请求登记入住
    void CheckInRequested(int roomNumber);
    /// 请求结账退房
    void CheckOutRequested(int roomNumber);
    /// 请求查询费用
    void QueryFeeRequested(int roomNumber);

private:
    void SetupUI();
    void UpdateStatusDisplay();

    Room* room_;

    // 子控件
    QLabel* icon_label_;
    QLabel* room_number_label_;
    QLabel* type_label_;
    QLabel* status_indicator_;
    QLabel* price_label_;
    QLabel* detail_label_;
    QPushButton* action_btn1_;
    QPushButton* action_btn2_;
};

#endif // ROOM_CARD_H
