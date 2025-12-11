#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string>

namespace Hydrogen {

/**
 * @brief 硬件抽象层 (HAL) 接口
 *
 * HydrogenUI 通过此接口与底层硬件（如屏幕驱动、Arduino API）进行交互。
 * 任何新的平台只需实现此接口即可运行 HydrogenUI。
 */
class HAL {
public:
    virtual ~HAL() = default;

    /**
     * @brief 初始化硬件
     * 通常在这里调用底层驱动的 begin() 方法
     */
    virtual void init() = 0;

    /**
     * @brief 清除屏幕缓冲区
     */
    virtual void clear() = 0;

    /**
     * @brief 将缓冲区内容刷新到屏幕
     */
    virtual void update() = 0;

    /**
     * @brief 绘制一个像素点
     * @param x X 坐标
     * @param y Y 坐标
     * @param color 颜色值 (1=亮, 0=灭)
     */
    virtual void drawPixel(int x, int y, uint8_t color) = 0;

    /**
     * @brief 获取屏幕宽度
     * @return 宽度像素值
     */
    virtual int getWidth() const = 0;

    /**
     * @brief 获取屏幕高度
     * @return 高度像素值
     */
    virtual int getHeight() const = 0;

    /**
     * @brief 绘制字符串
     * @param x 起始 X 坐标
     * @param y 起始 Y 坐标 (通常是基线位置)
     * @param s 字符串内容 (支持 UTF-8)
     */
    virtual void drawStr(int x, int y, const char* s) = 0;

    /**
     * @brief 获取字符串的显示宽度
     * 用于自适应布局计算
     * @param s 字符串内容
     * @return 宽度像素值
     */
    virtual int getStrWidth(const char* s) = 0;

    /**
     * @brief 获取系统运行时间
     * 用于动画和帧率计算
     * @return 毫秒数
     */
    virtual unsigned long getMillis() = 0;
};

} // namespace Hydrogen
