# HydrogenUI

HydrogenUI 是一个专为嵌入式设备（如 STM32, ESP32, Arduino）设计的**轻量级、高性能、跨平台 UI 框架**。

它旨在提供**丝滑的动画体验**（即使在低性能 MCU 上）和**极简的开发流程**，同时保持对底层硬件的完全解耦。

## ✨ 核心特性

*   **⚡ 极致轻量**: 核心库极小，适合 Flash/RAM 资源受限的单片机。
*   **🎥 丝滑动画**: 内置全局**虚拟相机 (Camera)** 系统，支持平滑滚动、惯性跟随和缓动动画。
*   **📱 现代化控件**: 提供类似智能手机的**自适应列表 (List)**，支持选中框变形、宽度自适应等高级视觉效果。
*   **🛠️ 硬件解耦**: 通过 HAL (硬件抽象层) 设计，可轻松适配 U8g2, TFT_eSPI, LovyanGFX 等任何底层驱动。
*   **🚀 一键部署**: 针对 U8g2 用户提供 `Hydrogen::deploy(u8g2)` 宏，一行代码即可集成。
*   **🇨🇳 中文支持**: 原生支持 UTF-8 字符串和中文字体渲染（需底层驱动支持）。

## 📦 依赖项

*   **U8g2** (推荐): 默认提供完美适配。
*   或者任何能提供画点 (`drawPixel`) 接口的图形库。

## 🚀 快速开始

### 1. 安装
将 `HydrogenUI` 文件夹放入你的 `lib` 目录，或者通过 PlatformIO `lib_deps` 安装。

### 2. 基础示例 (Arduino + U8g2)

```cpp
#include <Arduino.h>
#include <U8g2lib.h>
#include "HydrogenUI.h"

// 1. 定义 U8g2 驱动 (根据你的屏幕型号选择)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

void setup() {
    // 2. 一键部署 HydrogenUI
    // 这会自动初始化 HAL、图形引擎和应用程序管理器
    Hydrogen::deploy(u8g2);

    // 设置字体 (推荐使用支持中文的字体)
    u8g2.setFont(u8g2_font_wqy12_t_chinese1);
    u8g2.enableUTF8Print();

    // 3. 添加控件
    auto* btn = new Hydrogen::Button(10, 10, 60, 20, "点击我");
    Hydrogen::App.add(btn);
}

void loop() {
    // 4. 驱动 UI 更新循环
    Hydrogen::App.update();
}
```

## 📚 核心概念

### Application (应用程序)
全局单例 `Hydrogen::App`，是整个框架的指挥官。
*   `App.begin(hal)`: 初始化框架。
*   `App.add(widget)`: 将控件添加到屏幕。
*   `App.update()`: 处理动画、渲染和屏幕刷新。

### Widget (控件)
所有 UI 元素的基类。
*   **List (列表)**: 旗舰级控件。支持平滑滚动、自动对齐、选择框变形动画。
*   **FPSCounter**: 帧率监视器，用于性能调试。
*   **Button / Label**: 基础交互组件。

### Camera (虚拟相机)
HydrogenUI 的核心黑科技。
*   所有绘图操作都基于“世界坐标”。
*   相机决定了屏幕显示世界坐标的哪一部分。
*   通过 `App.getCamera().setTarget(x, y)`，你可以让屏幕平滑地“飞”到任何位置，从而实现丝滑的滚动效果。

### HAL (硬件抽象层)
位于 `src/hal/hal.h`。如果你不想用 U8g2，只需继承 `Hydrogen::HAL` 并实现 `drawPixel` 等几个纯虚函数，即可将框架移植到任何屏幕。

## 📖 API 速查

### List 控件
```cpp
auto* list = new Hydrogen::List(0, 0, 128, 64);
list->addItem("选项一");
list->addItem("选项二");
list->next(); // 滚动到下一项
list->prev(); // 滚动到上一项
```

### Graphics 绘图
```cpp
auto* g = App.getGraphics();
g->drawRect(x, y, w, h);       // 绘制矩形
g->drawRoundRect(x, y, w, h, r); // 绘制圆角矩形
g->drawText(x, y, "你好");      // 绘制文本
```

## 📂 目录结构

*   `src/core/`: 核心引擎 (App, Graphics, Camera)
*   `src/hal/`: 硬件适配层
*   `src/ui/`: UI 控件库
*   `examples/`: 示例代码

## ⚠️ 注意事项

*   **Flash 占用**: 如果启用中文字库，请注意单片机的 Flash 容量。STM32F103C8T6 (64KB) 建议使用 `u8g2_font_wqy12_t_chinese1` 等子集字体。
*   **性能**: 为获得最佳的丝滑体验，建议将 I2C 频率设置为 400kHz 或更高 (`u8g2.setBusClock(800000)`)。

---
License: MIT
