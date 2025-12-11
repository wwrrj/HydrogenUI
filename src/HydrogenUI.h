#pragma once

/**
 * @file HydrogenUI.h
 * @brief HydrogenUI 框架主头文件
 *
 * 包含此文件即可访问 HydrogenUI 的所有核心功能。
 * 框架会自动检测环境（如 U8g2）并启用相应的适配器。
 */

// 核心模块
#include "hal/hal.h"
#include "core/graphics.h"
#include "core/app.h"
#include "ui/widget.h"
#include "ui/list.h"
#include "ui/fps_counter.h"

// 平台适配器
// 如果检测到 U8g2 库，则自动包含 U8g2 适配层
// U8G2LIB_HH 是 U8g2lib.h 中的包含保护宏
#if defined(U8G2_LIB_H) || defined(U8X8_LIB_H) || defined(U8G2LIB_HH)
#include "hal/hal_u8g2.h"
#endif

namespace Hydrogen {

/**
 * @brief U8g2 一键部署助手
 *
 * 这是一个辅助函数，用于快速将 HydrogenUI 绑定到现有的 U8g2 实例上。
 * 它会自动创建 U8g2HAL 适配器并初始化 Application 单例。
 *
 * @param u8g2_instance 你的 U8g2 对象实例 (例如 U8G2_SSD1306_...)
 *
 * @par 使用示例:
 * @code
 *   U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);
 *
 *   void setup() {
 *     // 一行代码完成所有初始化
 *     Hydrogen::deploy(u8g2);
 *   }
 * @endcode
 */
#if defined(U8G2_LIB_H) || defined(U8X8_LIB_H) || defined(U8G2LIB_HH)
inline void deploy(U8G2& u8g2_instance) {
    // 创建静态 HAL 实例，确保其生命周期贯穿整个程序运行期
    static U8g2HAL hal(&u8g2_instance);
    // 启动应用程序并绑定 HAL
    App.begin(&hal);
}
#endif

} // namespace Hydrogen
