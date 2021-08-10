#pragma once
#include <LovyanGFX.hpp>
#include <queue>
#include <vector>

struct FaceMark {
    int32_t x;
    int32_t y;
};

struct FaceFrame {
    float x;
    float y;
    float w;
    float h;
    // FaceMark mark[5];
    float prob;
    std::vector<FaceMark> mark;
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
    static constexpr int PALETTE_WHITE = 4;
    static constexpr int32_t UV2_WIDTH = 480;

    std::vector<FaceFrame> last_face_frame;

    void clearLastFaceFrame();
    int32_t convLcdRate(float u) { return (int32_t)(u * zoom); }

   public:
    UV2Drawer();
    ~UV2Drawer() = default;
    void setup(void);
    void pushEvent(FaceFrame frame);
    bool popEvent(FaceFrame &frame);
    int getLcdWidth(void) { return lcd_width; };
    int getCenterPx(void) { return center_px; };
    int getCenterPy(void) { return center_py; };
    void drawFaceFrame(uint32_t millis);
};
