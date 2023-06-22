#include "bt.hpp"

const char *BT::ATCommand           = "AT\r\n";
const char *BT::ATNameCommand       = "AT+NAME\r\n";
const char *BT::ATVersionCommand    = "AT+VERSION\r\n";
const char *BT::ATUartCommand       = "AT+UART\r\n";

/*
==============
BT constructor
==============
*/
BT::BT(uart_inst_t *uart, uint txGpio, uint rxGpio)
    : _uart(uart), _txGpio(txGpio), _rxGpio(rxGpio)
{
    // Initialise le bus UART à 115200 bauds
    uart_init(_uart, 115200);

    // Initialise la broche tout ou rien pour configurer le module
    // gpio_init(_keyGpio);
    // gpio_set_dir(_keyGpio, GPIO_OUT);
    // gpio_put(_keyGpio, _atCommandMode);

    // Attribue les fonctions UART aux broches spécifiées
    gpio_set_function(_txGpio, GPIO_FUNC_UART);
    gpio_set_function(_rxGpio, GPIO_FUNC_UART);
}

/*
============
BT::readByte
============
*/
uint8_t BT::readByte() const
{
    uint8_t b;
    uart_read_blocking(_uart, &b, sizeof(b));
    return b;
}

/*
============
BT::readByte
============
*/
std::string BT::readLine() const
{
    std::string s;
    char c;

    while(true) {
        if(dataAvailable()) {
            c = readByte();
            if(c != '\r') {
                if(c == '\n')
                    break;
                s += c;
            }                
        }
    }

    return s;
}
