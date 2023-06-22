#ifndef __LED_HPP__
#define __LED_HPP__

#include <stdint.h>
#include <string>

#include "pico/stdlib.h"
#include "hardware/spi.h"

#define LED_START_FRAME_LENGTH  4
#define LED_RGB_FRAME_LENGTH    4

#define LED_OFF     LedColor(100, 200, 50, 0)
#define LED_RED     LedColor(255, 0, 0)
#define LED_GREEN   LedColor(0, 255, 0)
#define LED_BLUE    LedColor(0, 0, 255)
#define LED_MAGENTA LedColor(255, 0, 255)
#define LED_WHITE   LedColor(255, 255, 255)

struct LedColor {
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t A;

    LedColor(uint8_t r = 255, uint8_t g = 255, uint8_t b = 255, uint8_t a = 0x1F);
};

class Led {

    private:
        static LedColor _pileColor[5];

    private:
        uint _clkGpio;
        uint _mosiGpio;
        uint16_t _ledNumber;
        spi_inst_t *_spi;
        uint16_t _bufferSize;
        uint8_t *_buffer;

    public:
        Led() = delete;
        Led(uint16_t ledNumber, uint clkGpio, uint mosiGpio, spi_inst_t *spi);
        ~Led();

        bool startFrame();
        void turnOff();
        bool copyBuffer(const uint8_t *source, uint16_t size);
        bool setColor(uint16_t ledNumber, const LedColor &color);
        bool setColors(uint16_t ledOffset, const LedColor *colors, uint16_t numberOfColors);
        bool setPileColors(uint8_t pileNumber, uint16_t numberOfLedPerPile, const LedColor *colors);
        bool endFrame();

        bool executeCommand(const std::string &command);

        int send() const;
        int send(uint16_t size) const;

        bool setLedNumber(uint16_t number);
        uint16_t getLedNumber() const;

        static void turnLedOn(LedColor *pile, uint8_t pileNumber, uint16_t numberOfLedsPerPile, uint16_t numberOfLeds);
};

/*
==================
Led::setPileColors
==================
*/
inline bool Led::setPileColors(uint8_t pileNumber, uint16_t numberOfLedPerPile, const LedColor *colors)
{
    return setColors(pileNumber * numberOfLedPerPile, colors, numberOfLedPerPile);
}

/*
=========
Led::send
=========
*/
inline int Led::send() const
{
    return spi_write_blocking(_spi, _buffer, _bufferSize);
}

/*
=========
Led::send
=========
*/
inline int Led::send(uint16_t size) const
{
    return spi_write_blocking(_spi, _buffer, size);
}

/*
=================
Led::getLedNumber
=================
*/
inline uint16_t Led::getLedNumber() const
{
    return _ledNumber;
}

#endif