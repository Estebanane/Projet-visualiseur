#ifndef __BT_HPP__
#define __BT_HPP__

#include <stdint.h>
#include <string>

#include "pico/stdlib.h"
#include "hardware/uart.h"

#define BT_AT_COMMAND_RESPONSE_LENGTH 4

class BT {

    public:
        static const char *ATCommand;
        static const char *ATNameCommand;
        static const char *ATVersionCommand;
        static const char *ATUartCommand;


    private:
        uint _txGpio;
        uint _rxGpio;
        uart_inst_t *_uart;

    public:
        BT() = delete;
        BT(uart_inst_t *uart, uint txGpio, uint rxGpio);

        void send(const std::string &text) const;
        void read(uint8_t *buffer, uint16_t size) const;
        uint8_t readByte() const;
        std::string readLine() const;

        bool dataAvailable() const;

        void setATCommandMode(bool value);
        bool getATCommandMode() const;

};

/*
========
BT::send
========
*/
inline void BT::send(const std::string &text) const
{
    uart_write_blocking(_uart, (const uint8_t *) text.c_str(), text.size());
    sleep_ms(1000);
}

/*
========
BT::read
========
*/
inline void BT::read(uint8_t *buffer, uint16_t size) const
{
    uart_read_blocking(_uart, buffer, size);
}

/*
=================
BT::dataAvailable
=================
*/
inline bool BT::dataAvailable() const
{
    return uart_is_readable(_uart);
}

#endif