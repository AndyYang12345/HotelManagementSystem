#include "main_window.h"
#include "apple_style.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QApplication>
#include <QScreen>
#include <QScrollBar>

// ============================================================================
// 构造 & 初始化
// ============================================================================
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
}

bool MainWindow::Initialize(const std::string& configPath) {
    if (!hotel_.Initialize(configPath)) {
        QMessageBox::critical(this, "初始化失败",
            QString::fromStdString(hotel_.GetLastError()));
        return false;
    }
    SetupUI();
    return true;
}

// ============================================================================
// UI 搭建
// ============================================================================
void MainWindow::SetupUI() {
    // ---- 窗口设置 ----
    setWindowTitle(QString::fromStdString(hotel_.GetName()) + " — 酒店管理系统");
    setMinimumSize(920, 640);

    // 居中显示
    QScreen* screen = QApplication::primaryScreen();
    QRect screenRect = screen->availableGeometry();
    int w = std::min(1100, screenRect.width() - 100);
    int h = std::min(780, screenRect.height() - 100);
    resize(w, h);
    move((screenRect.width() - w) / 2, (screenRect.height() - h) / 2);

    // 全局背景
    setStyleSheet("QMainWindow { background-color: " +
                  QString(AppleStyle::COLOR_BG) + "; }");

    // ---- 中央控件 ----
    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(32, 24, 32, 20);
    mainLayout->setSpacing(16);

    // ======== 顶部：标题 + 统计 ========
    auto* headerLayout = new QHBoxLayout();

    auto* titleCol = new QVBoxLayout();
    hotel_name_label_ = new QLabel(QString::fromStdString(hotel_.GetName()));
    hotel_name_label_->setFont(AppleStyle::TitleFont());
    hotel_name_label_->setStyleSheet("color: " + QString(AppleStyle::COLOR_TEXT) + ";");
    titleCol->addWidget(hotel_name_label_);

    stats_label_ = new QLabel();
    stats_label_->setFont(AppleStyle::BodyFont());
    stats_label_->setStyleSheet("color: " + QString(AppleStyle::COLOR_TEXT_SEC) + ";");
    titleCol->addWidget(stats_label_);

    headerLayout->addLayout(titleCol);
    headerLayout->addStretch();

    // 刷新按钮
    refresh_btn_ = new QPushButton("🔄 刷新");
    refresh_btn_->setStyleSheet(AppleStyle::SecondaryButtonStyle());
    refresh_btn_->setFont(AppleStyle::BodyFont());
    refresh_btn_->setCursor(Qt::PointingHandCursor);
    refresh_btn_->setFixedHeight(38);
    connect(refresh_btn_, &QPushButton::clicked, this, &MainWindow::OnRefreshAll);
    headerLayout->addWidget(refresh_btn_);

    mainLayout->addLayout(headerLayout);

    RefreshStats();

    // ======== 中部：房间卡片网格（可滚动） ========
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; }"
                              "QScrollBar:vertical {"
                              "  width: 6px; background: transparent; }"
                              "QScrollBar::handle:vertical {"
                              "  background: #C6C6C8; border-radius: 3px; }");

    cards_container_ = new QWidget();
    cards_container_->setStyleSheet("background: transparent;");
    cards_grid_ = new QGridLayout(cards_container_);
    cards_grid_->setSpacing(14);
    cards_grid_->setContentsMargins(0, 4, 0, 4);

    // 创建所有房间卡片
    int col = 0, row = 0;
    const int COLS = 4;  // 每行4张卡片

    for (int i = 0; i < hotel_.GetTotalRooms(); ++i) {
        Room* room = hotel_.FindRoom(101 + i);
        if (!room) continue;

        auto* card = new RoomCard(room, cards_container_);
        cards_.push_back(card);

        connect(card, &RoomCard::CheckInRequested, this, &MainWindow::OnCheckIn);
        connect(card, &RoomCard::CheckOutRequested, this, &MainWindow::OnCheckOut);
        connect(card, &RoomCard::QueryFeeRequested, this, &MainWindow::OnQueryFee);

        cards_grid_->addWidget(card, row, col, Qt::AlignCenter);

        ++col;
        if (col >= COLS) {
            col = 0;
            ++row;
        }
    }

    scrollArea->setWidget(cards_container_);
    mainLayout->addWidget(scrollArea, 1);  // stretch factor 1

    // ======== 底部：状态栏 ========
    auto* footerLabel = new QLabel("v1.0 · 数据实时更新 · 点击卡片即可操作");
    footerLabel->setFont(AppleStyle::CaptionFont());
    footerLabel->setStyleSheet("color: " + QString(AppleStyle::COLOR_TEXT_SEC) + ";");
    footerLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(footerLabel);
}

// ============================================================================
// 统计刷新
// ============================================================================
void MainWindow::RefreshStats() {
    stats_label_->setText(QString("共 %1 间客房 · %2 空闲 · %3 入住中")
        .arg(hotel_.GetTotalRooms())
        .arg(hotel_.GetAvailableCount())
        .arg(hotel_.GetOccupiedCount()));
}

void MainWindow::RefreshAllCards() {
    RefreshStats();
    for (auto* card : cards_) {
        card->Refresh();
    }
}

void MainWindow::OnRefreshAll() {
    RefreshAllCards();
}

// ============================================================================
// 登记入住对话框
// ============================================================================
void MainWindow::OnCheckIn(int roomNumber) {
    Room* room = hotel_.FindRoom(roomNumber);
    if (!room || room->IsOccupied()) {
        QMessageBox::warning(this, "操作失败", "该房间不可入住。");
        return;
    }

    // 创建对话框
    QDialog dialog(this);
    dialog.setWindowTitle("登记入住");
    dialog.setFixedSize(380, 240);
    dialog.setStyleSheet("QDialog { background-color: #FFFFFF; border-radius: 16px; }");
    dialog.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    auto* dlgLayout = new QVBoxLayout(&dialog);
    dlgLayout->setContentsMargins(24, 20, 24, 20);
    dlgLayout->setSpacing(14);

    // 标题
    auto* titleLabel = new QLabel(
        QString("房间 %1 — %2").arg(roomNumber).arg(room->GetTypeName().c_str()));
    titleLabel->setFont(AppleStyle::HeadingFont());
    titleLabel->setStyleSheet("color: " + QString(AppleStyle::COLOR_TEXT) + ";");
    dlgLayout->addWidget(titleLabel);

    // 价格信息
    auto* priceLabel = new QLabel(
        QString("单价：¥%1 / 小时  |  ¥%2 / 天")
            .arg(static_cast<int>(room->GetPricePerHour()))
            .arg(static_cast<int>(room->GetPricePerDay())));
    priceLabel->setFont(AppleStyle::BodyFont());
    priceLabel->setStyleSheet("color: " + QString(AppleStyle::COLOR_TEXT_SEC) + ";");
    dlgLayout->addWidget(priceLabel);

    // 计费方式选择
    auto* modeLabel = new QLabel("选择计费方式：");
    modeLabel->setFont(AppleStyle::BodyFont());
    dlgLayout->addWidget(modeLabel);

    auto* btnGroup = new QButtonGroup(&dialog);
    auto* radioHourly = new QRadioButton("按小时计费");
    auto* radioDaily  = new QRadioButton("按天计费");
    radioDaily->setChecked(true);

    radioHourly->setFont(AppleStyle::BodyFont());
    radioDaily->setFont(AppleStyle::BodyFont());
    radioHourly->setStyleSheet("color: " + QString(AppleStyle::COLOR_TEXT) + ";");
    radioDaily->setStyleSheet("color: " + QString(AppleStyle::COLOR_TEXT) + ";");

    btnGroup->addButton(radioHourly);
    btnGroup->addButton(radioDaily);

    auto* radioLayout = new QHBoxLayout();
    radioLayout->addWidget(radioHourly);
    radioLayout->addWidget(radioDaily);
    radioLayout->addStretch();
    dlgLayout->addLayout(radioLayout);

    dlgLayout->addStretch();

    // 按钮
    auto* btnBox = new QDialogButtonBox();
    auto* confirmBtn = new QPushButton("确认入住");
    confirmBtn->setStyleSheet(AppleStyle::PrimaryButtonStyle());
    confirmBtn->setCursor(Qt::PointingHandCursor);

    auto* cancelBtn = new QPushButton("取消");
    cancelBtn->setStyleSheet(AppleStyle::SecondaryButtonStyle());
    cancelBtn->setCursor(Qt::PointingHandCursor);

    btnBox->addButton(confirmBtn, QDialogButtonBox::AcceptRole);
    btnBox->addButton(cancelBtn, QDialogButtonBox::RejectRole);
    dlgLayout->addWidget(btnBox);

    connect(btnBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        BillingMode mode = radioHourly->isChecked() ? BillingMode::HOURLY : BillingMode::DAILY;
        if (hotel_.CheckIn(roomNumber, mode)) {
            RefreshAllCards();
            QMessageBox::information(this, "入住成功",
                QString("房间 %1 登记成功！\n计费方式：%2\n入住时间：%3")
                    .arg(roomNumber)
                    .arg(BillingModeToString(mode).c_str())
                    .arg(Room::NowString().c_str()));
        } else {
            QMessageBox::warning(this, "入住失败",
                QString::fromStdString(hotel_.GetLastError()));
        }
    }
}

// ============================================================================
// 结账退房
// ============================================================================
void MainWindow::OnCheckOut(int roomNumber) {
    Room* room = hotel_.FindRoom(roomNumber);
    if (!room || !room->IsOccupied()) {
        QMessageBox::warning(this, "操作失败", "该房间未入住，无需退房。");
        return;
    }

    double currentFee = room->QueryCurrentFee();

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("确认退房");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(QString("确认房间 %1 退房？").arg(roomNumber));
    msgBox.setInformativeText(QString("入住时间：%1\n计费方式：%2\n当前费用：¥%3\n\n退房后将按当前时间结算。")
        .arg(room->GetCheckInTimeStr().c_str())
        .arg(BillingModeToString(room->GetBillingMode()).c_str())
        .arg(currentFee, 0, 'f', 0));

    auto* confirmBtn = new QPushButton("确认退房");
    confirmBtn->setStyleSheet(AppleStyle::DangerButtonStyle());
    auto* cancelBtn = new QPushButton("取消");
    cancelBtn->setStyleSheet(AppleStyle::SecondaryButtonStyle());

    msgBox.addButton(confirmBtn, QMessageBox::AcceptRole);
    msgBox.addButton(cancelBtn, QMessageBox::RejectRole);
    msgBox.setDefaultButton(cancelBtn);

    // 设置样式
    msgBox.setStyleSheet("QMessageBox { background-color: #FFFFFF; }"
                         "QLabel { color: " + QString(AppleStyle::COLOR_TEXT) + "; }");

    msgBox.exec();

    if (msgBox.clickedButton() == confirmBtn) {
        double totalFee = hotel_.CheckOut(roomNumber);
        RefreshAllCards();
        QMessageBox::information(this, "退房成功",
            QString("房间 %1 退房成功！\n退房时间：%2\n总费用：¥%3")
                .arg(roomNumber)
                .arg(Room::NowString().c_str())
                .arg(totalFee, 0, 'f', 0));
    }
}

// ============================================================================
// 查询费用
// ============================================================================
void MainWindow::OnQueryFee(int roomNumber) {
    Room* room = hotel_.FindRoom(roomNumber);
    if (!room || !room->IsOccupied()) {
        QMessageBox::information(this, "查询费用",
            QString("房间 %1 当前未入住，无费用。").arg(roomNumber));
        return;
    }

    double fee = room->QueryCurrentFee();
    QMessageBox::information(this, "查询费用",
        QString("房间 %1 (%2)\n计费方式：%3\n入住时间：%4\n当前费用：¥%5")
            .arg(roomNumber)
            .arg(room->GetTypeName().c_str())
            .arg(BillingModeToString(room->GetBillingMode()).c_str())
            .arg(room->GetCheckInTimeStr().c_str())
            .arg(fee, 0, 'f', 0));
}
