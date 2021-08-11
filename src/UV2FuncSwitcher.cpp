#include "UV2FuncSwitcher.h"

UV2FuncSwitcher::UV2FuncSwitcher()
    : func_name_index(0), func_names{"Audio FFT",         "Code Detector",
                                     "Face Detector",     "Lane Line Tracker",
                                     "Motion Tracker",    "Shape Matching",
                                     "Camera Stream",     "Online Classifier",
                                     "Color Tracker",     "Face Recognition",
                                     "Target Tracker",    "Shape Detector",
                                     "Object Recognition"} {}

void UV2FuncSwitcher::setup(void) {}

void UV2FuncSwitcher::sendSwitchCommand(std::string func_name,
                                        HardwareSerial &serial) {
    if (func_name.empty()) return;
    doc["function"] = func_name.c_str();

    // check functions name is in list

    if (func_name == "Object Recognition") {
        doc["args"][0] = "yolo_20class";
    } else {
        doc["args"][0] = "";
    }
    std::string json;
    serializeJson(doc, json);
    serial.println(json.c_str());
}

std::string UV2FuncSwitcher::backSelectedFunc(void) {
    if (func_name_index == 0)
        func_name_index = func_names.size() - 1;
    else
        func_name_index--;
    return func_names[func_name_index];
}

std::string UV2FuncSwitcher::nextSelectedFunc(void) {
    if (func_name_index == func_names.size() - 1)
        func_name_index = 0;
    else
        func_name_index++;
    return func_names[func_name_index];
}

std::string UV2FuncSwitcher::getCurrentFuncName(void) {
    return func_names[func_name_index];
}

bool UV2FuncSwitcher::switchFunc(HardwareSerial &serial) {
    sendSwitchCommand(func_names[func_name_index], serial);
}