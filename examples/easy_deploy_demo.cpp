#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "HydrogenUI.h"

// 定义 U8g2 实例
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void setup() {
    Serial.begin(115200);

    // 一行代码部署！
    // 自动完成 HAL 绑定、图形库初始化、屏幕启动
    Hydrogen::deploy(u8g2);

    // 配置中文字体
    u8g2.setFont(u8g2_font_wqy12_t_chinese1);
    u8g2.enableUTF8Print();

    // 直接向全局 App 添加控件
    Hydrogen::App.add(new Hydrogen::Button(10, 10, 80, 24, "极简部署"));
    Hydrogen::App.add(new Hydrogen::Label(10, 40, "你好 世界"));
}

void loop() {
    // 一行代码更新！
    // 自动处理清屏、绘制、刷新
    Hydrogen::App.update();
    delay(50);
}
