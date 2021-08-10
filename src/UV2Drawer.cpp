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

#if 0
void UV2Drawer::drawLRButton(void) {
    const int inside_x = 30;
    const int outside_x = 5;
    const int top_y = (lcd->height() >> 1) - 20;
    const int mdl_y = (lcd->height() >> 1) - 0;
    const int btm_y = (lcd->height() >> 1) + 20;
    canvas->fillTriangle(inside_x, top_y, inside_x, btm_y, outside_x, mdl_y,
                         PALETTE_ORANGE);  // left
    canvas->fillTriangle(
        lcd->width() - inside_x, top_y, lcd->width() - inside_x, btm_y,
        lcd->width() - outside_x, mdl_y, PALETTE_ORANGE);  // right
}

void UV2Drawer::drawCenterBase(void) {
    auto transpalette = 0;
    center_base->fillRect(0, 0, center_button_width, center_button_width,
                          PALETTE_BLACK);
    center_base->pushRotateZoom(0, zoom, zoom, transpalette);
}

void UV2Drawer::drawCenterOFF(String current_switch_str) {
    Serial.println("drawCenterOff\n");
    int center = center_button_width >> 1;
    center_button->fillCircle(center, center, center - 1, 1);
    center_button->drawCircle(center, center, center - 2 - RING_OUTSIDE_WIDTH,
                              PALETTE_ORANGE);
    center_button->drawCircle(center, center, center - 2 - RING_TOTAL_WIDTH,
                              PALETTE_ORANGE);
    for (int i = 0; i < RING_INSIDE_DIV; i++) {
        center_button->fillArc(center, center, center - 2 - RING_OUTSIDE_WIDTH,
                               center - 2 - RING_TOTAL_WIDTH,
                               360 / RING_INSIDE_DIV * i,
                               360 / RING_INSIDE_DIV * i, PALETTE_ORANGE);
    }
}

void UV2Drawer::drawCenter(FaceFrame &info, uint32_t decision_time,
                             String current_switch_str) {
    if (info.is_in_transition) {
        if (info.is_switched_on) {
            drawCenterTransitionOn2Off(info.keep_push_time, decision_time);
        } else {
            drawCenterTransitionOff2On(info.keep_push_time, decision_time);
        }
    } else {
        if (info.is_switched_on) {
            drawCenterON(current_switch_str);
        } else {
            drawCenterOFF(current_switch_str);
        }
    }
    drawCenterBase();
    auto transpalette = 0;
    center_button->pushRotateZoom(0, zoom, zoom, transpalette);
    canvas->pushSprite(0, 0);
}
#endif
void UV2Drawer::drawFaceFrame() {
    FaceFrame face_frame;
    while (!last_face_frame.empty()) {
        face_frame = last_face_frame.back();
        last_face_frame.pop_back();
        canvas->fillRect(convLcdRate(face_frame.x), convLcdRate(face_frame.y),
                         convLcdRate(face_frame.w), convLcdRate(face_frame.h),
                         PALETTE_BLACK);
    }

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