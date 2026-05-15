#ifndef BIG_BED_ROOM_H
#define BIG_BED_ROOM_H

#include "room.h"

/**
 * @brief 大床房 — 配备一张 1.8m×2.0m 大床
 *
 * 适合单人商务或情侣入住，空间宽敞舒适。
 */
class BigBedRoom : public Room {
public:
    BigBedRoom(int number, double pricePerHour, double pricePerDay);

    std::string GetDescription() const override;
};

#endif // BIG_BED_ROOM_H
