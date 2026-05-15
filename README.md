# 程序设计作业 -- 酒店管理系统

## 1. 项目简介

项目采取面向对象设计原理，通过对酒店房间的抽象化，设计一个酒店管理系统。系统功能包括登记入住、查询房间入住情况、查询当前费用、结账退房等操作。界面采用命令行交互方式，后续扩展为图形界面。图形界面采用Qt编写，设计风格参考苹果风格，提升界面美观度和用户体验。

其中代码框架和构建由本人进行，基于Qt的图形界面部分由DeepSeekV4Pro协助实现。

## 2. 部署教程

### 2.1 环境准备

- 安装C++编译器（如g++）和Qt开发环境（如Qt Creator）。
- 确保系统已安装CMake和Git。
- 克隆项目代码库：

    ```bash
    git clone https://github.com/AndyYang12345/HotelManagementSystem.git
    cd HotelManagementSystem
    mkdir build && cd build
    cmake ..
    make
    ./hotel_mgmt_gui # 运行图形界面
    ./hotel_mgmt # 运行命令行界面
    ```

### 2.2 配置文件

- 项目包含一个配置文件`hotel_config.xml`，用于定义酒店名称、房间类型、价格等信息。可以根据需要修改该文件来调整酒店的设置。
- 由于并非运行时读取可能需要重新编译才能生效。

## 开源许可

本项目采用MIT许可证，允许任何人自由使用、修改和分发代码，但需保留原作者信息和许可证声明。
