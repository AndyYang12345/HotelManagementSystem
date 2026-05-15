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
    // 卡片外观
    setFixedSize(200, 180);
    setStyleSheet(AppleStyle::CardStyle());
    setCursor(Qt::PointingHandCursor);

    // 阴影效果
    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 30));
    setGraphicsEffect(shadow);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(14, 12, 14, 12);
    layout->setSpacing(4);

    // 顶部：图标 + 房间号
    auto* topLayout = new QHBoxLayout();
    icon_label_ = new QLabel("🛏️");
    icon_label_->setFont(QFont("sans-serif", 22));
    topLayout->addWidget(icon_label_);

    topLayout->addStretch();

    room_number_label_ = new QLabel(QString::number(room_->GetRoomNumber()));
    room_number_label_->setFont(AppleStyle::HeadingFont());
    room_number_label_->setStyleSheet("color: " + QString(AppleStyle::COLOR_TEXT) + ";");
    topLayout->addWidget(room_number_label_);
    layout->addLayout(topLayout);

    // 类型
    type_label_ = new QLabel(QString::fromStdString(room_->GetTypeName()));
    type_label_->setFont(AppleStyle::CaptionFont());
    type_label_->setStyleSheet("color: " + QString(AppleStyle::COLOR_TEXT_SEC) + ";");
    layout->addWidget(type_label_);

    // 价格
    QString priceText = QString("¥%1/h  ¥%2/d")
        .arg(static_cast<int>(room_->GetPricePerHour()))
        .arg(static_cast<int>(room_->GetPricePerDay()));
    price_label_ = new QLabel(priceText);
    price_label_->setFont(AppleStyle::CaptionFont());
    price_label_->setStyleSheet("color: " + QString(AppleStyle::COLOR_TEXT_SEC) + ";");
    layout->addWidget(price_label_);

    // 状态指示
    status_indicator_ = new QLabel();
    status_indicator_->setFont(AppleStyle::BodyFont());
    layout->addWidget(status_indicator_);

    // 详情（入住时间等）
    detail_label_ = new QLabel();
    detail_label_->setFont(QFont("sans-serif", 10));
    detail_label_->setStyleSheet("color: " + QString(AppleStyle::COLOR_TEXT_SEC) + ";");
    detail_label_->setWordWrap(true);
    layout->addWidget(detail_label_);

    layout->addStretch();

    // 操作按钮行
    auto* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(6);

    action_btn1_ = new QPushButton();
    action_btn1_->setFont(AppleStyle::CaptionFont());
    action_btn1_->setCursor(Qt::PointingHandCursor);
    action_btn1_->setFixedHeight(28);
    btnLayout->addWidget(action_btn1_);

    action_btn2_ = new QPushButton();
    action_btn2_->setFont(AppleStyle::CaptionFont());
    action_btn2_->setCursor(Qt::PointingHandCursor);
    action_btn2_->setFixedHeight(28);
    btnLayout->addWidget(action_btn2_);

    layout->addLayout(btnLayout);

    // 按钮信号连接
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

void RoomCard::Refresh() {
    UpdateStatusDisplay();
}

void RoomCard::UpdateStatusDisplay() {
    bool occupied = room_->IsOccupied();

    if (occupied) {
        // 已入住状态
        icon_label_->setText("🚫");
        status_indicator_->setText("🔴 已入住");
        status_indicator_->setStyleSheet(
            "color: " + QString(AppleStyle::COLOR_DANGER) + "; font-weight: 600;");

        QString detail = QString("入住: %1\n计费: %2\n当前: ¥%3")
            .arg(QString::fromStdString(room_->GetCheckInTimeStr()).mid(11))  // 只显示时间
            .arg(BillingModeToString(room_->GetBillingMode()).c_str())
            .arg(room_->QueryCurrentFee(), 0, 'f', 0);
        detail_label_->setText(detail);

        // 按钮：查询费用 + 退房
        action_btn1_->setText("💰 查询费用");
        action_btn1_->setStyleSheet(
            "QPushButton { background-color: " + QString(AppleStyle::COLOR_PRIMARY) +
            "; color: white; border-radius: 6px; padding: 4px 10px; }"
            "QPushButton:hover { background-color: " + QString(AppleStyle::COLOR_PRIMARY_HOVER) + "; }");

        action_btn2_->setText("🏁 退房");
        action_btn2_->setStyleSheet(
            "QPushButton { background-color: " + QString(AppleStyle::COLOR_DANGER) +
            "; color: white; border-radius: 6px; padding: 4px 10px; }"
            "QPushButton:hover { background-color: #FF5E55; }");
        action_btn2_->setVisible(true);
    } else {
        // 空闲状态
        icon_label_->setText("🛏️");
        status_indicator_->setText("🟢 空闲");
        status_indicator_->setStyleSheet(
            "color: " + QString(AppleStyle::COLOR_SUCCESS) + "; font-weight: 600;");
        detail_label_->setText("");

        // 按钮：入住
        action_btn1_->setText("🏨 登记入住");
        action_btn1_->setStyleSheet(
            "QPushButton { background-color: " + QString(AppleStyle::COLOR_PRIMARY) +
            "; color: white; border-radius: 6px; padding: 4px 10px; }"
            "QPushButton:hover { background-color: " + QString(AppleStyle::COLOR_PRIMARY_HOVER) + "; }");

        action_btn2_->setVisible(false);
    }
}
