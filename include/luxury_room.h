#ifndef LUXURY_ROOM_H
#define LUXURY_ROOM_H

#include "room.h"

/**
 * @brief 豪华套房 — 独立客厅+卧室，配备 2.0m×2.0m 特大床
 *
 * 高端商务或家庭出行，提供最佳入住体验。
 */
class LuxuryRoom : public Room {
public:
    LuxuryRoom(int number, double pricePerHour, double pricePerDay);

    std::string GetDescription() const override;
};

#endif // LUXURY_ROOM_H
