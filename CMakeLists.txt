cmake_minimum_required(VERSION 3.16)
project(HotelManagementSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# 查找 tinyxml2
find_package(tinyxml2 REQUIRED)

# 头文件路径
include_directories(${CMAKE_SOURCE_DIR}/include)

# 源文件
set(SOURCES
    src/room.cpp
    src/big_bed_room.cpp
    src/standard_room.cpp
    src/luxury_room.cpp
    src/config_loader.cpp
    src/main.cpp
)

# 可执行文件
add_executable(hotel_mgmt ${SOURCES})

# 链接库
target_link_libraries(hotel_mgmt PRIVATE tinyxml2::tinyxml2)

# 复制 config 目录到 build
add_custom_command(TARGET hotel_mgmt POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/config
    $<TARGET_FILE_DIR:hotel_mgmt>/config
)
