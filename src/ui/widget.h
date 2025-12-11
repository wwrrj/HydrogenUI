#pragma once
#include "../core/graphics.h"
#include <vector>
#include <string>

namespace Hydrogen {

/**
 * @brief UI 控件基类
 * 
 * 所有 UI 组件（如按钮、列表、标签）都必须继承此类。
 * 支持树状层级结构（父子关系），尽管目前主要使用扁平结构。
 */
class Widget {
protected:
    Rect bounds;                ///< 控件的几何边界 (x, y, w, h)
    Widget* parent;             ///< 父控件指针
    std::vector<Widget*> children; ///< 子控件列表
    bool visible;               ///< 可见性标志

public:
    /**
     * @brief 构造函数
     * @param x X 坐标
     * @param y Y 坐标
     * @param w 宽度
     * @param h 高度
     */
    Widget(int x, int y, int w, int h) : bounds({x, y, w, h}), parent(nullptr), visible(true) {}
    virtual ~Widget();

    /**
     * @brief 绘制方法（纯虚函数）
     * 子类必须实现此方法来定义控件的外观。
     * @param g 图形上下文
     */
    virtual void draw(Graphics& g) = 0;

    /**
     * @brief 逻辑更新方法
     * 每帧调用一次，用于处理动画、输入等非绘图逻辑。
     */
    virtual void update() {}

    /**
     * @brief 添加子控件
     * @param child 子控件指针
     */
    void addChild(Widget* child);

    /**
     * @brief 设置可见性
     */
    void setVisible(bool v) { visible = v; }
    
    /**
     * @brief 检查是否可见
     */
    bool isVisible() const { return visible; }
    
    /**
     * @brief 获取控件边界
     */
    Rect getBounds() const { return bounds; }
};

/**
 * @brief 文本标签控件
 * 用于显示单行文本。
 */
class Label : public Widget {
    std::string text;
public:
    Label(int x, int y, const std::string& text) : Widget(x, y, 0, 0), text(text) {}
    void draw(Graphics& g) override;
};

/**
 * @brief 按钮控件
 * 简单的矩形按钮，包含文本标签。
 */
class Button : public Widget {
    std::string label;
    bool pressed;
public:
    Button(int x, int y, int w, int h, const std::string& label) 
        : Widget(x, y, w, h), label(label), pressed(false) {}
    void draw(Graphics& g) override;
};

} // namespace Hydrogen
