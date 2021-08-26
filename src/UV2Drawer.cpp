#include "UV2Drawer.h"

UV2Drawer::UV2Drawer() {
    lcd = new LGFX();
    canvas = new LGFX_Sprite(lcd);

    while (!event_queue.empty()) {
        event_queue.pop();
    }
}

void UV2Drawer::pushEvent(std::string event) { event_queue.push(event); }

bool UV2Drawer::popEvent(std::string &event) {
    if (!event_queue.empty()) {
        event = event_queue.front();
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

    lcd->startWrite();
    canvas->pushSprite(0, 0);
}

void UV2Drawer::clearFullScreen() { canvas->fillScreen(PALETTE_BLACK); }

void UV2Drawer::updateScreen() { canvas->pushSprite(0, 0); }

void UV2Drawer::drawFaceFrame(FaceFrame &face_frame) {
    canvas->drawRect(convLcdRate(face_frame.x), convLcdRate(face_frame.y),
                     convLcdRate(face_frame.w), convLcdRate(face_frame.h),
                     PALETTE_GREEN);
    const int32_t MARK_HALF_LEN = convLcdRate(face_frame.w) / MARK_DIV_RATE;
    for (auto m : face_frame.mark) {
        canvas->drawLine(convLcdRate(m.x) - MARK_HALF_LEN, convLcdRate(m.y),
                         convLcdRate(m.x) + MARK_HALF_LEN, convLcdRate(m.y),
                         PALETTE_ORANGE);
        canvas->drawLine(convLcdRate(m.x), convLcdRate(m.y) - MARK_HALF_LEN,
                         convLcdRate(m.x), convLcdRate(m.y) + MARK_HALF_LEN,
                         PALETTE_ORANGE);
        face_frame.mark.pop_back();
    }
    canvas->setTextSize(0.5);
    canvas->setCursor(convLcdRate(face_frame.x),
                      convLcdRate(face_frame.y) - canvas->fontHeight());
    // canvas->setTextColor(PALETTE_WHITE, PALETTE_GREEN);
    canvas->setTextColor(PALETTE_GREEN);
    canvas->printf("%.2f", face_frame.prob);
}

void UV2Drawer::drawCodeDetector(CodeDetector &code) {
    canvas->drawRect(convLcdRate(code.x), convLcdRate(code.y),
                     convLcdRate(code.w), convLcdRate(code.h), PALETTE_GREEN);
    canvas->setTextSize(0.5);
    canvas->setCursor(convLcdRate(code.x),
                      convLcdRate(code.y) - canvas->fontHeight());
    // canvas->setTextColor(PALETTE_WHITE, PALETTE_GREEN);
    canvas->setTextColor(PALETTE_GREEN);
    canvas->printf("%.2f %s", code.prob, code.content.c_str());
}

void UV2Drawer::drawTargetTracker(TargetTracker &tracker) {
    canvas->drawRect(convLcdRate(tracker.x), convLcdRate(tracker.y),
                     convLcdRate(tracker.w), convLcdRate(tracker.h),
                     PALETTE_GREEN);
}

void UV2Drawer::drawObjectRecognition(ObjectRecognition &recog) {
    canvas->drawRect(convLcdRate(recog.x), convLcdRate(recog.y),
                     convLcdRate(recog.w), convLcdRate(recog.h), PALETTE_GREEN);
    canvas->setTextSize(0.5);
    canvas->setCursor(convLcdRate(recog.x),
                      convLcdRate(recog.y) - canvas->fontHeight());
    // canvas->setTextColor(PALETTE_WHITE, PALETTE_GREEN);
    canvas->setTextColor(PALETTE_GREEN);
    canvas->printf("%.2f %s", recog.prob, recog.type.c_str());
}

void UV2Drawer::drawFuncName(std::string func_name, bool is_dediced) {
    canvas->fillScreen(PALETTE_BLACK);
    canvas->setTextSize(1);

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