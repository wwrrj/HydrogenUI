#include "fps_counter.h"
#include <cstdio>

namespace Hydrogen {

void FPSCounter::draw(Graphics& g) {
    if (!visible) return;

    // 计算 FPS
    unsigned long now = g.getHAL()->getMillis();
    frameCount++;
    
    // 每秒更新一次
    if (now - lastTime >= 1000) {
        fps = frameCount;
        frameCount = 0;
        lastTime = now;
    }

    // 格式化 FPS 字符串
    char buf[16];
    snprintf(buf, sizeof(buf), "FPS: %d", fps);
    
    // 以 HUD (平视显示器) 模式绘制
    // 忽略相机的平移，确保 FPS 始终显示在屏幕固定位置
    int oldCamX = g.getCamX();
    int oldCamY = g.getCamY();
    g.setCamera(0, 0); // 暂时重置相机
    
    // 可选：绘制背景以提高对比度
    // g.fillRect(bounds.x, bounds.y, 50, 12);
    
    // 绘制文本 (y+10 是为了基线对齐)
    g.drawText(bounds.x, bounds.y + 10, std::string(buf));
    
    g.setCamera(oldCamX, oldCamY); // 恢复相机
}

} // namespace Hydrogen
