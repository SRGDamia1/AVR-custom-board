#ifndef Pins_Arduino_h
#define Pins_Arduino_h

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include <avr/pgmspace.h>

/*----------------------------------------------------------------------------
 *        Version Information
 *
 * Don't change this; it is used as a template!
 *----------------------------------------------------------------------------*/
// clang-format off
/** Major version number (X.x.x) */
#define ${board_name_upper}_VERSION_MAJOR $package_version_major
/** Minor version number (x.X.x) */
#define ${board_name_upper}_VERSION_MINOR $package_version_minor
/** Patch version number (x.x.X) */
#define ${board_name_upper}_VERSION_PATCH $package_version_patch

/**
 * Macro to convert version number into an integer
 *
 * To be used in comparisons, such as ${board_name_upper}_VERSION >= ${board_name_upper}_VERSION_VAL(4, 0, 0)
 */
#define ${board_name_upper}_VERSION_VAL(major, minor, patch)((major << 16) | (minor << 8) | (patch))

/**
 * Current Board version, as an integer
 *
 * To be used in comparisons, such as ${board_name_upper}_VERSION >= ${board_name_upper}_VERSION_VAL(4, 0, 0)
 */
#define ${board_name_upper}_VERSION ${board_name_upper}_VERSION_VAL(${board_name_upper}_VERSION_MAJOR, \
                                                                       ${board_name_upper}_VERSION_MINOR, \
                                                                       ${board_name_upper}_VERSION_PATCH)
// clang-format on

/*----------------------------------------------------------------------------
 *        Pins
 *----------------------------------------------------------------------------*/

#define NUM_DIGITAL_PINS (32u)
#define NUM_ANALOG_INPUTS (8u)

// Mapping from analog pin number to digital pin number
// This can be a function if pin numbers are contiguous or it can specify the mapping for each pin explicitly.
#define analogInputToDigitalPin(p) ((p < NUM_ANALOG_INPUTS) ? (p) + 24 : -1)
#define digitalPinToInterrupt(p) ((p) == 10 ? 2 : ((p) == 2 ? 0 : ((p) == 3 ? 1 : NOT_AN_INTERRUPT)))

#define digitalPinHasPWM(p) ((p) == 11 || (p) == 12 || (p) == 14 || (p) == 15 || (p) == 4 || (p) == 5 || (p) == 6 || (p) == 7)

// LEDs
// Optional macros for user LEDs.
static const uint8_t LED2 = 8; // Green
static const uint8_t LED1 = 9; // Red

/**
 * Analog pin definitions
 * Explicitly define each analog pin number to its corresponding digital pin number
 */

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
 * Pin-Change Interrupt Mapping
 */

#define digitalPinToPCICR(p) (((p) >= 0 && (p) < NUM_DIGITAL_PINS) ? (&PCICR) : ((uint8_t *)0))
#define digitalPinToPCICRbit(p) (((p) <= 7) ? 3 : (((p) <= 15) ? 1 : (((p) <= 23) ? 2 : 0)))
#define digitalPinToPCMSK(p) (((p) <= 7) ? (&PCMSK3) : (((p) <= 15) ? (&PCMSK1) : (((p) <= 23) ? (&PCMSK2) : (&PCMSK0))))
#define digitalPinToPCMSKbit(p) ((p) % 8)

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
// static constants for the SPI pins
static const uint8_t SS = 12;
static const uint8_t MOSI = 13;
static const uint8_t MISO = 14;
static const uint8_t SCK = 15;

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

static const uint8_t GROVEPWR = 22;
static const uint8_t GROVEPWR_OFF = 0;
static const uint8_t GROVEPWR_ON = 1;

static const uint8_t BEEDTR = 23;
static const uint8_t BEERTS = 20;
static const uint8_t BEECTS = 19;

static const uint8_t BATVOLTPIN = 30; // A6
#define BATVOLT_R1 47                 // in fact 4.7M
#define BATVOLT_R2 100                // in fact 10M

#endif // Pins_Arduino_h
// vim:ai:cin:sts=2 sw=2 ft=cpp
