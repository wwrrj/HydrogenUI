#include "list.h"
#include <cmath>

namespace Hydrogen {

void List::addItem(const std::string& item) {
    items.push_back(new Label(0, 0, item));
}

void List::addItem(Widget* widget) {
    items.push_back(widget);
}

void List::next() {
    if (items.empty()) return;
    int originalIndex = selectedIndex;

    // 尝试寻找下一个可交互的项
    int nextIndex = selectedIndex;
    do {
        if (nextIndex < (int)items.size() - 1) {
            nextIndex++;
        } else {
            nextIndex = 0; // 循环回顶部
        }

        // 如果找到了可交互的项，或者循环回到了原点（避免死循环）
        if (items[nextIndex]->isInteractive() || nextIndex == originalIndex) {
            selectedIndex = nextIndex;
            return;
        }
    } while (nextIndex != originalIndex);
}

void List::prev() {
    if (items.empty()) return;
    int originalIndex = selectedIndex;

    // 尝试寻找上一个可交互的项
    int prevIndex = selectedIndex;
    do {
        if (prevIndex > 0) {
            prevIndex--;
        } else {
            prevIndex = items.size() - 1; // 循环到底部
        }

        // 如果找到了可交互的项，或者循环回到了原点
        if (items[prevIndex]->isInteractive() || prevIndex == originalIndex) {
            selectedIndex = prevIndex;
            return;
        }
    } while (prevIndex != originalIndex);
}

void List::update() {
    // 更新所有子控件
    for (auto w : items) {
        w->update();
    }

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
    // 根据内容宽度计算目标宽度
    if (selectedIndex >= 0 && selectedIndex < (int)items.size()) {
        Widget* w = items[selectedIndex];

        // 如果不可交互，理论上不应该被选中，但为了安全起见，这里做一个检查
        // 如果选中了不可交互项（例如刚初始化时），我们可以让选中框消失或者全宽显示

        int contentW = w->getBounds().w;
        // 如果是 Label (宽度为0)，则计算文本宽度
        if (contentW == 0) {
             std::string s = w->toString();
             if (!s.empty()) {
                 contentW = App.getGraphics()->getHAL()->getStrWidth(s.c_str());
                 // 如果有箭头，需要加上箭头的宽度
                 // 这里无法直接判断是否是 Label 并有箭头，但我们可以给额外的 padding
                 // 或者通过 dynamic_cast (如果开启了 RTTI)
                 // 简单起见，统一给比较大的 padding
             }
        }
        targetSelectWidth = contentW + 12; // 内容宽度 + 左右 padding (各 6px)
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
        return items[selectedIndex]->toString();
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
        Widget* w = items[i];

        // 简单的对齐逻辑：如果是 Label (宽度为0且有文本)，则按基线对齐 (y+12)
        // 否则按左上角对齐 (y)
        // 注意：如果是两行模式的 ProgressBar，需要特殊处理高度
        if (w->getBounds().w == 0 && !w->toString().empty()) {
             // 文本垂直居中 (y + itemHeight/2 + 4)
             w->setPosition(bounds.x + 6, y + itemHeight/2 + 4);
        } else {
            // 固定宽度的 Label (如二级菜单) 或其他控件
            // 对于 Label，如果宽度固定，我们希望它是垂直居中对齐的基线
            // 通过 RTTI 或虚函数判断是否是 Label 比较麻烦
            // 简单判断：如果它有 text (toString不空)，且不是 Switch/ProgressBar (通常 Switch/ProgressBar 自己处理对齐)
            //
            // 实际上 Switch 和 ProgressBar 的 draw 里都是自己处理垂直居中的 (bounds.y + bounds.h/2)
            // 所以我们只需要正确设置 bounds.y 即可
            //
            // 如果是固定宽度的 Label，它的 draw 也是 drawText，需要基线
            // 但我们的 Label::draw 使用的是 drawText(x, y)，这里的 y 是基线
            // 如果我们给 Label 设置了 y = bounds.y (top)，它会画在顶部，可能被切掉
            //
            // 这是一个不一致性：Label::draw 期望 y 是基线，而 Switch::draw 期望 y 是 top
            //
            // 修复方案：
            // 如果是 Label，我们需要给它基线坐标。
            // 怎么判断是 Label？
            // 我们在 Label::draw 中自己处理垂直居中？
            //
            // 让我们修改 Label::draw，使其像 Switch 一样接受 top 坐标，并自己计算基线
            // 这需要修改 Label::draw

            // 暂时：我们在 List 中 hack 一下
            // 如果 toString 不为空，且不是 Label (宽度!=0)
            // 实际上我们无法区分 "固定宽度的 Label" 和 "Switch" (Switch 也有 toString)
            //
            // 让我们看看 Switch::draw: drawText(y + h/2 + 4) -> 自己加了偏移
            // Label::draw: drawText(y) -> 直接用 y
            //
            // 这确实是个问题。为了统一，Label::draw 也应该自己处理垂直对齐。
            // 让我们修改 Label::draw。

            w->setPosition(bounds.x + 6, y);
        }
        w->draw(g);
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
