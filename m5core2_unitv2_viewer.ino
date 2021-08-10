#include <ArduinoJson.h>
#include <M5Core2.h>
#include <WiFi.h>

#include <LovyanGFX.hpp>

#include "src/UV2Drawer.h"

UV2Drawer uv2drawer;
StaticJsonDocument<1536> doc;
std::string recv_uart_str;
HardwareSerial SerialPortA(1);

void setup(void) {
    Serial.begin(115200);
    SerialPortA.begin(115200, SERIAL_8N1, 33, 32);  // connect to UnitV2

    uv2drawer.setup();

    // updateDrawingCenter();
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

void loop(void) {
    if (recvUart(recv_uart_str)) {
        Serial.printf("%s", recv_uart_str.c_str());
        if (deserializeReceivedJson(recv_uart_str)) {
            parseReceivedJson();
        }
        recv_uart_str.clear();
    }

    uv2drawer.drawFaceFrame();
}