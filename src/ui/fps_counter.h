#pragma once
#include "widget.h"
#include "../core/graphics.h"
#include <string>

namespace Hydrogen {

/**
 * @brief FPS 帧率计数器控件
 * 
 * 一个简单的调试控件，用于显示当前的屏幕刷新率。
 * 建议在开发阶段使用，以监控性能。
 */
class FPSCounter : public Widget {
private:
    unsigned long lastTime; ///< 上次统计的时间戳
    int frameCount;         ///< 当前时间窗口内的帧数累积
    int fps;                ///< 计算出的 FPS 值

public:
    /**
     * @brief 构造函数
     * @param x 显示位置 X
     * @param y 显示位置 Y
     */
    FPSCounter(int x, int y) : Widget(x, y, 0, 0), lastTime(0), frameCount(0), fps(0) {}

    /**
     * @brief 绘制方法
     * 自动统计并显示 FPS
     */
    void draw(Graphics& g) override;
};

} // namespace Hydrogen
