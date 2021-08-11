#pragma once
#include <ArduinoJson.h>
#include <M5Core2.h>

#include <string>
#include <vector>

class UV2FuncSwitcher {
   private:
    uint32_t func_name_index;
    std::vector<std::string> func_names;
    StaticJsonDocument<96> doc;
    void sendSwitchCommand(std::string func_name, HardwareSerial &serial);

   public:
    UV2FuncSwitcher();
    ~UV2FuncSwitcher() = default;
    void setup(void);
    std::string backSelectedFunc(void);
    std::string nextSelectedFunc(void);
    std::string getCurrentFuncName(void);
    bool switchFunc(HardwareSerial &serial);
    std::vector<std::string> getFuncNames(void) { return func_names; }
};