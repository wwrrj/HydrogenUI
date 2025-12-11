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

void ProgressBar::update() {
    // 一阶低通滤波 (Exponential Moving Average)
    // value = (target * alpha) + (current * (1 - alpha))
    // 这里的 smoothing 参数直接作为 alpha
    //
    // 如果差异很小，直接等于目标值，避免浮点数“无限逼近”导致的计算开销
    if (std::abs(targetValue - value) > 0.001f) {
        value += (targetValue - value) * smoothing;
    } else {
        value = targetValue;
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

void Line::draw(Graphics& g) {
    if (!visible) return;
    g.drawLine(bounds.x, bounds.y, x2, y2);
}

void RectWidget::draw(Graphics& g) {
    if (!visible) return;
    if (filled) {
        g.fillRect(bounds.x, bounds.y, bounds.w, bounds.h);
    } else {
        g.drawRect(bounds.x, bounds.y, bounds.w, bounds.h);
    }
}

void CircleWidget::draw(Graphics& g) {
    if (!visible) return;
    // bounds.x, y 是圆心还是左上角？
    // Widget 构造函数里 CircleWidget 用的是 (x,y, r*2, r*2) 作为 bounds
    // 这意味着 bounds.x, y 是左上角
    // 但 Graphics::drawCircle 需要圆心
    int cx = bounds.x + radius;
    int cy = bounds.y + radius;

    if (filled) {
        g.fillCircle(cx, cy, radius);
    } else {
        g.drawCircle(cx, cy, radius);
    }
}

void Pixel::draw(Graphics& g) {
    if (!visible) return;
    // Graphics 没有 drawPixel，用 1px 线代替，或者调用 HAL
    // 假设 drawLine(x,y,x,y) 可以画点
    g.drawLine(bounds.x, bounds.y, bounds.x, bounds.y);
}

void Logger::log(const std::string& msg) {
    lines.push_back(msg);
    // 保持最大行数
    while ((int)lines.size() > maxLines) {
        lines.erase(lines.begin());
    }
}

void Logger::draw(Graphics& g) {
    if (!visible) return;

    // 绘制终端背景 (可选)
    // g.fillRect(bounds.x, bounds.y, bounds.w, bounds.h); // 需要设置颜色反转逻辑，这里默认黑色背景

    int y = bounds.y;
    for (const auto& line : lines) {
        g.drawText(bounds.x + 2, y + lineHeight, line); // 基线对齐
        y += lineHeight;
    }

    // 绘制光标 (闪烁效果可加)
    if (lines.size() < (size_t)maxLines) {
        g.drawText(bounds.x + 2, y + lineHeight, "_");
    }
}

MatrixRain::MatrixRain(int x, int y, int w, int h) : Widget(x, y, w, h) {
    for (int i = 0; i < MAX_COLS; i++) {
        cols[i].y = -std::rand() % 64; // 随机初始高度
        cols[i].speed = (std::rand() % 20 + 10) / 10.0f; // 随机速度 1.0 ~ 3.0
        cols[i].length = std::rand() % 6 + 3; // 长度 3~8
        cols[i].content = (char)(std::rand() % 94 + 33); // ASCII 33-126
    }
}

void MatrixRain::update() {
    for (int i = 0; i < MAX_COLS; i++) {
        cols[i].y += cols[i].speed;

        // 随机改变字符
        if (std::rand() % 10 > 8) {
            cols[i].content = (char)(std::rand() % 94 + 33);
        }

        if (cols[i].y > bounds.h + 10) {
            cols[i].y = -(std::rand() % 20);
            cols[i].speed = (std::rand() % 30 + 10) / 10.0f;
        }
    }
}

void MatrixRain::draw(Graphics& g) {
    if (!visible) return;

    // 列宽 6px，行高 8px (对于小字体)
    for (int i = 0; i < MAX_COLS; i++) {
        int x = bounds.x + i * 6;
        int headY = (int)cols[i].y;

        // 绘制整条雨滴
        for (int j = 0; j < cols[i].length; j++) {
            int y = headY - j * 8;

            // 越界检查
            if (y < -8) continue; // 还在屏幕上方太远
            if (y > bounds.h) continue; // 已经掉出屏幕

            // 字符生成：头部是 content，尾部依次偏移
            char displayChar = (char)(((cols[i].content + j) % 94) + 33);
            std::string s(1, displayChar);

            // 绘制
            // 注意：头部(j=0)应该是最亮的。OLED 没有亮度，我们全画实心。
            // 也可以通过隔行绘制尾部来模拟变暗效果
            if (j > 0 && j % 2 != 0) {
                // 尾部奇数位置跳过绘制，模拟虚线/变暗
                // continue;
            }

            // 垂直坐标对齐：drawText 通常 y 是基线。
            // 假设字体高 8，基线大概在 Top + 7
            g.drawText(x, y + 7, s);
        }
    }
}

} // namespace Hydrogen
