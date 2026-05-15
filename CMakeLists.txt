cmake_minimum_required(VERSION 3.16)
project(HotelManagementSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找依赖
find_package(tinyxml2 REQUIRED)

# 头文件路径（CLI + 核心库共用）
include_directories(${CMAKE_SOURCE_DIR}/include)

# ============================
# 公共核心源文件（不含 main）
# ============================
set(CORE_SOURCES
    src/room.cpp
    src/big_bed_room.cpp
    src/standard_room.cpp
    src/luxury_room.cpp
    src/hotel.cpp
    src/config_loader.cpp
)

# ============================
# CLI 目标
# ============================
add_executable(hotel_mgmt
    ${CORE_SOURCES}
    src/cli.cpp
    src/main.cpp
)
target_link_libraries(hotel_mgmt PRIVATE tinyxml2::tinyxml2)

# 复制 config 到 CLI build 目录
add_custom_command(TARGET hotel_mgmt POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/config
    $<TARGET_FILE_DIR:hotel_mgmt>/config
)

# ============================
# Qt GUI 目标（可选）
# ============================
find_package(Qt5 COMPONENTS Widgets QUIET)

if(Qt5_FOUND)
    message(STATUS "Qt5 found — building GUI target")

    # GUI 需要访问 hotel.h 等核心头文件，额外加上 gui/ 目录
    include_directories(${CMAKE_SOURCE_DIR}/gui)

    add_executable(hotel_mgmt_gui
        ${CORE_SOURCES}
        gui/main.cpp
        gui/main_window.cpp
        gui/room_card.cpp
    )
    target_link_libraries(hotel_mgmt_gui PRIVATE
        tinyxml2::tinyxml2
        Qt5::Widgets
    )

    # 复制 config 到 GUI build 目录
    add_custom_command(TARGET hotel_mgmt_gui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/config
        $<TARGET_FILE_DIR:hotel_mgmt_gui>/config
    )
else()
    message(STATUS "Qt5 not found — skipping GUI target. Install: sudo apt install qtbase5-dev")
endif()
