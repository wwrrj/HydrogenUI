#pragma once
#include "hal.h"
#include <U8g2lib.h>
#ifdef ARDUINO
#include <Arduino.h>
#endif

namespace Hydrogen {

class U8g2HAL : public HAL {
private:
    U8G2* u8g2;

public:
    explicit U8g2HAL(U8G2* u8g2_instance) : u8g2(u8g2_instance) {}

    void init() override {
        u8g2->begin();
    }

    void clear() override {
        u8g2->clearBuffer();
    }

    void update() override {
        u8g2->sendBuffer();
    }

    void drawPixel(int x, int y, uint8_t color) override {
        u8g2->setDrawColor(color);
        u8g2->drawPixel(x, y);
    }

    int getWidth() const override {
        return u8g2->getDisplayWidth();
    }

    int getHeight() const override {
        return u8g2->getDisplayHeight();
    }

    void drawStr(int x, int y, const char* s) override {
        u8g2->setDrawColor(1);
        u8g2->drawUTF8(x, y, s);
    }

    int getStrWidth(const char* s) override {
        return u8g2->getUTF8Width(s);
    }

    unsigned long getMillis() override {
        #ifdef ARDUINO
        return millis();
        #else
        return 0; // Fallback or implement for other platforms
        #endif
    }
};

} // namespace Hydrogen
