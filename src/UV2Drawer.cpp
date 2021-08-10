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

void UV2Drawer::setup(void) {
    lcd->init();

    lcd_width = lcd->width();
    center_px = lcd->width() >> 1;
    center_py = lcd->height() >> 1;
    zoom = (float)(std::min(lcd->width(), lcd->height())) / UV2_WIDTH;

    lcd->setPivot(center_px, center_py);
    canvas->setColorDepth(lgfx::palette_4bit);

    // center_button_width = lcd->width() * 7 / 10;

    canvas->createSprite(lcd->width(), lcd->height());
    // center_base->createSprite(center_button_width, center_button_width);

    auto transpalette = 0;
    canvas->fillScreen(transpalette);
    // center_base->fillScreen(transpalette);

    canvas->setPaletteColor(PALETTE_BLACK, lcd->color888(0, 0, 15));
    canvas->setPaletteColor(PALETTE_ORANGE, lcd->color888(255, 81, 0));
    canvas->setPaletteColor(PALETTE_GREEN, lcd->color888(0, 255, 0));
    canvas->setPaletteColor(PALETTE_WHITE, lcd->color888(255, 255, 255));

    lcd->startWrite();
    canvas->fillRect(0, 0, lcd->width() / 2, lcd->height() / 2, PALETTE_GREEN);
    // center_button->pushRotateZoom(0, zoom, zoom, transpalette);
    // canvas->pushRotateZoom(0, zoom, zoom, transpalette);
    canvas->pushSprite(0, 0);
}

void UV2Drawer::clearLastFaceFrame() {
    FaceFrame face_frame;
    while (!last_face_frame.empty()) {
        face_frame = last_face_frame.back();
        last_face_frame.pop_back();
        canvas->drawRect(convLcdRate(face_frame.x), convLcdRate(face_frame.y),
                         convLcdRate(face_frame.w), convLcdRate(face_frame.h),
                         PALETTE_BLACK);
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