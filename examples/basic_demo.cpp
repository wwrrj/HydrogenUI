#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "HydrogenUI.h"
#include "hal/hal_u8g2.h"

// 定义 U8g2 实例 (请根据你的硬件选择合适的驱动)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// HydrogenUI 组件指针
Hydrogen::U8g2HAL* hal;
Hydrogen::Graphics* graphics;
Hydrogen::Button* btn;

void setup() {
    Serial.begin(115200);

    // 1. 初始化 HAL (硬件抽象层)
    // 这是最底层的手动初始化方式，适合需要深度定制的场景
    hal = new Hydrogen::U8g2HAL(&u8g2);
    hal->init();

    // 2. 初始化图形引擎
    graphics = new Hydrogen::Graphics(hal);

    // 3. 创建 UI 控件
    // 手动管理控件生命周期
    btn = new Hydrogen::Button(10, 10, 60, 20, "点击我");

    // 设置字体 (必须支持中文才能显示中文)
    u8g2.setFont(u8g2_font_wqy12_t_chinese1);
    u8g2.enableUTF8Print();
}

void loop() {
    hal->clear();

    // 手动绘制 UI
    // 这种方式不使用 App 管理器，需要手动调用 draw
    btn->draw(*graphics);

    // 直接使用图形引擎绘制基本图形
    graphics->drawCircle(100, 32, 10);

    hal->update();
    delay(100);
}
