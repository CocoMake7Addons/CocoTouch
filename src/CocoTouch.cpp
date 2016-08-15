#include "Arduino.h"
#include "CocoTouch.h"
#include <util/delay.h>

// ATMEL ATTINY85
//
//                   +-\/-+
//             PB5  1|    |8  VCC
//        ADC3/PB3  2|    |7  PB2/ADC1
//        ADC2/PB4  3|    |6  PB1*
//             GND  4|    |5  PB0*
//                   +----+
//
// * indicates PWM port
//

#define CHARGE_DELAY  5 // time it takes for the capacitor to get charged/discharged in microseconds
#define TRANSFER_DELAY  5 // time it takes for the capacitors to exchange charge
#define TOUCH_READ_DELAY 8

#define ADMUX_MASK  0b00001111 // mask the mux bits in the ADMUX register
#define MUX_GND 0b00001111 // mux value for connecting the ADC unit internally to GND
#define MUX_REF_VCC 0b00000000 // value to set the ADC reference to Vcc
#define MUX_ADMUX_GND 0b00001101 // value to set the ADC reference to Vcc


CocoTouchClass::CocoTouchClass()
{
    delay = 1;
    _value = 0;
}

void CocoTouchClass::begin()
{
    this->setAdcSpeed(3);
}



void CocoTouchClass::setAdcSpeed(uint8_t mode)
{
    ADMUX = (0<<REFS1) | (0<<REFS0); //REFS0=0:VCC reference, =1:internal reference 1.1V

    switch (mode) {
      case 1:
        ADCSRA = (1<<ADEN)| (0<<ADPS2)|(0<<ADPS1)|(1<<ADPS0); //ADC enable, prescaler 2
        break;
      case 2:
        ADCSRA = (1<<ADEN)| (0<<ADPS2)|(1<<ADPS1)|(0<<ADPS0); //ADC enable, prescaler 4
        break;
      case 3:
        ADCSRA = (1<<ADEN)| (0<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //ADC enable, prescaler 8
        break;
      case 4:
        ADCSRA = (1<<ADEN)| (1<<ADPS2)|(0<<ADPS1)|(0<<ADPS0); //ADC enable, prescaler 16
        break;
      case 5:
        ADCSRA = (1<<ADEN)| (1<<ADPS2)|(0<<ADPS1)|(1<<ADPS0); //ADC enable, prescaler 32
        break;
      case 6:
        ADCSRA = (1<<ADEN)| (1<<ADPS2)|(1<<ADPS1)|(0<<ADPS0); //ADC enable, prescaler 64
        break;
      case 7:
        ADCSRA = (1<<ADEN)| (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //ADC enable, prescaler 128
      break;
    default:
        break;
    }

}


/*

                                  ^ 5V
                                  |
                                  |
                                  | +   enable disable refPin
                                  | +--------+
                                  | +
                                  |
                                 +++
                                 | |
                                 | |
                                 +++                             +---------+
                Touch             |                              |         |
                Probe  +----------+--------+          <-----+----+  ADCpin |
                       |                                    |    |         |
                       |                                    |    +---------+
                     +---+              +--+                |
                     +---+              |                 +---+
                       |                |                 +---+
                       |                |                   |
                     +---+            +---+               +---+
                      +-+              +-+                 +-+
                       +                +                   +

                                     ADMUX                sample and hold
                                     (multiplexer)



 */



uint16_t CocoTouchClass::sense(byte adcPin, byte refPin, uint8_t samples)
{
    int muxAdc = 0;
    int muxRef = 0;
    int measurement1, measurement2;
    int QTouchDelay = 5;
    _value = 0;

    if (adcPin == PB3) muxAdc = 0x03;
    if (adcPin == PB4) muxAdc = 0x02;
    if (adcPin == PB2) muxAdc = 0x01;
    if (adcPin == PB5) muxAdc = 0x00;

    if (refPin == PB3) muxRef = 0x03;
    if (refPin == PB4) muxRef = 0x02;
    if (refPin == PB2) muxRef = 0x01;
    if (refPin == PB5) muxRef = 0x00;

    for(int _counter = 0; _counter < samples; _counter ++)
        {
            //this->usb_poll();
            // first measurement: adcPin low, S/H high
            ADMUX = (0<<REFS0) | (muxRef); // set ADC sample+hold condenser to the free A0 (ADC0)
            //_delay_us(QTouchDelay);
            PORTB |= (1<<refPin); //PC0/ADC0 ref/ S/H high (pullup or output, doesn't matter)
            //PORTB &= ~(1<<adcPin);
            DDRB |= (1<<adcPin) | (1<<refPin); // both output: adcPin low, S/H (ADC0) high

            _delay_us(TOUCH_READ_DELAY);
            PORTB &= ~((1<<adcPin) | (1<<refPin)); // ... and low: Tristate

            DDRB &= ~((1<<adcPin) | (1<<refPin)); // set pins to Input...

            ADMUX = (0<<REFS0) | (muxAdc); //  read extern condensator from adcPin
            ADCSRA |= (1<<ADSC); // start conversion
            while (!(ADCSRA & (1 << ADIF))); // wait for conversion complete
            ADCSRA |= (1 << ADIF); // clear ADIF
            measurement1=ADC;

            //measurement1 = analogRead(adcPin);

            // second measurement: adcPin high, S/H low
            ADMUX = (0<<REFS0) | (muxAdc); // set ADC sample+hold condenser to the free PC0 (ADC0)
            //_delay_us(QTouchDelay);
            PORTB |= (1<<adcPin); // sensePad/adcPin high
            //PORTB &= ~(1<<refPin);
            DDRB |= (1<<adcPin) | (1<<refPin); // both output: adcPin high, S/H (ADC0) low

            _delay_us(TOUCH_READ_DELAY);
            PORTB &= ~((1<<adcPin) | (1<<refPin));

            DDRB &= ~((1<<adcPin) | (1<<refPin));

            ADMUX = (0<<REFS0) | (muxAdc); //   read extern condensator from adcPin
            ADCSRA |= (1<<ADSC); // start conversion
            while (!(ADCSRA & (1 << ADIF))); // wait for conversion complete
            ADCSRA |= (1 << ADIF); // clear ADCIF
            measurement2=ADC;

            //measurement2 = analogRead(adcPin);

            _value += (measurement2 - measurement1);

        }

    return _value / samples;
}



CocoTouchClass CocoTouch;
