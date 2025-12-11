#pragma once
#include "../hal/hal.h"
#include <string>

namespace Hydrogen {

/**
 * @brief 2D 坐标点结构
 */
struct Point {
    int x, y;
};

/**
 * @brief 矩形区域结构
 */
struct Rect {
    int x, y, w, h;
};

/**
 * @brief 核心图形引擎
 *
 * 提供了基础的 2D 绘图原语。
 * 所有绘图操作都基于 HAL 接口，因此与硬件无关。
 *
 * @note 坐标系统：
 * Graphics 内部会自动处理“世界坐标”到“屏幕坐标”的转换。
 * 绘图时传入的是世界坐标，Graphics 会自动减去 Camera 的偏移量。
 */
class Graphics {
protected:
    HAL* hal;
    int camX, camY;

public:
    /**
     * @brief 构造函数
     * @param hal 硬件抽象层实例
     */
    explicit Graphics(HAL* hal) : hal(hal), camX(0), camY(0) {}

    /**
     * @brief 设置相机位置
     * @param x 相机左上角 X 坐标
     * @param y 相机左上角 Y 坐标
     */
    void setCamera(int x, int y) {
        camX = x;
        camY = y;
    }

    int getCamX() const { return camX; }
    int getCamY() const { return camY; }

    /**
     * @brief 绘制直线 (Bresenham 算法)
     */
    void drawLine(int x0, int y0, int x1, int y1);

    /**
     * @brief 绘制空心矩形
     */
    void drawRect(int x, int y, int w, int h);

    /**
     * @brief 绘制实心矩形
     */
    void fillRect(int x, int y, int w, int h);

    /**
     * @brief 绘制空心圆
     * @param x0 圆心 X
     * @param y0 圆心 Y
     * @param r 半径
     */
    void drawCircle(int x0, int y0, int r);

    /**
     * @brief 绘制圆角矩形
     * @param r 圆角半径
     */
    void drawRoundRect(int x, int y, int w, int h, int r);

    /**
     * @brief 绘制文本
     * @param text 支持 UTF-8 字符串
     */
    void drawText(int x, int y, const std::string& text);

    /**
     * @brief 获取底层 HAL 实例
     * 用于需要直接访问底层接口的高级操作
     */
    HAL* getHAL() { return hal; }
};

} // namespace Hydrogen
