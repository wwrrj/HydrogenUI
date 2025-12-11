#pragma once
#include "../hal/hal.h"
#include "graphics.h"
#include "camera.h"
#include "../ui/widget.h"
#include <vector>

namespace Hydrogen {

/**
 * @brief 应用程序核心管理类
 *
 * 这是一个单例类（通过全局实例 App 访问），负责：
 * 1. 管理硬件抽象层 (HAL)
 * 2. 维护全局图形上下文 (Graphics)
 * 3. 管理 UI 控件树
 * 4. 驱动主循环和全局相机系统
 */
class Application {
private:
    HAL* _hal;
    Graphics* _graphics;
    Camera _camera;
    std::vector<Widget*> _widgets;

public:
    Application();
    ~Application();

    /**
     * @brief 启动应用程序
     * @param hal 指向已初始化的 HAL 实例
     */
    void begin(HAL* hal);

    /**
     * @brief 添加根级控件
     * @param widget 控件指针 (框架接管其生命周期)
     */
    void add(Widget* widget);

    /**
     * @brief 主循环更新
     * 需要在主程序的 loop() 中调用。
     * 负责：更新相机 -> 清屏 -> 更新控件逻辑 -> 绘制控件 -> 刷新屏幕
     */
    void update();

    /**
     * @brief 获取全局图形上下文
     */
    Graphics* getGraphics() { return _graphics; }

    /**
     * @brief 获取全局相机对象
     * 可通过此对象控制屏幕滚动
     */
    Camera& getCamera() { return _camera; }
};

// 全局唯一的应用程序实例
extern Application App;

} // namespace Hydrogen
