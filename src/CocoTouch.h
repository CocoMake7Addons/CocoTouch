/*
  QTouchSense.h - Library for Capacitive touch sensors using only one ADC PIN and a Reference
  modified from https://github.com/jgeisler0303/QTouchADCArduino
  Released into the public domain.
*/

#ifndef QTouchADCTiny_h
#define QTouchADCTiny_h


#include <Arduino.h>

class CocoTouchClass
{
  public:
    CocoTouchClass();
    void begin();
    uint16_t sense(byte adcPin, byte refPin, uint8_t samples);
    void setAdcSpeed(uint8_t mode);
    uint8_t delay;
    void (*delay_cb)(uint8_t* text);
    void (*usb_poll)(void);
    int _value;

};

extern CocoTouchClass CocoTouch;

#endif

