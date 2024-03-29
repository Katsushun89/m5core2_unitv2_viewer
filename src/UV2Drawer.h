#pragma once
#include <LovyanGFX.hpp>
#include <queue>
#include <vector>

struct FaceMark {
    int32_t x;
    int32_t y;
};

struct FaceDetector {
    float x;
    float y;
    float w;
    float h;
    float prob;
    std::vector<FaceMark> mark;
};

struct CodeDetector {
    float x;
    float y;
    float w;
    float h;
    float prob;
    std::string type;
    std::string content;
};

struct TargetTracker {
    float x;
    float y;
    float w;
    float h;
};

struct ObjectRecognition {
    float x;
    float y;
    float w;
    float h;
    float prob;
    std::string type;
};

class UV2Drawer {
   private:
    std::queue<std::string> event_queue;
    LGFX *lcd;
    LGFX_Sprite *canvas;

    int32_t center_px;
    int32_t center_py;
    int32_t lcd_width;

    float zoom;

    static constexpr int PALETTE_BLACK = 1;
    static constexpr int PALETTE_ORANGE = 2;
    static constexpr int PALETTE_GREEN = 3;
    static constexpr int PALETTE_WHITE = 4;
    static constexpr int32_t UV2_WIDTH = 480;
    static constexpr int32_t MARK_DIV_RATE = 10;

    int32_t convLcdRate(float u) { return (int32_t)(u * zoom); }

   public:
    UV2Drawer();
    ~UV2Drawer() = default;
    void setup(void);
    void pushEvent(std::string event);
    bool popEvent(std::string &event);
    void clearEvent();
    int getLcdWidth(void) { return lcd_width; };
    int getCenterPx(void) { return center_px; };
    int getCenterPy(void) { return center_py; };
    void updateScreen();
    void drawFaceDetector(FaceDetector &face);
    void drawCodeDetector(CodeDetector &code);
    void drawTargetTracker(TargetTracker &tracker);
    void drawObjectRecognition(ObjectRecognition &recog);

    void drawFuncName(std::string func_name, bool is_dediced = false);
    void clearFullScreen();
};
