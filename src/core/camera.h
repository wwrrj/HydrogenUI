#pragma once
#include <cmath>

namespace Hydrogen {

/**
 * @brief 虚拟相机类
 * 
 * 实现了带缓动效果的 2D 坐标跟随系统。
 * 通过改变相机位置，实现整个 UI 层的平移和滚动效果。
 */
class Camera {
private:
    float x, y;             ///< 当前位置 (浮点数用于平滑计算)
    float targetX, targetY; ///< 目标位置
    float easing;           ///< 缓动系数 (0.0 ~ 1.0)，值越大响应越快

public:
    /**
     * @brief 构造函数
     * 默认缓动系数为 0.4
     */
    Camera() : x(0), y(0), targetX(0), targetY(0), easing(0.4f) {}

    /**
     * @brief 设置目标位置
     * 相机会在后续的 update() 中平滑移动到此位置
     */
    void setTarget(float tx, float ty) {
        targetX = tx;
        targetY = ty;
    }

    /**
     * @brief 瞬间跳转到指定位置
     * 不产生动画效果
     */
    void jumpTo(float jx, float jy) {
        x = targetX = jx;
        y = targetY = jy;
    }

    /**
     * @brief 更新相机状态
     * 计算下一帧的位置 (线性插值/缓动)
     */
    void update() {
        if (std::abs(targetX - x) > 0.1f) {
            x += (targetX - x) * easing;
        } else {
            x = targetX;
        }

        if (std::abs(targetY - y) > 0.1f) {
            y += (targetY - y) * easing;
        } else {
            y = targetY;
        }
    }

    /**
     * @brief 获取当前 X 坐标 (整数)
     */
    int getX() const { return (int)round(x); }

    /**
     * @brief 获取当前 Y 坐标 (整数)
     */
    int getY() const { return (int)round(y); }
};

} // namespace Hydrogen
