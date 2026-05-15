#include "config_loader.h"
#include <tinyxml2.h>
#include <iostream>

// 使用 tinyxml2 命名空间
using namespace tinyxml2;

bool ConfigLoader::Load(const std::string& filePath) {
    XMLDocument doc;
    XMLError err = doc.LoadFile(filePath.c_str());

    if (err != XML_SUCCESS) {
        last_error_ = "无法打开 XML 文件: " + filePath;
        return false;
    }

    // 根节点 <hotel_config>
    XMLElement* root = doc.RootElement();
    if (!root) {
        last_error_ = "缺少根元素 <hotel_config>";
        return false;
    }

    // 读取酒店名称
    XMLElement* nameElem = root->FirstChildElement("hotel_name");
    if (nameElem && nameElem->GetText()) {
        hotel_name_ = nameElem->GetText();
    } else {
        hotel_name_ = "未命名酒店";
    }

    // 读取 <room_types>
    XMLElement* typesElem = root->FirstChildElement("room_types");
    if (!typesElem) {
        last_error_ = "缺少 <room_types> 元素";
        return false;
    }

    room_types_.clear();
    for (XMLElement* typeElem = typesElem->FirstChildElement("type");
         typeElem != nullptr;
         typeElem = typeElem->NextSiblingElement("type")) {

        RoomTypeConfig cfg;
        const char* name  = typeElem->Attribute("name");
        const char* label = typeElem->Attribute("label");
        double priceHour  = typeElem->DoubleAttribute("price_per_hour");
        double priceDay   = typeElem->DoubleAttribute("price_per_day");
        int count         = typeElem->IntAttribute("count");

        if (!name || !label) {
            last_error_ = "房间类型缺少 name 或 label 属性";
            return false;
        }
        if (priceHour <= 0 || priceDay <= 0) {
            last_error_ = std::string("房间类型 ") + label + " 价格必须大于0";
            return false;
        }
        if (count <= 0) {
            last_error_ = std::string("房间类型 ") + label + " 数量必须大于0";
            return false;
        }

        cfg.typeName    = name;
        cfg.label       = label;
        cfg.pricePerHour = priceHour;
        cfg.pricePerDay  = priceDay;
        cfg.count       = count;

        room_types_.push_back(cfg);
    }

    if (room_types_.empty()) {
        last_error_ = "未定义任何房间类型";
        return false;
    }

    return true;
}
