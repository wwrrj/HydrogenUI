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
    g.drawText(bounds.x, bounds.y, text);
}

void Button::draw(Graphics& g) {
    if (!visible) return;
    g.drawRect(bounds.x, bounds.y, bounds.w, bounds.h);
    if (pressed) {
        g.fillRect(bounds.x + 2, bounds.y + 2, bounds.w - 4, bounds.h - 4);
    }
    // Simple centering logic placeholder
    g.drawText(bounds.x + 5, bounds.y + 5, label);
}

} // namespace Hydrogen
