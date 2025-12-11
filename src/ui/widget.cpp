#include "widget.h"

namespace Hydrogen {

Widget::~Widget() {
    for (auto child : children) {
        delete child;
    }
}

void Widget::addChild(Widget* child) {
    child->parent = this;
    children.push_back(child);
}

void Label::draw(Graphics& g) {
    if (!visible) return;

    int drawY = bounds.y;

    // 如果设置了固定宽度 (bounds.w > 0)，我们假设 bounds.y 是 Top 坐标，需要计算基线
    // 假设高度为 16px (List默认)，基线在 Top + 12
    // 这里简单处理：如果 bounds.h > 0，则垂直居中；否则认为 y 就是基线
    //
    // 在 List::draw 中：
    // 自适应 Label (w=0): y 已经是基线 (y + itemHeight/2 + 4)
    // 固定宽 Label (w>0): y 是 Top (y)

    if (bounds.w > 0) {
        // 固定宽度模式：y 是 Top，需要计算基线
        // 假设字体高度 12px
        drawY = bounds.y + 12;
        // 或者更通用的垂直居中： bounds.y + bounds.h/2 + 4
        // 但 Label 默认 h=0
        // 让我们用简单的偏移
    }

    g.drawText(bounds.x, drawY, text);

    // 绘制二级菜单箭头 (->)
    if (hasArrow) {
        // 如果 bounds.w > 0，则箭头画在右边界附近
        // 否则画在文本右侧

        int arrowX;
        if (bounds.w > 0) {
            // 固定宽度模式：画在最右侧
            arrowX = bounds.x + bounds.w - 10;
        } else {
            // 自适应模式：画在文本右侧
            int textW = g.getHAL()->getStrWidth(text.c_str());
            arrowX = bounds.x + textW + 10;
        }

        int arrowY = drawY - 4; // 调整基线

        // 绘制 ">" 形状
        g.drawLine(arrowX, arrowY, arrowX + 4, arrowY + 4);
        g.drawLine(arrowX, arrowY + 8, arrowX + 4, arrowY + 4);
    }
}

void Switch::toggle() {
    isOn = !isOn;
    targetKnobX = isOn ? 1.0f : 0.0f;
}

void Switch::setState(bool s) {
    isOn = s;
    targetKnobX = isOn ? 1.0f : 0.0f;
}

void Switch::update() {
    if (std::abs(targetKnobX - knobX) > 0.05f) {
        knobX += (targetKnobX - knobX) * 0.3f;
    } else {
        knobX = targetKnobX;
    }
}

void Switch::draw(Graphics& g) {
    if (!visible) return;

    // 1. 绘制左侧描述文本 (垂直居中)
    g.drawText(bounds.x + 2, bounds.y + bounds.h/2 + 4, label);

    // 2. 绘制右侧开关图标
    // 为了让圆形的滑块(直径总是奇数)能完美垂直居中，外框高度最好也是奇数
    // 同时为了接近胶囊形状，圆角半径取高度的一半
    int swH = 13;
    int swW = 25;

    int swX = bounds.x + bounds.w - swW - 4; // 右对齐，留出padding
    int swY = bounds.y + (bounds.h - swH) / 2;

    // 绘制外框 (胶囊形状)
    // h=13, r=6. 2r=12. 中间有 1px 的直线，视觉上基本是胶囊
    g.drawRoundRect(swX, swY, swW, swH, swH/2);

    // 绘制滑块
    // 留出 2px 边距: 13 - 2*2 = 9px 直径
    // 半径 r=4 -> 直径 2r+1 = 9px
    int margin = 2;
    int r = 4;
    int knobDiameter = 9;

    int minKnobX = swX + margin;
    int maxKnobX = swX + swW - knobDiameter - margin;

    int knobXPos = minKnobX + (int)((maxKnobX - minKnobX) * knobX);
    int knobY = swY + margin; // y偏移

    // 计算圆心
    // knobY 是滑块的 Top 坐标
    // 圆心 y = knobY + r = swY + 2 + 4 = swY + 6
    // 检查居中：
    // Top Gap = (swY+2) - swY = 2
    // Bottom Gap = (swY+13) - (swY+2+9) = 2
    // 完美居中
    int centerX = knobXPos + r;
    int centerY = knobY + r;

    if (isOn || knobX > 0.5f) {
        // 实心圆滑块表示开启
        g.fillCircle(centerX, centerY, r);
    } else {
        // 空心圆滑块表示关闭
        g.drawCircle(centerX, centerY, r);
    }
}

void ProgressBar::draw(Graphics& g) {
    if (!visible) return;

    if (twoLineMode) {
        // 两行模式
        // 第一行：文本
        g.drawText(bounds.x + 2, bounds.y + bounds.h/4 + 4, label);

        // 第二行：进度条 (占满宽度)
        int barW = bounds.w - 4;
        int barH = 6;
        int barX = bounds.x + 2;
        int barY = bounds.y + bounds.h * 3/4 - barH/2;

        g.drawRect(barX, barY, barW, barH);

        if (value > 0.0f) {
            int fillW = (int)((barW - 4) * value);
            if (fillW > 0) {
                g.fillRect(barX + 2, barY + 2, fillW, barH - 4);
            }
        }
    } else {
        // 单行模式 (原有逻辑)
        // 1. 绘制左侧描述文本
        g.drawText(bounds.x + 2, bounds.y + bounds.h/2 + 4, label);

        // 2. 智能计算进度条宽度
        int textW = g.getHAL()->getStrWidth(label.c_str());
        int maxBarW = bounds.w - textW - 12;
        if (maxBarW < 20) maxBarW = 20;

        int barW = maxBarW;
        int barH = 8;
        int barX = bounds.x + bounds.w - barW - 4;
        int barY = bounds.y + (bounds.h - barH) / 2;

        g.drawRect(barX, barY, barW, barH);

        if (value > 0.0f) {
            int fillW = (int)((barW - 4) * value);
            if (fillW > 0) {
                g.fillRect(barX + 2, barY + 2, fillW, barH - 4);
            }
        }
    }
}

} // namespace Hydrogen
