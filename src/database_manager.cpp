#include "database_manager.h"
#include "big_bed_room.h"
#include "standard_room.h"
#include "luxury_room.h"
#include <cstdint>
#include <cstring>

// ============================================================================
// 构造 / 析构
// ============================================================================
DatabaseManager::DatabaseManager() : db_(nullptr) {}

DatabaseManager::~DatabaseManager() {
    Close();
}

// ============================================================================
// Open / Close
// ============================================================================
bool DatabaseManager::Open(const std::string& dbPath) {
    int rc = sqlite3_open(dbPath.c_str(), &db_);
    if (rc != SQLITE_OK) {
        last_error_ = "无法打开数据库: " + std::string(sqlite3_errmsg(db_));
        sqlite3_close(db_);
        db_ = nullptr;
        return false;
    }

    // 启用 WAL 模式提升并发性能
    ExecuteSQL("PRAGMA journal_mode=WAL;");
    // 外键约束
    ExecuteSQL("PRAGMA foreign_keys=ON;");

    return CreateTables();
}

void DatabaseManager::Close() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

// ============================================================================
// ExecuteSQL — 执行无结果集 SQL
// ============================================================================
bool DatabaseManager::ExecuteSQL(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        last_error_ = "SQL 错误: " + std::string(errMsg ? errMsg : "unknown");
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// ============================================================================
// CreateTables
// ============================================================================
bool DatabaseManager::CreateTables() {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS rooms (
            room_number    INTEGER PRIMARY KEY,
            type_name      TEXT    NOT NULL,
            price_per_hour REAL    NOT NULL,
            price_per_day  REAL    NOT NULL,
            is_occupied    INTEGER NOT NULL DEFAULT 0,
            check_in_time  INTEGER NOT NULL DEFAULT 0,
            billing_mode   TEXT    NOT NULL DEFAULT 'HOURLY'
        );
    )";
    return ExecuteSQL(sql);
}

// ============================================================================
// SaveAllRooms — 批量写入（INSERT OR REPLACE）
// ============================================================================
bool DatabaseManager::SaveAllRooms(const std::vector<std::unique_ptr<Room>>& rooms) {
    if (!db_) {
        last_error_ = "数据库未打开";
        return false;
    }

    // 使用事务批量写入
    ExecuteSQL("BEGIN TRANSACTION;");

    const char* insertSQL = R"(
        INSERT OR REPLACE INTO rooms
            (room_number, type_name, price_per_hour, price_per_day,
             is_occupied, check_in_time, billing_mode)
        VALUES (?, ?, ?, ?, ?, ?, ?);
    )";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, insertSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        last_error_ = "预编译 SQL 失败: " + std::string(sqlite3_errmsg(db_));
        ExecuteSQL("ROLLBACK;");
        return false;
    }

    for (const auto& room : rooms) {
        sqlite3_reset(stmt);
        sqlite3_bind_int(stmt,    1, room->GetRoomNumber());
        sqlite3_bind_text(stmt,   2, room->GetTypeName().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 3, room->GetPricePerHour());
        sqlite3_bind_double(stmt, 4, room->GetPricePerDay());
        sqlite3_bind_int(stmt,    5, room->IsOccupied() ? 1 : 0);
        sqlite3_bind_int64(stmt,  6, 0);  // check_in_time will be updated individually
        sqlite3_bind_text(stmt,   7,
            room->IsOccupied() ? (room->GetBillingMode() == BillingMode::HOURLY ? "HOURLY" : "DAILY") : "HOURLY",
            -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            last_error_ = "写入房间 " + std::to_string(room->GetRoomNumber()) +
                          " 失败: " + std::string(sqlite3_errmsg(db_));
            sqlite3_finalize(stmt);
            ExecuteSQL("ROLLBACK;");
            return false;
        }
    }

    sqlite3_finalize(stmt);
    ExecuteSQL("COMMIT;");
    return true;
}

// ============================================================================
// LoadRoomStates — 加载入住状态
// ============================================================================
bool DatabaseManager::LoadRoomStates(std::vector<std::unique_ptr<Room>>& rooms) {
    if (!db_) {
        last_error_ = "数据库未打开";
        return false;
    }

    const char* querySQL = R"(
        SELECT room_number, is_occupied, check_in_time, billing_mode
        FROM rooms;
    )";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, querySQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        last_error_ = "查询失败: " + std::string(sqlite3_errmsg(db_));
        return false;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int roomNumber     = sqlite3_column_int(stmt, 0);
        int isOccupied     = sqlite3_column_int(stmt, 1);
        int64_t checkIn    = sqlite3_column_int64(stmt, 2);
        const char* modeStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        if (isOccupied != 1) continue;

        // 找到对应房间并恢复状态
        for (auto& room : rooms) {
            if (room->GetRoomNumber() == roomNumber) {
                BillingMode mode = (modeStr && std::strcmp(modeStr, "DAILY") == 0)
                                   ? BillingMode::DAILY : BillingMode::HOURLY;
                room->RestoreState(static_cast<std::time_t>(checkIn), mode);
                break;
            }
        }
    }

    sqlite3_finalize(stmt);
    return true;
}

// ============================================================================
// SaveRoomState — 实时保存单房间状态
// ============================================================================
bool DatabaseManager::SaveRoomState(const Room& room) {
    if (!db_) {
        last_error_ = "数据库未打开";
        return false;
    }

    const char* updateSQL = R"(
        INSERT OR REPLACE INTO rooms
            (room_number, type_name, price_per_hour, price_per_day,
             is_occupied, check_in_time, billing_mode)
        VALUES (?, ?, ?, ?, ?, ?, ?);
    )";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, updateSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        last_error_ = "预编译失败: " + std::string(sqlite3_errmsg(db_));
        return false;
    }

    sqlite3_bind_int(stmt,    1, room.GetRoomNumber());
    sqlite3_bind_text(stmt,   2, room.GetTypeName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, room.GetPricePerHour());
    sqlite3_bind_double(stmt, 4, room.GetPricePerDay());
    sqlite3_bind_int(stmt,    5, room.IsOccupied() ? 1 : 0);
    sqlite3_bind_int64(stmt,  6, room.IsOccupied() ? static_cast<int64_t>(room.GetCheckInTime()) : 0);

    const char* modeStr = (room.GetBillingMode() == BillingMode::DAILY) ? "DAILY" : "HOURLY";
    sqlite3_bind_text(stmt, 7, modeStr, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        last_error_ = "更新房间 " + std::to_string(room.GetRoomNumber()) +
                      " 状态失败: " + std::string(sqlite3_errmsg(db_));
        return false;
    }
    return true;
}
