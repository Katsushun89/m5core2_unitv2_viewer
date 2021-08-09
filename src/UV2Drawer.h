#pragma once
#include <LovyanGFX.hpp>
#include <queue>

struct FaceMark {
    int32_t x;
    int32_t y;
};

struct FaceFrame {
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
    FaceMark mark[5];
    double prob;
};

class UV2Drawer {
   private:
    std::queue<FaceFrame> event_queue;
    LGFX *lcd;
    LGFX_Sprite *canvas;  //(&lcd);
    // LGFX_Sprite *center_base;  //(&canvas);

    int32_t center_px;
    int32_t center_py;
    int32_t lcd_width;

    float zoom;

    static constexpr int PALETTE_BLACK = 1;
    static constexpr int PALETTE_ORANGE = 2;
    static constexpr int PALETTE_GREEN = 3;
    static constexpr int32_t UV2_WIDTH = 240;

   public:
    UV2Drawer();
    ~UV2Drawer() = default;
    void setup(void);
    void pushEvent(FaceFrame frame);
    bool popEvent(FaceFrame &frame);
    int getLcdWidth(void) { return lcd_width; };
    int getCenterPx(void) { return center_px; };
    int getCenterPy(void) { return center_py; };
    void drawFaceFrame();
};
