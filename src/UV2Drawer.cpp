#include "UV2Drawer.h"

UV2Drawer::UV2Drawer() {
    lcd = new LGFX();
    canvas = new LGFX_Sprite(lcd);

    while (!event_queue.empty()) {
        event_queue.pop();
    }
}

void UV2Drawer::pushEvent(FaceFrame frame) { event_queue.push(frame); }

bool UV2Drawer::popEvent(FaceFrame &frame) {
    if (!event_queue.empty()) {
        frame = event_queue.front();
        event_queue.pop();
        return true;
    }
    return false;
}

void UV2Drawer::clearEvent() {
    while (!event_queue.empty()) {
        event_queue.pop();
    }
}

void UV2Drawer::setup(void) {
    lcd->init();

    lcd_width = lcd->width();
    center_px = lcd->width() >> 1;
    center_py = lcd->height() >> 1;
    zoom = (float)(std::min(lcd->width(), lcd->height())) / UV2_WIDTH;

    lcd->setPivot(center_px, center_py);
    canvas->setColorDepth(lgfx::palette_4bit);
    canvas->createSprite(lcd->width(), lcd->height());

    auto transpalette = 0;
    canvas->fillScreen(transpalette);

    canvas->setPaletteColor(PALETTE_BLACK, lcd->color888(0, 0, 15));
    canvas->setPaletteColor(PALETTE_ORANGE, lcd->color888(255, 81, 0));
    canvas->setPaletteColor(PALETTE_GREEN, lcd->color888(0, 255, 0));
    canvas->setPaletteColor(PALETTE_WHITE, lcd->color888(255, 255, 255));

    canvas->setTextFont(4);
    canvas->setTextDatum(lgfx::middle_center);

    // canvas->drawRect(100, 100, 120, 120, PALETTE_ORANGE);

    lcd->startWrite();
    canvas->pushSprite(0, 0);
}

void UV2Drawer::clearLastFaceFrame() {
    FaceFrame face_frame;
    while (!last_face_frame.empty()) {
        face_frame = last_face_frame.back();
        last_face_frame.pop_back();
        canvas->fillRect(convLcdRate(face_frame.x), convLcdRate(face_frame.y),
                         convLcdRate(face_frame.w) + 1,
                         convLcdRate(face_frame.h) + 1, PALETTE_BLACK);
    }
}

void UV2Drawer::drawFaceFrame(uint32_t millis) {
    static uint32_t last_draw_time = millis;
    if (!event_queue.empty()) {
        FaceFrame face_frame;
        clearLastFaceFrame();
        last_draw_time = millis;

        while (!event_queue.empty()) {
            if (popEvent(face_frame)) {
                canvas->drawRect(convLcdRate(face_frame.x),
                                 convLcdRate(face_frame.y),
                                 convLcdRate(face_frame.w),
                                 convLcdRate(face_frame.h), PALETTE_GREEN);
                const int32_t MARK_HALF_LEN =
                    convLcdRate(face_frame.w) / MARK_DIV_RATE;
                for (auto m : face_frame.mark) {
                    canvas->drawLine(convLcdRate(m.x) - MARK_HALF_LEN,
                                     convLcdRate(m.y),
                                     convLcdRate(m.x) + MARK_HALF_LEN,
                                     convLcdRate(m.y), PALETTE_ORANGE);
                    canvas->drawLine(
                        convLcdRate(m.x), convLcdRate(m.y) - MARK_HALF_LEN,
                        convLcdRate(m.x), convLcdRate(m.y) + MARK_HALF_LEN,
                        PALETTE_ORANGE);
                    face_frame.mark.pop_back();
                }
                last_face_frame.push_back(face_frame);
            }
        }
        canvas->pushSprite(0, 0);
    }

    if (millis - last_draw_time >= 500) {
        clearLastFaceFrame();
        canvas->pushSprite(0, 0);
    }
}

void UV2Drawer::drawFuncName(std::string func_name, bool is_dediced) {
    canvas->fillScreen(PALETTE_BLACK);
    // canvas->setTextSize(0.75);

    int y = canvas->height() / 2;
    int x =
        canvas->width() / 2 - canvas->textWidth(String(func_name.c_str())) / 2;
    canvas->setCursor(x, y);
    if (is_dediced)
        canvas->setTextColor(PALETTE_ORANGE);
    else
        canvas->setTextColor(PALETTE_WHITE);

    canvas->printf("%s", func_name.c_str());

    canvas->pushSprite(0, 0);
}