#include "list.h"
#include <cmath>

namespace Hydrogen {

void List::addItem(const std::string& item) {
    items.push_back(item);
}

void List::next() {
    if (items.empty()) return;
    if (selectedIndex < (int)items.size() - 1) {
        selectedIndex++;
    } else {
        selectedIndex = 0; // 循环回顶部
    }
}

void List::prev() {
    if (items.empty()) return;
    if (selectedIndex > 0) {
        selectedIndex--;
    } else {
        selectedIndex = items.size() - 1; // 循环到底部
    }
}

void List::update() {
    // 1. 相机逻辑 (列表平滑滚动)
    int screenCenterY = bounds.h / 2;
    int itemCenterY = (selectedIndex * itemHeight) + (itemHeight / 2);
    int targetCamY = itemCenterY - screenCenterY;

    int totalHeight = items.size() * itemHeight;
    int maxCamY = totalHeight - bounds.h;

    if (totalHeight < bounds.h) {
        // 如果列表高度小于屏幕，则居中显示
        targetCamY = -(bounds.h - totalHeight) / 2;
    } else {
        // 限制滚动范围
        if (targetCamY < 0) targetCamY = 0;
        if (targetCamY > maxCamY) targetCamY = maxCamY;
    }

    App.getCamera().setTarget(0, targetCamY);

    // 2. 选中框位置动画 (Y轴)
    targetSelectY = selectedIndex * itemHeight;

    // 使用缓动函数平滑移动
    if (std::abs(targetSelectY - selectY) > 0.5f) {
        selectY += (targetSelectY - selectY) * easing;
    } else {
        selectY = targetSelectY;
    }

    // 3. 选中框宽度动画 (Width)
    // 根据文本宽度计算目标宽度
    if (selectedIndex >= 0 && selectedIndex < (int)items.size()) {
        int textW = App.getGraphics()->getHAL()->getStrWidth(items[selectedIndex].c_str());
        targetSelectWidth = textW + 12; // 文本宽度 + 左右 padding (各 6px)
    } else {
        targetSelectWidth = 0;
    }

    // 首次运行时初始化宽度
    if (selectWidth == 0) selectWidth = targetSelectWidth;

    // 使用缓动函数平滑缩放
    if (std::abs(targetSelectWidth - selectWidth) > 0.5f) {
        selectWidth += (targetSelectWidth - selectWidth) * easing;
    } else {
        selectWidth = targetSelectWidth;
    }
}

std::string List::getSelectedItem() const {
    if (selectedIndex >= 0 && selectedIndex < (int)items.size()) {
        return items[selectedIndex];
    }
    return "";
}

void List::draw(Graphics& g) {
    if (!visible) return;

    // 绘制选中框 (动画效果)
    // 圆角矩形，半径 2px
    // 使用 selectY 实现平滑移动
    int boxY = bounds.y + (int)round(selectY);
    int boxW = (int)round(selectWidth);

    // 绘制圆角选中框
    // 左对齐 bounds.x，宽度动态变化
    g.drawRoundRect(bounds.x + 2, boxY, boxW, itemHeight, 2);

    // 绘制文本项
    // 优化：仅绘制可见区域内的项
    int camY = App.getCamera().getY();
    int screenH = bounds.h;

    // 根据可视区域计算起始和结束索引
    int startIdx = (camY - bounds.y) / itemHeight;
    int endIdx = (camY - bounds.y + screenH) / itemHeight + 1;

    // 限制索引范围
    if (startIdx < 0) startIdx = 0;
    if (endIdx > (int)items.size()) endIdx = items.size();

    int y = bounds.y + (startIdx * itemHeight);
    for (int i = startIdx; i < endIdx; ++i) {
        // 绘制文本
        // y + 12 是基线位置 (16px 行高)
        g.drawText(bounds.x + 6, y + 12, items[i]);
        y += itemHeight;
    }

    // 绘制滚动条 (静态显示在屏幕右侧)
    // 计算总高度和可视高度比例
    int totalHeight = items.size() * itemHeight;

    if (totalHeight > screenH) {
        // 计算滚动条高度
        // 条高 = (屏幕高 / 总高) * 屏幕高
        // 最小高度限制为 4px
        int barH = (screenH * screenH) / totalHeight;
        if (barH < 4) barH = 4;

        // 计算滚动条位置
        int maxCamY = totalHeight - screenH;
        if (maxCamY > 0) {
            int barY = (camY * (screenH - barH)) / maxCamY;

            // 限制范围
            if (barY < 0) barY = 0;
            if (barY > screenH - barH) barY = screenH - barH;

            // 绘制滚动条 (需要绕过相机偏移，因为它是 HUD 元素)
            int oldCamX = g.getCamX();
            int oldCamY = g.getCamY();
            g.setCamera(0, 0); // 暂时重置相机

            // 在右边缘绘制 2px 宽的滚动条
            g.fillRect(bounds.x + bounds.w - 3, bounds.y + barY, 2, barH);

            g.setCamera(oldCamX, oldCamY); // 恢复相机
        }
    }
}

} // namespace Hydrogen
