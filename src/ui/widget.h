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

    /**
     * @brief 设置控件位置
     * @param x 新的 X 坐标
     * @param y 新的 Y 坐标
     */
    void setPosition(int x, int y) {
        bounds.x = x;
        bounds.y = y;
    }

    /**
     * @brief 设置控件尺寸
     * @param w 宽度
     * @param h 高度
     */
    void setSize(int w, int h) {
        bounds.w = w;
        bounds.h = h;
    }

    /**
     * @brief 检查控件是否可交互
     * 用于列表选择逻辑：只有可交互的控件才能被选中
     */
    virtual bool isInteractive() const { return false; }

    /**
     * @brief 触发点击事件
     */
    virtual void click() {}

    /**
     * @brief 获取控件内容的字符串表示
     * 用于列表宽度自适应计算
     */
    virtual std::string toString() const { return ""; }
};

/**
 * @brief 文本标签控件
 * 用于显示单行文本。
 */
class Label : public Widget {
    std::string text;
    bool hasArrow; // 是否显示二级菜单箭头
    
public:
    // w 默认为 0 (自适应宽度)。如果设置了 w (如 100)，则箭头会画在最右边
    Label(int x, int y, const std::string& text, bool hasArrow = false, int w = 0) 
        : Widget(x, y, w, 0), text(text), hasArrow(hasArrow) {}
        
    void draw(Graphics& g) override;
    std::string toString() const override { return text; }
    
    // 如果有箭头，通常意味着这是一个可点击进入的菜单项
    bool isInteractive() const override { return hasArrow; }
};

/**
 * @brief 开关控件
 * 包含描述文本和右侧的切换开关
 */
class Switch : public Widget {
private:
    std::string label;
    bool isOn;
    
    // 动画状态
    float knobX;        // 当前滑块位置 (0.0 ~ 1.0)
    float targetKnobX;  // 目标位置 (0.0 或 1.0)
    
public:
    Switch(int x, int y, int w, int h, const std::string& label, bool initial = false) 
        : Widget(x, y, w, h), label(label), isOn(initial), knobX(initial ? 1.0f : 0.0f), targetKnobX(initial ? 1.0f : 0.0f) {}
        
    void update() override;
    void draw(Graphics& g) override;
    std::string toString() const override { return label; }
    
    bool isInteractive() const override { return true; }
    void click() override { toggle(); }

    // 切换状态
    void toggle();
    void setState(bool s);
    bool getState() const { return isOn; }
};

/**
 * @brief 进度条控件
 * 用于显示数值进度
 */
class ProgressBar : public Widget {
private:
    std::string label;
    float value;        // 当前显示的平滑值 (0.0 ~ 1.0)
    float targetValue;  // 目标值
    bool twoLineMode;   // 是否分两行显示
    float smoothing;    // 平滑系数 (0.0 ~ 1.0, 越小越平滑)
    
public:
    /**
     * @param twoLineMode 如果为 true，文字在第一行，进度条在第二行
     */
    ProgressBar(int x, int y, int w, int h, const std::string& label, float initial = 0.0f, bool twoLineMode = false) 
        : Widget(x, y, w, h), label(label), value(initial), targetValue(initial), twoLineMode(twoLineMode), smoothing(0.2f) {}
        
    void update() override;
    void draw(Graphics& g) override;
    std::string toString() const override { return label; }
    
    // 进度条通常是只读展示，不可交互
    bool isInteractive() const override { return false; }
    
    void setValue(float v) { 
        if (v < 0.0f) v = 0.0f;
        if (v > 1.0f) v = 1.0f;
        targetValue = v; // 仅设置目标值，实际值在 update 中平滑过渡
    }
    float getValue() const { return targetValue; }
    
    /**
     * @brief 设置平滑系数
     * @param s 0.1(慢) ~ 1.0(快)
     */
    void setSmoothing(float s) { smoothing = s; }
};

/**
 * @brief 直线控件
 */
class Line : public Widget {
    int x2, y2;
public:
    Line(int x1, int y1, int x2, int y2) : Widget(x1, y1, 0, 0), x2(x2), y2(y2) {}
    void draw(Graphics& g) override;
};

/**
 * @brief 矩形控件
 */
class RectWidget : public Widget {
    bool filled;
public:
    RectWidget(int x, int y, int w, int h, bool filled = false) : Widget(x, y, w, h), filled(filled) {}
    void draw(Graphics& g) override;
};

/**
 * @brief 圆形控件
 */
class CircleWidget : public Widget {
    int radius;
    bool filled;
public:
    CircleWidget(int x, int y, int r, bool filled = false) : Widget(x, y, r*2, r*2), radius(r), filled(filled) {}
    void draw(Graphics& g) override;
};

/**
 * @brief 像素点控件
 */
class Pixel : public Widget {
public:
    Pixel(int x, int y) : Widget(x, y, 1, 1) {}
    void draw(Graphics& g) override;
};

/**
 * @brief 日志终端控件
 * 用于显示滚动的日志文本
 */
class Logger : public Widget {
private:
    std::vector<std::string> lines;
    int maxLines;
    int lineHeight;
    bool autoScroll;

public:
    Logger(int x, int y, int w, int h, int maxLines = 10) 
        : Widget(x, y, w, h), maxLines(maxLines), lineHeight(12), autoScroll(true) {}

    void log(const std::string& msg);
    void draw(Graphics& g) override;
    
    // 清空日志
    void clear() { lines.clear(); }
};

/**
 * @brief 黑客帝国数字雨效果
 */
class MatrixRain : public Widget {
private:
    struct Column {
        float y;       // 当前下落高度
        float speed;   // 下落速度
        char content;  // 当前显示的字符（头）
        int length;    // 雨滴长度
    };
    static const int MAX_COLS = 22; // 128 / 6 ≈ 21
    Column cols[MAX_COLS];

public:
    MatrixRain(int x, int y, int w, int h);
    void update() override;
    void draw(Graphics& g) override;
};

} // namespace Hydrogen
