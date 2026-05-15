#ifndef STANDARD_ROOM_H
#define STANDARD_ROOM_H

#include "room.h"

/**
 * @brief 标准间 — 配备两张 1.2m×2.0m 单人床
 *
 * 经济实惠，适合双人出行的标准选择。
 */
class StandardRoom : public Room {
public:
    StandardRoom(int number, double pricePerHour, double pricePerDay);

    std::string GetDescription() const override;
};

#endif // STANDARD_ROOM_H
