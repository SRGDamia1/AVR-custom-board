#ifndef Pins_Arduino_h
#define Pins_Arduino_h

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include <avr/pgmspace.h>

// ${variant_version_macros}

/*----------------------------------------------------------------------------
 *        Pins
 *----------------------------------------------------------------------------*/

// pin counts
#define NUM_DIGITAL_PINS (32u)
#define NUM_ANALOG_INPUTS (8u)

/**
 * Analog pin definitions
 * Explicitly define each analog pin number to its corresponding digital pin number
 */

// Mapping from analog pin number to digital pin number
// This can be a function if pin numbers are contiguous or it can specify the mapping for each pin explicitly.
#define analogInputToDigitalPin(p) ((p < NUM_ANALOG_INPUTS) ? (p) + 24 : -1)

// Standard analog pins as defines
static const uint8_t A0 = 24;
static const uint8_t A1 = 25;
static const uint8_t A2 = 26;
static const uint8_t A3 = 27;
static const uint8_t A4 = 28;
static const uint8_t A5 = 29;
static const uint8_t A6 = 30;
static const uint8_t A7 = 31;

/**
 * Interrupt Mapping
 * External Interrupts have a have a dedicated mapping - one pin per interrupt.
 * Pin Change Interrupts are mapped to ports.
 */

// Mapping from digital pin number to EXTERNAL interrupt number
#define digitalPinToInterrupt(p) ((p) == 10 ? 2 : ((p) == 2 ? 0 : ((p) == 3 ? 1 : NOT_AN_INTERRUPT)))

// Mapping from digital pin number to pin change interrupt control register (PCICR) and bit
#define digitalPinToPCICR(p) (((p) >= 0 && (p) < NUM_DIGITAL_PINS) ? (&PCICR) : ((uint8_t *)0))
#define digitalPinToPCICRbit(p) (((p) <= 7) ? 3 : (((p) <= 15) ? 1 : (((p) <= 23) ? 2 : 0)))
// Mapping from digital pin number to pin change mask register (PCMSK) and bit
#define digitalPinToPCMSK(p) (((p) <= 7) ? (&PCMSK3) : (((p) <= 15) ? (&PCMSK1) : (((p) <= 23) ? (&PCMSK2) : (&PCMSK0))))
#define digitalPinToPCMSKbit(p) ((p) % 8)

/**
 * Pulse Width Modulation (PWM) Mapping
 */

// Boolean function to check if a digital pin supports PWM
#define digitalPinHasPWM(p) ((p) == 11 || (p) == 12 || (p) == 14 || (p) == 15 || (p) == 4 || (p) == 5 || (p) == 6 || (p) == 7)

#ifdef ARDUINO_MAIN

/**
 * Port Mappings
 *
 * These arrays map port names (e.g. port B) to the appropriate addresses for
 * various functions (e.g. reading and writing)
 */

// These are used as index in port_to_XYZ arrays, right?
#define PA 1
#define PB 2
#define PC 3
#define PD 4

const uint16_t PROGMEM port_to_mode_PGM[] =
    {
        NOT_A_PORT,
        (uint16_t)&DDRA,
        (uint16_t)&DDRB,
        (uint16_t)&DDRC,
        (uint16_t)&DDRD,
};

const uint16_t PROGMEM port_to_output_PGM[] =
    {
        NOT_A_PORT,
        (uint16_t)&PORTA,
        (uint16_t)&PORTB,
        (uint16_t)&PORTC,
        (uint16_t)&PORTD,
};

const uint16_t PROGMEM port_to_input_PGM[] =
    {
        NOT_A_PORT,
        (uint16_t)&PINA,
        (uint16_t)&PINB,
        (uint16_t)&PINC,
        (uint16_t)&PIND,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] =
    {
        PD, /* D0 */
        PD,
        PD,
        PD,
        PD,
        PD,
        PD,
        PD,
        PB, /* D8 */
        PB,
        PB,
        PB,
        PB,
        PB,
        PB,
        PB,
        PC, /* D16 */
        PC,
        PC,
        PC,
        PC,
        PC,
        PC,
        PC,
        PA, /* D24 */
        PA,
        PA,
        PA,
        PA,
        PA,
        PA,
        PA /* D31 */
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] =
    {
        _BV(0), /* D0, port D */
        _BV(1),
        _BV(2),
        _BV(3),
        _BV(4),
        _BV(5),
        _BV(6),
        _BV(7),
        _BV(0), /* D8, port B */
        _BV(1),
        _BV(2),
        _BV(3),
        _BV(4),
        _BV(5),
        _BV(6),
        _BV(7),
        _BV(0), /* D16, port C */
        _BV(1),
        _BV(2),
        _BV(3),
        _BV(4),
        _BV(5),
        _BV(6),
        _BV(7),
        _BV(0), /* D24, port A */
        _BV(1),
        _BV(2),
        _BV(3),
        _BV(4),
        _BV(5),
        _BV(6),
        _BV(7)};

/**
 * Timer/Counter Mapping
 */
const uint8_t PROGMEM digital_pin_to_timer_PGM[] =
    {
        NOT_ON_TIMER, /* D0  - PD0 */
        NOT_ON_TIMER, /* D1  - PD1 */
        NOT_ON_TIMER, /* D2  - PD2 */
        NOT_ON_TIMER, /* D3  - PD3 */
        TIMER1B,      /* D4  - PD4 */
        TIMER1A,      /* D5  - PD5 */
        TIMER2B,      /* D6  - PD6 */
        TIMER2A,      /* D7  - PD7 */
        NOT_ON_TIMER, /* D8  - PB0 */
        NOT_ON_TIMER, /* D9  - PB1 */
        NOT_ON_TIMER, /* D10 - PB2 */
        TIMER0A,      /* D11 - PB3 */
        TIMER0B,      /* D12 - PB4 */
        NOT_ON_TIMER, /* D13 - PB5 */
        TIMER3A,      /* D14 - PB6 */
        TIMER3B,      /* D15 - PB7 */
        NOT_ON_TIMER, /* D16 - PC0 */
        NOT_ON_TIMER, /* D17 - PC1 */
        NOT_ON_TIMER, /* D18 - PC2 */
        NOT_ON_TIMER, /* D19 - PC3 */
        NOT_ON_TIMER, /* D20 - PC4 */
        NOT_ON_TIMER, /* D21 - PC5 */
        NOT_ON_TIMER, /* D22 - PC6 */
        NOT_ON_TIMER, /* D23 - PC7 */
        NOT_ON_TIMER, /* D24 - PA0 */
        NOT_ON_TIMER, /* D25 - PA1 */
        NOT_ON_TIMER, /* D26 - PA2 */
        NOT_ON_TIMER, /* D27 - PA3 */
        NOT_ON_TIMER, /* D28 - PA4 */
        NOT_ON_TIMER, /* D29 - PA5 */
        NOT_ON_TIMER, /* D30 - PA6 */
        NOT_ON_TIMER  /* D31 - PA7 */
};

#endif // ARDUINO_MAIN

/**
 * SPI Interfaces
 *
 * SPI – Serial Peripheral Interface (Host operation)
 */
// SD Card SPI
#define PIN_SPI_MOSI (13)
// ^ Digital pin for SPI Data Out (MOSI as host, MISO as client)
#define PIN_SPI_SCK (15)
// ^ Digital pin for SPI SCK
#define PIN_SPI_SS (12)
// ^ Digital pin for SPI CS
#define PIN_SPI_MISO (14)
// ^ Digital pin for SPI Data In (MISO as host, MOSI as client)

// static constants for the SPI pins
static const uint8_t SS = PIN_SPI_SS;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK = PIN_SPI_SCK;

/**
 * Wire (I2C) Interfaces
 *
 * I2C – Inter-Integrated Circuit (Host operation)
 */
static const uint8_t SDA = 17;
static const uint8_t SCL = 16;

/*----------------------------------------------------------------------------
 * Other defines anc static constants
 *
 * Put other defines that will be convenient for your users or libraries here.
 *----------------------------------------------------------------------------*/

// LEDs
// Optional macros and static constants for user LEDs.
#define LED_BUILTIN 8
static const uint8_t LED2 = 8; // Green
static const uint8_t LED1 = 9; // Red

#endif // Pins_Arduino_h
// vim:ai:cin:sts=2 sw=2 ft=cpp

// cSpell:words RXPO DIPO DOPO MSSEN SYNCBUSY PERIPH XCLK LINUXBRIDGE
// cSpell:words ATMEGA TQFN XTAL AVCC XBEE PCICR
// cSpell:words DDRA DDRB DDRC DDRD PORTA PORTB PORTC PORTD PINA PINB PINC PIND
