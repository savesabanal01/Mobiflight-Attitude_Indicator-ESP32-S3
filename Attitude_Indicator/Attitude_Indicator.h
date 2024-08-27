#pragma once

#include "Arduino.h"

class Attitude_Indicator
{
public:
    Attitude_Indicator(uint8_t Pin1, uint8_t Pin2);
    void begin();
    void attach(uint16_t Pin3, char *init);
    void detach();
    void set(int16_t messageID, char *setPoint);
    void update();

private:
    bool    _initialised;
    uint8_t _pin1, _pin2, _pin3;

    float pitch = 0;       // pitch value from sim
    float roll = 0;        // roll value from sim
    float pitchPosition = 0; // pitch Postion on the screen based on the pitch angle
    bool powerSaveFlag = false;
    float instrumentBrightnessRatio = 1;
    int instrumentBrightness = 255;
    int screenRotation = 3;
    int prevScreenRotation = 3;

    // Function declarations
    float scaleValue(float x, float in_min, float in_max, float out_min, float out_max);
    void  drawAll();
    void setPitch(float value);
    void setRoll(float value);
    void setScreenRotation(int rotation);
    void setPowerSaveMode(bool enabled);
    void setInstrumentBrightnessRatio(float ratio);

};