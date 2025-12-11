#pragma once
#include "widget.h"
#include "../core/app.h"
#include <vector>
#include <string>

namespace Hydrogen {

/**
 * @brief 垂直滚动列表控件
 *
 * 功能特性：
 * - 自动布局
 * - 支持无限长列表（受限于内存）
 * - 平滑滚动动画（配合全局相机）
 * - 选中框位置与宽度自适应动画
 * - 自动渲染裁剪（仅绘制可见区域）
 * - 内置滚动条
 */
class List : public Widget {
private:
    std::vector<std::string> items; ///< 列表项数据
    int selectedIndex;              ///< 当前选中的索引
    int itemHeight;                 ///< 单行高度（像素）

    // 动画状态变量
    float selectY;           ///< 选中框当前 Y 坐标
    float targetSelectY;     ///< 选中框目标 Y 坐标

    float selectWidth;       ///< 选中框当前宽度
    float targetSelectWidth; ///< 选中框目标宽度

    float easing;            ///< 动画缓动系数 (0.0 - 1.0)

public:
    /**
     * @brief 构造函数
     * @param x X 坐标
     * @param y Y 坐标
     * @param w 宽度
     * @param h 高度
     */
    List(int x, int y, int w, int h)
        : Widget(x, y, w, h), selectedIndex(0), itemHeight(16),
          selectY(0), targetSelectY(0),
          selectWidth(0), targetSelectWidth(0),
          easing(0.3f) {
    }

    /**
     * @brief 添加列表项
     * @param item 显示文本 (支持 UTF-8)
     */
    void addItem(const std::string& item);

    /**
     * @brief 选中下一项
     * 循环滚动：到达底部后自动回到顶部
     */
    void next();

    /**
     * @brief 选中上一项
     * 循环滚动：到达顶部后自动跳到底部
     */
    void prev();

    /**
     * @brief 更新逻辑
     * 处理平滑滚动、相机跟随和选中框动画
     */
    void update() override;

    /**
     * @brief 绘制控件
     * @param g 图形上下文
     */
    void draw(Graphics& g) override;

    /**
     * @brief 获取当前选中项的索引
     */
    int getSelectedIndex() const { return selectedIndex; }

    /**
     * @brief 获取当前选中项的文本
     */
    std::string getSelectedItem() const;
};

} // namespace Hydrogen
