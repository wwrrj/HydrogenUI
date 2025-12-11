#include <Arduino.h>
#include <U8g2lib.h>
#include "HydrogenUI.h"

// 1. U8g2 驱动
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// 2. 全局列表控件指针
Hydrogen::List* menuList;

// 3. 模拟编码器状态
unsigned long lastEncoderEvent = 0;
const unsigned long ENCODER_INTERVAL = 1000; // 每 1000ms 模拟一次旋转

void setup() {
    // 部署框架
    Hydrogen::deploy(u8g2);
    
    // 配置中文支持
    u8g2.setFont(u8g2_font_wqy12_t_chinese1);
    u8g2.enableUTF8Print();

    // 创建列表控件 (全屏减去内边距)
    menuList = new Hydrogen::List(0, 0, 128, 64);
    
    // 添加列表项
    menuList->addItem("仪表盘");
    menuList->addItem("WiFi 设置");
    menuList->addItem("蓝牙连接");
    menuList->addItem("消息通知");
    menuList->addItem("系统信息");
    menuList->addItem("关于设备");
    menuList->addItem("重启系统");
    
    // 添加到应用程序
    Hydrogen::App.add(menuList);
}

void loop() {
    // 4. 更新 UI
    Hydrogen::App.update();
    
    // 5. 模拟编码器旋转
    if (millis() - lastEncoderEvent > ENCODER_INTERVAL) {
        lastEncoderEvent = millis();
        
        // 模拟 "下一步" 动作 (顺时针旋转)
        menuList->next();
        
        // 可选: 打印调试信息
        // Serial.print("Selected: ");
        // Serial.println(menuList->getSelectedItem().c_str());
    }
}
