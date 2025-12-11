#include "graphics.h"
#include <cmath>

namespace Hydrogen {

void Graphics::drawLine(int x0, int y0, int x1, int y1) {
    // 转换到屏幕坐标
    x0 -= camX; y0 -= camY;
    x1 -= camX; y1 -= camY;

    // Bresenham 直线算法
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        hal->drawPixel(x0, y0, 1);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
    }
}

void Graphics::drawRect(int x, int y, int w, int h) {
    // 使用 drawLine 绘制四条边
    drawLine(x, y, x + w - 1, y);                 // 上
    drawLine(x, y + h - 1, x + w - 1, y + h - 1); // 下
    drawLine(x, y, x, y + h - 1);                 // 左
    drawLine(x + w - 1, y, x + w - 1, y + h - 1); // 右
}

void Graphics::fillRect(int x, int y, int w, int h) {
    // 转换到屏幕坐标
    int sx = x - camX;
    int sy = y - camY;

    // 简单的双重循环填充（效率较低，后续可优化为扫描线算法）
    for (int i = sx; i < sx + w; ++i) {
        for (int j = sy; j < sy + h; ++j) {
            hal->drawPixel(i, j, 1);
        }
    }
}

void Graphics::drawCircle(int x0, int y0, int r) {
    // 转换到屏幕坐标
    x0 -= camX; y0 -= camY;

    // Bresenham 圆算法
    int f = 1 - r;
    int ddF_x = 1;
    int ddF_y = -2 * r;
    int x = 0;
    int y = r;

    // 绘制四个基准点
    hal->drawPixel(x0, y0 + r, 1);
    hal->drawPixel(x0, y0 - r, 1);
    hal->drawPixel(x0 + r, y0, 1);
    hal->drawPixel(x0 - r, y0, 1);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        // 绘制八个象限的点
        hal->drawPixel(x0 + x, y0 + y, 1);
        hal->drawPixel(x0 - x, y0 + y, 1);
        hal->drawPixel(x0 + x, y0 - y, 1);
        hal->drawPixel(x0 - x, y0 - y, 1);
        hal->drawPixel(x0 + y, y0 + x, 1);
        hal->drawPixel(x0 - y, y0 + x, 1);
        hal->drawPixel(x0 + y, y0 - x, 1);
        hal->drawPixel(x0 - y, y0 - x, 1);
    }
}

void Graphics::fillCircle(int x0, int y0, int r) {
    // 转换到屏幕坐标
    x0 -= camX; y0 -= camY;

    // 使用 Bresenham 算法生成圆周点，并用水平线填充
    int f = 1 - r;
    int ddF_x = 1;
    int ddF_y = -2 * r;
    int x = 0;
    int y = r;

    // 中心线
    hal->drawPixel(x0, y0 + r, 1);
    hal->drawPixel(x0, y0 - r, 1);
    // 水平线填充 (中心)
    for (int i = x0 - r; i <= x0 + r; ++i) hal->drawPixel(i, y0, 1);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        // 填充水平线
        for (int i = x0 - x; i <= x0 + x; ++i) {
            hal->drawPixel(i, y0 + y, 1);
            hal->drawPixel(i, y0 - y, 1);
        }
        for (int i = x0 - y; i <= x0 + y; ++i) {
            hal->drawPixel(i, y0 + x, 1);
            hal->drawPixel(i, y0 - x, 1);
        }
    }
}

void Graphics::drawRoundRect(int x, int y, int w, int h, int r) {
    // 绘制四条直线边（留出圆角空间）
    drawLine(x + r, y, x + w - r - 1, y);                 // 上
    drawLine(x + r, y + h - 1, x + w - r - 1, y + h - 1); // 下
    drawLine(x, y + r, x, y + h - r - 1);                 // 左
    drawLine(x + w - 1, y + r, x + w - 1, y + h - r - 1); // 右

    // 圆角绘制（基于 Bresenham 算法）
    int x0 = x + r;
    int y0 = y + r; // 左上圆心
    int x1 = x + w - r - 1;
    int y1 = y + r; // 右上圆心
    int x2 = x + r;
    int y2 = y + h - r - 1; // 左下圆心
    int x3 = x + w - r - 1;
    int y3 = y + h - r - 1; // 右下圆心

    // 辅助 lambda：绘制相对于圆心的点（自动处理相机偏移）
    auto drawCorner = [&](int cx, int cy, int px, int py, int q) {
        cx -= camX; cy -= camY;
        if (q == 0) hal->drawPixel(cx - px, cy - py, 1); // 左上象限
        if (q == 1) hal->drawPixel(cx + px, cy - py, 1); // 右上象限
        if (q == 2) hal->drawPixel(cx - px, cy + py, 1); // 左下象限
        if (q == 3) hal->drawPixel(cx + px, cy + py, 1); // 右下象限
    };

    int f = 1 - r;
    int ddF_x = 1;
    int ddF_y = -2 * r;
    int xx = 0;
    int yy = r;

    // 绘制圆角的四个基准点
    drawCorner(x0, y0, r, 0, 0); drawCorner(x0, y0, 0, r, 0);
    drawCorner(x1, y1, r, 0, 1); drawCorner(x1, y1, 0, r, 1);
    drawCorner(x2, y2, r, 0, 2); drawCorner(x2, y2, 0, r, 2);
    drawCorner(x3, y3, r, 0, 3); drawCorner(x3, y3, 0, r, 3);

    while (xx < yy) {
        if (f >= 0) {
            yy--;
            ddF_y += 2;
            f += ddF_y;
        }
        xx++;
        ddF_x += 2;
        f += ddF_x;

        // 绘制圆弧
        drawCorner(x0, y0, xx, yy, 0); drawCorner(x0, y0, yy, xx, 0);
        drawCorner(x1, y1, xx, yy, 1); drawCorner(x1, y1, yy, xx, 1);
        drawCorner(x2, y2, xx, yy, 2); drawCorner(x2, y2, yy, xx, 2);
        drawCorner(x3, y3, xx, yy, 3); drawCorner(x3, y3, yy, xx, 3);
    }
}

void Graphics::drawText(int x, int y, const std::string& text) {
    // 转换到屏幕坐标并调用 HAL 绘制
    hal->drawStr(x - camX, y - camY, text.c_str());
}

} // namespace Hydrogen
