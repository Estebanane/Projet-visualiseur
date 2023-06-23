#include "led.hpp"

#include <stdlib.h>
#include <stdio.h>

LedColor Led::_pileColor[5] = { 
    LedColor(0, 0, 255, 5),
    LedColor(0, 0, 255, 5),
    LedColor(0, 0, 255, 5),
    LedColor(0, 0, 255, 5),
    LedColor(0, 0, 255, 5)
};

bool __parseColor(const std::string &text, size_t start, LedColor &dest, uint16_t &number)
{
    size_t pos, latest;
    std::string c;

    // Couleur Rouge
    pos = text.find(" ", start);
    if(pos == std::string::npos)
        return false;
    c = text.substr(4, pos);
    dest.R = std::stoi(c);

    latest = pos + 1;

    // Couleur Verte
    pos = text.find(" ", latest);
    if(pos == std::string::npos)
        return false;
    c = text.substr(latest, pos);
    dest.G = std::stoi(c);

    latest = pos + 1;

    // Couleur Bleue
    pos = text.find(" ", latest);
    if(pos == std::string::npos)
        return false;
    c = text.substr(latest, pos);
    dest.B = std::stoi(c);

    latest = pos + 1;

    // Intensité
    pos = text.find(" ", latest);
    if(pos == std::string::npos)
        return false;
    c = text.substr(latest, pos);
    dest.A = std::stoi(c);

    latest = pos + 1;

    // Numéro de la LED
    pos = text.find(" ", latest);
    if(pos == std::string::npos) {
        pos = text.find("\r", latest);
        if(pos == std::string::npos) {
            pos = text.find("\n", latest);
            if(pos == std::string::npos) {
                if(latest >= text.size())
                    return false;
                pos = text.size();
            }
        }
    }

    c = text.substr(latest, pos);
    number = std::stoi(c);

    return true;
}

/*
====================
LedColor constructor
====================
*/
LedColor::LedColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    : R(r), G(g), B(b), A(a)
{ }

/*
===============
Led constructor
===============
*/
Led::Led(uint16_t ledNumber, uint clkGpio, uint mosiGpio, spi_inst_t *spi)
    : _ledNumber(ledNumber), _clkGpio(clkGpio), _mosiGpio(mosiGpio), _spi(spi)
{
    // Initialise le bus SPI à 1 MHz
    spi_init(spi, 1000 * 1000);

    // Configure le format des trames SPI
    spi_set_format( spi,
                    8,              // Nombre de bit pour chaque donnée
                    (spi_cpol_t) 0,              // Mode de polarité
                    (spi_cpha_t) 0,              // Mode de la phase
                    SPI_MSB_FIRST   // MSB LSB
    );

    // Indique que la broche SPI est le maître
    spi_set_slave(spi, false);

    // Attribue les fonctions SPI aux broches spécifiées
    gpio_set_function(_clkGpio, GPIO_FUNC_SPI);
    gpio_set_function(_mosiGpio, GPIO_FUNC_SPI);

    _bufferSize = (LED_START_FRAME_LENGTH + (_ledNumber * LED_RGB_FRAME_LENGTH) + (((_ledNumber - 1) / 16)) + 1) * sizeof(*_buffer);
    _buffer = (uint8_t *) malloc(_bufferSize);
    if(_buffer == NULL)
        _bufferSize = 0;
}

/*
==============
Led destructor
==============
*/
Led::~Led()
{
    if(_buffer != NULL)
        free(_buffer);
}

/*
===============
Led::startFrame
===============
*/
bool Led::startFrame()
{
    uint8_t i;

    if(_buffer == NULL || _bufferSize < LED_START_FRAME_LENGTH)
        return false;
    
    for(i = 0; i < LED_START_FRAME_LENGTH; ++i)
        _buffer[i] = 0x00;

    return true;
}

/*
============
Led::turnOff
============
*/
void Led::turnOff()
{
    uint16_t number;

    for(number = 0; number < _ledNumber; ++number)
        setColor(number, LED_OFF);
}

/*
===============
Led::copyBuffer
===============
*/
bool Led::copyBuffer(const uint8_t *source, uint16_t size)
{
    uint16_t i;

    if(_buffer == NULL)
        return false;

    if(size > _bufferSize)
        size = _bufferSize;

    for(i = 0; i < size; ++i)
        _buffer[i] = source[i];

    return true;
}

/*
=============
Led::setColor
=============
*/
bool Led::setColor(uint16_t ledNumber, const LedColor &color)
{
    uint16_t index = LED_START_FRAME_LENGTH + (ledNumber * LED_RGB_FRAME_LENGTH);

    if(_buffer == NULL || index + 3 >= _bufferSize)
        return false;

    // L'intensité lumineuse ne doit pas dépasser 0x1F (31) pour ne pas changer les 3 bit à 1
    // de poids forts qui sont obligatoires dans la trame
    _buffer[index] = 0xE0 | (color.A <= 0x1F ? color.A : 0x1F);     // 0xEO = 0b11100000
    // Dans la trame, les couleurs sont dans l'ordre BGR et non RGB
    _buffer[index + 1] = color.B;
    _buffer[index + 2] = color.G;
    _buffer[index + 3] = color.R;

    return true;
}

/*
==============
Led::setColors
==============
*/
bool Led::setColors(uint16_t ledOffset, const LedColor *colors, uint16_t numberOfColors)
{
    uint16_t index;

    if(ledOffset + numberOfColors > _ledNumber)
        numberOfColors = _ledNumber;

    for(index = 0; index < numberOfColors; ++index) {
        if(!setColor(ledOffset + index, colors[index]))
            return false;
    }

    return true;
}

/*
=============
Led::endFrame
=============
*/
bool Led::endFrame()
{
    uint16_t i;

    if(_buffer == NULL)
        return false;

    for(i = LED_START_FRAME_LENGTH + (_ledNumber * LED_RGB_FRAME_LENGTH); i < _bufferSize; ++i)
        _buffer[i] = 0x00;

    return true;
}

/*
=================
Led::setLedNumber
=================
*/
bool Led::setLedNumber(uint16_t number)
{
    uint16_t newSize;
    void *ptr;

    // Si le nombre de LEDs voulues est déjà attribué, pas besoin de continuer
    if(number == _ledNumber)
        return true;

    newSize = (LED_START_FRAME_LENGTH + (number * LED_RGB_FRAME_LENGTH) + (((number - 1) / 16)) + 1) * sizeof(*_buffer);
    ptr = realloc(_buffer, newSize);
    if(ptr == NULL)
        return false;

    _buffer = (uint8_t *) ptr;
    _bufferSize = newSize;
    _ledNumber = number;

    return true;
}

/*
===================
Led::executeCommand
===================
*/
bool Led::executeCommand(const std::string &command)
{
    uint16_t number;
    LedColor color;

    // Contrôle la couleur des LEDs
    if(command.rfind("RGB ", 0) == 0) {
        if(__parseColor(command, 4, color, number))
            setColor(number, color);
    // Couleur d'une pile
    }else if(command.rfind("PILE ", 0) == 0) {
        if(__parseColor(command, 5, color, number)) {
            LedColor pile[] = {
                color,
                color,
                color,
                color,
                color,
                color,
                color,
                color,
                color,
                color
            };
            setPileColors(number, 10, pile);
        }
            
    }

    return true;
}

/*
==============
Led::turnLedOn
==============
*/
void Led::turnLedOn(LedColor *pile, uint8_t pileNumber, uint16_t numberOfLedsPerPile, uint16_t numberOfLeds){
    uint16_t i;

    for(i = 0; i < numberOfLeds; ++i)
        pile[i] = _pileColor[pileNumber];

    for(; i < numberOfLedsPerPile; ++i)
        pile[i] = LED_OFF;
}
