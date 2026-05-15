#include "room_card.h"
#include "apple_style.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>

RoomCard::RoomCard(Room* room, QWidget* parent)
    : QFrame(parent), room_(room)
{
    SetupUI();
    Refresh();
}

void RoomCard::SetupUI() {
    // 卡片外观 — 加大尺寸以确保文字清晰
    setFixedSize(230, 215);
    setStyleSheet(AppleStyle::CardStyle());
    setCursor(Qt::PointingHandCursor);

    // 阴影效果
    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 30));
    setGraphicsEffect(shadow);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(4);

    // ======== 第一行：彩色状态圆点 + 类型名 + 房间号 ========
    auto* topLayout = new QHBoxLayout();
    topLayout->setSpacing(8);

    // 状态圆点（用 QLabel + border-radius 模拟，稳定渲染）
    icon_label_ = new QLabel();
    icon_label_->setFixedSize(12, 12);
    icon_label_->setStyleSheet(
        "background-color: " + QString(AppleStyle::COLOR_SUCCESS) +
        "; border-radius: 6px; min-width: 12px; min-height: 12px;");
    topLayout->addWidget(icon_label_);

    // 房间类型文字
    type_label_ = new QLabel(QString::fromStdString(room_->GetTypeName()));
    type_label_->setFont(AppleStyle::BodyFont());
    type_label_->setStyleSheet("color: " + QString(AppleStyle::COLOR_TEXT_SEC) + ";");
    topLayout->addWidget(type_label_);

    topLayout->addStretch();

    // 房间号
    room_number_label_ = new QLabel(QString::number(room_->GetRoomNumber()));
    QFont numFont = AppleStyle::HeadingFont();
    numFont.setPointSize(18);
    room_number_label_->setFont(numFont);
    room_number_label_->setStyleSheet("color: " + QString(AppleStyle::COLOR_TEXT) + ";");
    topLayout->addWidget(room_number_label_);

    layout->addLayout(topLayout);

    // ======== 价格信息 ========
    QString priceText = QString("¥%1 / 小时  |  ¥%2 / 天")
        .arg(static_cast<int>(room_->GetPricePerHour()))
        .arg(static_cast<int>(room_->GetPricePerDay()));
    price_label_ = new QLabel(priceText);
    price_label_->setFont(AppleStyle::CaptionFont());
    price_label_->setStyleSheet("color: " + QString(AppleStyle::COLOR_TEXT_SEC) + ";");
    layout->addWidget(price_label_);

    // ======== 状态文字 ========
    status_indicator_ = new QLabel();
    QFont statusFont = AppleStyle::BodyFont();
    statusFont.setPointSize(12);
    status_indicator_->setFont(statusFont);
    layout->addWidget(status_indicator_);

    // ======== 详情区域 ========
    detail_label_ = new QLabel();
    detail_label_->setFont(AppleStyle::CaptionFont());
    detail_label_->setStyleSheet("color: " + QString(AppleStyle::COLOR_TEXT_SEC) +
                                 "; padding: 0px;");
    detail_label_->setWordWrap(true);
    detail_label_->setMinimumHeight(36);
    layout->addWidget(detail_label_);

    layout->addStretch();

    // ======== 操作按钮行 ========
    auto* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(8);

    action_btn1_ = new QPushButton();
    action_btn1_->setFont(AppleStyle::CaptionFont());
    action_btn1_->setCursor(Qt::PointingHandCursor);
    action_btn1_->setMinimumWidth(80);
    action_btn1_->setFixedHeight(30);
    btnLayout->addWidget(action_btn1_);

    action_btn2_ = new QPushButton();
    action_btn2_->setFont(AppleStyle::CaptionFont());
    action_btn2_->setCursor(Qt::PointingHandCursor);
    action_btn2_->setMinimumWidth(70);
    action_btn2_->setFixedHeight(30);
    btnLayout->addWidget(action_btn2_);

    layout->addLayout(btnLayout);

    // ======== 按钮信号连接 ========
    connect(action_btn1_, &QPushButton::clicked, this, [this]() {
        if (room_->IsOccupied()) {
            emit QueryFeeRequested(room_->GetRoomNumber());
        } else {
            emit CheckInRequested(room_->GetRoomNumber());
        }
    });

    connect(action_btn2_, &QPushButton::clicked, this, [this]() {
        if (room_->IsOccupied()) {
            emit CheckOutRequested(room_->GetRoomNumber());
        }
    });
}

// ============================================================================
// 刷新显示
// ============================================================================
void RoomCard::Refresh() {
    UpdateStatusDisplay();
}

void RoomCard::UpdateStatusDisplay() {
    bool occupied = room_->IsOccupied();

    if (occupied) {
        // ---- 已入住状态 ----
        // 圆点变红
        icon_label_->setStyleSheet(
            "background-color: " + QString(AppleStyle::COLOR_DANGER) +
            "; border-radius: 6px; min-width: 12px; min-height: 12px;");

        // 状态文字
        status_indicator_->setText("● 已入住");
        status_indicator_->setStyleSheet(
            "color: " + QString(AppleStyle::COLOR_DANGER) +
            "; font-weight: 600; font-size: 12px;");

        // 详情
        QString detail = QString("入住: %1\n计费: %2\n当前: ¥%3")
            .arg(QString::fromStdString(room_->GetCheckInTimeStr()).mid(11))
            .arg(BillingModeToString(room_->GetBillingMode()).c_str())
            .arg(room_->QueryCurrentFee(), 0, 'f', 0);
        detail_label_->setText(detail);

        // 左按钮：查询费用
        action_btn1_->setText("查询费用");
        action_btn1_->setStyleSheet(
            "QPushButton { background-color: " + QString(AppleStyle::COLOR_PRIMARY) +
            "; color: white; border: none; border-radius: 6px; padding: 4px 12px; }"
            "QPushButton:hover { background-color: " + QString(AppleStyle::COLOR_PRIMARY_HOVER) + "; }");

        // 右按钮：退房
        action_btn2_->setText("退房");
        action_btn2_->setStyleSheet(
            "QPushButton { background-color: " + QString(AppleStyle::COLOR_DANGER) +
            "; color: white; border: none; border-radius: 6px; padding: 4px 12px; }"
            "QPushButton:hover { background-color: #FF5E55; }");
        action_btn2_->setVisible(true);
    } else {
        // ---- 空闲状态 ----
        // 圆点变绿
        icon_label_->setStyleSheet(
            "background-color: " + QString(AppleStyle::COLOR_SUCCESS) +
            "; border-radius: 6px; min-width: 12px; min-height: 12px;");

        // 状态文字
        status_indicator_->setText("● 空闲");
        status_indicator_->setStyleSheet(
            "color: " + QString(AppleStyle::COLOR_SUCCESS) +
            "; font-weight: 600; font-size: 12px;");

        // 详情占位
        detail_label_->setText("可预订");

        // 左按钮：登记入住
        action_btn1_->setText("登记入住");
        action_btn1_->setStyleSheet(
            "QPushButton { background-color: " + QString(AppleStyle::COLOR_PRIMARY) +
            "; color: white; border: none; border-radius: 6px; padding: 4px 12px; }"
            "QPushButton:hover { background-color: " + QString(AppleStyle::COLOR_PRIMARY_HOVER) + "; }");

        // 隐藏右按钮
        action_btn2_->setVisible(false);
    }
}
