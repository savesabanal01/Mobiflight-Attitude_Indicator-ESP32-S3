#include "Attitude_Indicator.h"
#include "allocateMem.h"
#include "commandmessenger.h"
#include <TFT_eSPI.h>
#include <SPI.h>
#include <background.h>
#include <bezel.h>
#include <roll_scale.h>
#include <pitch_scale.h>

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

TFT_eSprite mainSpr = TFT_eSprite(&tft);       // Sprite to hold everything
TFT_eSprite pitchScaleSpr = TFT_eSprite(&tft); // Sprite to hold everything
TFT_eSprite rollScaleSpr = TFT_eSprite(&tft);  // Sprite to hold everything
TFT_eSprite bezelSpr = TFT_eSprite(&tft);      // Sprite to hold everything



/* **********************************************************************************
    This is just the basic code to set up your custom device.
    Change/add your code as needed.
********************************************************************************** */

Attitude_Indicator::Attitude_Indicator(uint8_t Pin1, uint8_t Pin2)
{
    _pin1 = Pin1;
    _pin2 = Pin2;
}

void Attitude_Indicator::begin()
{

  Serial.begin(115200);

  delay(1000); // wait for serial monitor to open

  tft.begin();
  tft.setRotation(screenRotation);
  tft.fillScreen(TFT_BLACK);
  tft.setPivot(240, 160);

  mainSpr.createSprite(320, 320);
  mainSpr.setSwapBytes(true);
  mainSpr.fillSprite(TFT_BLACK);
  mainSpr.setPivot(160, 160);

  rollScaleSpr.createSprite(roll_scale_width, roll_scale_height);
  rollScaleSpr.setSwapBytes(false);
  rollScaleSpr.fillSprite(TFT_BLACK);

  pitchScaleSpr.createSprite(pitch_scale_width, pitch_scale_height);
  pitchScaleSpr.setSwapBytes(false);
  pitchScaleSpr.fillSprite(TFT_BLACK);
  pitchScaleSpr.pushImage(0, 0, pitch_scale_width, pitch_scale_height, pitch_scale);

  bezelSpr.createSprite(320, 320);
  bezelSpr.setSwapBytes(true);
  bezelSpr.fillSprite(TFT_BLACK);
  bezelSpr.pushImage(0, 0, bezel_width, bezel_height, bezel);
  bezelSpr.setPivot(160, 160);

}

void Attitude_Indicator::attach(uint16_t Pin3, char *init)
{
    _pin3 = Pin3;
}

void Attitude_Indicator::detach()
{
    if (!_initialised)
        return;
    _initialised = false;
}

void Attitude_Indicator::set(int16_t messageID, char *setPoint)
{
    /* **********************************************************************************
        Each messageID has it's own value
        check for the messageID and define what to do.
        Important Remark!
        MessageID == -2 will be send from the board when PowerSavingMode is set
            Message will be "0" for leaving and "1" for entering PowerSavingMode
        MessageID == -1 will be send from the connector when Connector stops running
        Put in your code to enter this mode (e.g. clear a display)

    ********************************************************************************** */
    int32_t  data = atoi(setPoint);
    // uint16_t output;

    // do something according your messageID
    switch (messageID) {
    case -1:
        // // tbd., get's called when Mobiflight shuts down        
        tft.drawString("Message ID 1: " + String(messageID), 10, 10, 2);
        tft.drawString("Data 1: " + String(data), 10, 10, 2);
        setPowerSaveMode(true);
    case -2:
        // // tbd., get's called when PowerSavingMode is entered
        tft.drawString("Data 2: " + String(data), 10, 30, 2);
        if (data == 1)
            setPowerSaveMode(true);
        else if (data == 0)
            setPowerSaveMode(false);
    case 0:
        // output = (uint16_t)data;
        // data   = output;
        setPitch(atof(setPoint));
        break;
    case 1:
        /* code */
        setRoll(atof(setPoint));
        break;
    case 2:
        /* code */
        setInstrumentBrightnessRatio(atof(setPoint));
        break;
    case 100:
        /* code */
        setScreenRotation(atoi(setPoint));
        break;
    default:
        break;
    }
}

void Attitude_Indicator::update()
{
    // Do something which is required regulary
  if(!powerSaveFlag)
  {

    analogWrite(TFT_BL, instrumentBrightness);

    if(prevScreenRotation != screenRotation)
    {
        tft.setRotation(screenRotation);
        prevScreenRotation = screenRotation;
    }
    drawAll();

   }
   else digitalWrite(TFT_BL, HIGH);

}

void Attitude_Indicator::drawAll()
{

  pitchPosition = round(scaleValue(pitch, -40, 40, -80, 80));
  mainSpr.pushImage(20, 20, 280, 280, background);
  pitchScaleSpr.pushImage(0, 0, pitch_scale_width, pitch_scale_height, pitch_scale);

  mainSpr.setViewport(30, 30, 260, 260);
  pitchScaleSpr.pushToSprite(&mainSpr, 28, pitchPosition + 58, TFT_BLACK);

  mainSpr.setViewport(0, 0, 320, 320);
  rollScaleSpr.pushImage(0, 0, roll_scale_width, roll_scale_height, roll_scale);
  rollScaleSpr.pushToSprite(&mainSpr, 20, 20, TFT_BLACK);

  mainSpr.setPivot(160, 160);
  mainSpr.setSwapBytes(true);

  bezelSpr.pushRotated(&mainSpr, roll, TFT_BLACK);
  mainSpr.pushRotated(-roll, TFT_BLACK);

}

void Attitude_Indicator::setPitch(float value)
{
    if (value >= 40)
        pitch = 40;
    else if (value <= -40)
        pitch = -40;
    else pitch = value;
}

void Attitude_Indicator::setRoll(float value)
{
    roll = value;
}

void Attitude_Indicator::setScreenRotation(int rotation)
{
  if(rotation == 1 || rotation == 3)
    screenRotation = rotation;
}

void Attitude_Indicator::setPowerSaveMode(bool enabled)
{
    if(enabled)
    {
        digitalWrite(TFT_BL, LOW);
        powerSaveFlag = true;
    }
    else
    {
        analogWrite(TFT_BL, instrumentBrightness);
        powerSaveFlag = false;
    }
}

void Attitude_Indicator::setInstrumentBrightnessRatio(float ratio)
{
  instrumentBrightnessRatio = ratio;
  instrumentBrightness = round(scaleValue(instrumentBrightnessRatio, 0, 1, 0, 255));
}

float Attitude_Indicator::scaleValue(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}