#ifndef APPLE_STYLE_H
#define APPLE_STYLE_H

#include <QString>
#include <QFont>

/**
 * @brief Apple 设计风格常量
 *
 * 定义颜色、字体、圆角等苹果风格 UI 参数。
 * 参考 Apple Human Interface Guidelines。
 */
namespace AppleStyle {

// ========== 颜色 ==========
// 系统蓝 (System Blue)
inline constexpr const char* COLOR_PRIMARY   = "#007AFF";
inline constexpr const char* COLOR_PRIMARY_HOVER = "#3395FF";

// 绿色（成功/空闲）
inline constexpr const char* COLOR_SUCCESS   = "#34C759";
// 红色（已入住/危险）
inline constexpr const char* COLOR_DANGER    = "#FF3B30";
// 橙色（警告）
inline constexpr const char* COLOR_WARNING   = "#FF9500";

// 中性色
inline constexpr const char* COLOR_BG        = "#F2F2F7";   // 系统背景
inline constexpr const char* COLOR_CARD_BG   = "#FFFFFF";   // 卡片背景
inline constexpr const char* COLOR_TEXT      = "#1C1C1E";   // 主文字
inline constexpr const char* COLOR_TEXT_SEC  = "#8E8E93";   // 次要文字
inline constexpr const char* COLOR_SEPARATOR = "#C6C6C8";   // 分隔线

// ========== 圆角 ==========
inline constexpr int RADIUS_SMALL  = 8;
inline constexpr int RADIUS_MEDIUM = 12;
inline constexpr int RADIUS_LARGE  = 16;

// ========== 字体 ==========
inline QFont TitleFont() {
    QFont f("SF Pro Display, Helvetica Neue, sans-serif", 28, QFont::Bold);
    return f;
}
inline QFont HeadingFont() {
    QFont f("SF Pro Display, Helvetica Neue, sans-serif", 17, QFont::DemiBold);
    return f;
}
inline QFont BodyFont() {
    QFont f("SF Pro Text, Helvetica Neue, sans-serif", 14);
    return f;
}
inline QFont CaptionFont() {
    QFont f("SF Pro Text, Helvetica Neue, sans-serif", 12);
    return f;
}

// ========== 样式表片段 ==========
/// 苹果风格主按钮
inline QString PrimaryButtonStyle() {
    return QString(
        "QPushButton {"
        "  background-color: %1;"
        "  color: white;"
        "  border: none;"
        "  border-radius: %2px;"
        "  padding: 10px 24px;"
        "  font-size: 14px;"
        "  font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "  background-color: %3;"
        "}"
        "QPushButton:pressed {"
        "  background-color: %1;"
        "  opacity: 0.8;"
        "}"
    ).arg(COLOR_PRIMARY).arg(RADIUS_SMALL).arg(COLOR_PRIMARY_HOVER);
}

/// 苹果风格次要按钮
inline QString SecondaryButtonStyle() {
    return QString(
        "QPushButton {"
        "  background-color: %1;"
        "  color: %2;"
        "  border: 1px solid %3;"
        "  border-radius: %4px;"
        "  padding: 10px 24px;"
        "  font-size: 14px;"
        "  font-weight: 500;"
        "}"
        "QPushButton:hover {"
        "  background-color: #E5E5EA;"
        "}"
    ).arg(COLOR_CARD_BG).arg(COLOR_TEXT).arg(COLOR_SEPARATOR).arg(RADIUS_SMALL);
}

/// 苹果风格危险按钮
inline QString DangerButtonStyle() {
    return QString(
        "QPushButton {"
        "  background-color: %1;"
        "  color: white;"
        "  border: none;"
        "  border-radius: %2px;"
        "  padding: 10px 24px;"
        "  font-size: 14px;"
        "  font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "  background-color: #FF5E55;"
        "}"
    ).arg(COLOR_DANGER).arg(RADIUS_SMALL);
}

/// 卡片容器样式
inline QString CardStyle() {
    return QString(
        "background-color: %1;"
        "border-radius: %2px;"
    ).arg(COLOR_CARD_BG).arg(RADIUS_MEDIUM);
}

} // namespace AppleStyle

#endif // APPLE_STYLE_H
