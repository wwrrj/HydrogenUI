#include "app.h"

namespace Hydrogen {

// 全局实例定义
Application App;

Application::Application() : _hal(nullptr), _graphics(nullptr) {}

Application::~Application() {
    if (_graphics) delete _graphics;
    // 不删除 _hal，因为它可能是在栈上分配的（参见 deploy 助手）
    // 但必须删除我们管理的所有控件
    for (auto w : _widgets) {
        delete w;
    }
}

void Application::begin(HAL* hal) {
    _hal = hal;
    if (_hal) {
        _hal->init(); // 初始化硬件
        _graphics = new Graphics(_hal); // 创建图形上下文
    }
}

void Application::add(Widget* widget) {
    _widgets.push_back(widget);
}

void Application::update() {
    if (!_hal || !_graphics) return;

    // 1. 更新相机位置（平滑滚动核心）
    _camera.update();

    // 2. 将相机位置应用到图形上下文
    // 这会影响后续所有的绘图操作（实现全局坐标系）
    _graphics->setCamera(_camera.getX(), _camera.getY());

    // 3. 清除屏幕
    _hal->clear();

    // 4. 更新并绘制所有根控件
    for (auto w : _widgets) {
        w->update(); // 更新控件逻辑（如动画状态）
        w->draw(*_graphics); // 绘制控件
    }

    // 5. 刷新屏幕缓冲区
    _hal->update();
}

} // namespace Hydrogen
