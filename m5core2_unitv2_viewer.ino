#include <ArduinoJson.h>
#include <M5Core2.h>
#include <WiFi.h>

#include <LovyanGFX.hpp>

#include "src/UV2Drawer.h"
#include "src/UV2FuncSwitcher.h"

UV2Drawer uv2drawer;
UV2FuncSwitcher uv2func_switcher;
StaticJsonDocument<1536> doc;
std::string recv_uart_str;
HardwareSerial SerialPortA(1);

void setup(void) {
    Serial.begin(115200);
    SerialPortA.begin(115200, SERIAL_8N1, 33, 32);  // connect to UnitV2

    uv2drawer.setup();

    // updateDrawingCenter();

    SerialPortA.flush();
}

// int parseReceivedJson(uint8_t *payload) {
bool deserializeReceivedJson(std::string &json_data) {
    // Serial.print("json:");
    // Serial.println(json_data.c_str());
    DeserializationError error = deserializeJson(doc, json_data);

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return false;
    }
    return true;
}

void parseReceivedJson() {
    int num = doc["num"];                  // 4
    const char *running = doc["running"];  // "Face Detector"
    Serial.printf("%s:", running);

    Serial.print("fase: ");
    for (JsonObject face_item : doc["face"].as<JsonArray>()) {
        FaceFrame face_frame;
        double face_item_x = face_item["x"];  // 87.22392273, 298.6412659,
                                              // 405.6531982, 180.1317749
        double face_item_y = face_item["y"];  // 99.36726379, 47.6712265,
                                              // 148.7048798, 63.35846329
        double face_item_w =
            face_item["w"];  // 96.59121704, 95.04016113, 81.55065918,
                             // 82.10971069
        double face_item_h =
            face_item["h"];  // 125.2233276,
                             // 121.9224625, 95.16421509, 94.93948364
        double face_item_prob = face_item["prob"];  // 0.997230828, 0.992135584,
                                                    // 0.990016222, 0.945436954

        face_frame.x = face_item_x;
        face_frame.y = face_item_y;
        face_frame.w = face_item_w;
        face_frame.h = face_item_h;
        face_frame.prob = face_item_prob;
        for (JsonObject face_item_mark_item :
             face_item["mark"].as<JsonArray>()) {
            int face_item_mark_item_x =
                face_item_mark_item["x"];  // 114, 154, 140, 124, 156
            int face_item_mark_item_y =
                face_item_mark_item["y"];  // 148, 140, 170, 194, 188
            FaceMark mark = {face_item_mark_item_x, face_item_mark_item_y};
            face_frame.mark.push_back(mark);
        }
        // Serial.printf("x:%f, y:%f, w:%f, h:%f ", face_item_x, face_item_y,
        //              face_item_w, face_item_h);
        uv2drawer.pushEvent(face_frame);
    }
}

bool recvUart(std::string &rs) {
    uint32_t json_bracket_count = 0;
    int32_t recv_size = SerialPortA.available();
    if (recv_size > 0) {
        char c;
        for (int i = 0; i < recv_size; i++) {
            c = (char)SerialPortA.read();

            if (c == '{') {
                rs += c;
                json_bracket_count++;
                break;
            }
        }
    } else {
        return false;
    }

    // search of end json bracket
    do {
        recv_size = SerialPortA.available();
        int read_cnt = 0;
        if (recv_size > 0) {
            for (int i = 0; i < recv_size; i++) {
                char recv_char = (char)SerialPortA.read();
                read_cnt++;
                rs += recv_char;
                if (recv_char == '{')
                    json_bracket_count++;
                else if (recv_char == '}')
                    json_bracket_count--;
            }
        }
    } while (json_bracket_count != 0);
    return true;
}

void judgeBottomButtons(TouchPoint_t pos, bool is_touch_pressed,
                        bool &is_in_selected) {
    static bool is_button_pressed = false;
    if (!is_touch_pressed) is_button_pressed = false;

    static uint32_t last_pressed_time = millis();
    if (millis() - last_pressed_time <= 200) return;  // dead time

    if (!is_button_pressed) {
        if (pos.y > 240) {
            if (pos.x < 120) {  // btnA
                last_pressed_time = millis();

                is_button_pressed = true;
                is_in_selected = true;
                std::string func_name = uv2func_switcher.backSelectedFunc();
                uv2drawer.drawFuncName(func_name);

                Serial.printf("push L:%s\n", func_name.c_str());
            } else if (pos.x > 240) {  // btnC
                last_pressed_time = millis();

                is_button_pressed = true;
                is_in_selected = true;
                std::string func_name = uv2func_switcher.nextSelectedFunc();
                uv2drawer.drawFuncName(func_name);

                Serial.printf("push R:%s\n", func_name.c_str());
            } else if (pos.x >= 180 && pos.x <= 210) {  // btnB
                last_pressed_time = millis();

                is_button_pressed = true;
                is_in_selected = false;
                std::string func_name = uv2func_switcher.getCurrentFuncName();
                uv2drawer.drawFuncName(func_name, true);
                uv2func_switcher.switchFunc(SerialPortA);

                Serial.printf("push Center\n");
            }
        }
    }
}

void loop(void) {
    static bool is_in_selected_func = false;
    // touch panel
    TouchPoint_t pos = M5.Touch.getPressPoint();
    bool is_touch_pressed = false;
    if (M5.Touch.ispressed()) is_touch_pressed = true;
    judgeBottomButtons(pos, is_touch_pressed, is_in_selected_func);

    // serial communication
    if (recvUart(recv_uart_str)) {
        Serial.printf("%s", recv_uart_str.c_str());
        if (deserializeReceivedJson(recv_uart_str)) {
            parseReceivedJson();
        }
        recv_uart_str.clear();
    }

    if (!is_in_selected_func) {
        uv2drawer.drawFaceFrame(millis());
    } else {
        uv2drawer.clearEvent();
    }
}