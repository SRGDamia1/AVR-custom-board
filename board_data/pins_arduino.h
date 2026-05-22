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
#define ${board_name_upper}_VERSION ${board_name_upper}_VERSION_VAL(${board_name_upper}_VERSION_MAJOR, ${board_name_upper}_VERSION_MINOR, ${board_name_upper}_VERSION_PATCH)
// clang-format on

/*----------------------------------------------------------------------------
 *        Pins
 *----------------------------------------------------------------------------*/

// clang-format off
// ATMEL ATMEGA1284P Process Diagram
// (Notice that EnviroDIY Mayfly is equipped with TQFN package. The layout below is
// just a convenience to see the port names and their usage.)
//
//                 |-----ICR BIT 1 (B)----|           |--ICR BIT 0 (A)--|
//                 SD             RED  GREEN          |--Processor ADC--|
//                CS/SS           LED1 LED2           D24  D25  D26  D27
//                 D12  D11  D10  D09  D08  GND  VCC  A00  A01  A02  A03
// ____            _44___43___42___41___40___39___38___37___36___35___34_                  ____
//  |        MOSI 1| *                                                   |33 A04/D28        |
// ICR             |                                                     |                  |
// BIT  MISO/OC3A 2|                                                     |32 A05/D29       ICR
// 1(B)            |                                                     |                 BIT
//__|__  SCK/OC3B 3|                                                     |31 A06/D30 BATT  0(A)
//                 |                                                     |                  |
//         RESET  4|                                                     |30 A07/D31 RTC  __|__
//                 |                                                     |
//           VCC  5|                                                     |29 AREF
//           GND  6|                                                     |28 GND
//         XTAL2  7|                                                     |27 AVCC
//                 |                                                     |                      ____
//         XTAL1  8|                                                     |26 D23 XBEE DTR        |
//    ____         |                                                     |                       |
//     |    RXD0  9|                                                     |25 D22 SWITCHED POWER ICR
//    ICR          |                                                     |                      BIT
//    BIT   TXD0 10|                                                     |24 TDI D21 BUTTON     2(C)
//    3(D)         |                                                     |                       |
//   __|__  RXD1 11|_____________________________________________________|23 TDO D20 XBEE RTS  __|__
//                 12   13   14   15   16   17   18   19   20   21   22
//                 TXD2 D04  D05  D06  D07  VCC  GND  SCL  SDA  TCK  TMS
//                |-----ICR BIT 3 (D)----|                      D18  D19
//                                                                   XBEE
//                                                                   CTS
//                                                   |--ICR BIT 2 (C)--|
// clang-format on

/*
 * 0..32
 * PD0..PD7, PB0..PB7, PC0..PC7, PA0..PA7
 */

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
 *     D2 (INT0), D3 (INT1), D10 (INT2)
 * Pin Change Interrupts are mapped to ports.
 *     PCINT31-24: D7-0   : bit 3(D)
 *     PCINT15-8:  D15-8  : bit 1(B)
 *     PCINT23-16: D23-16 : bit 2(C)
 *     PCINT7-0:   D31-24 : bit 0(A) (also A0..A7)
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
// SPI Pins
#define PIN_SPI_MOSI (13)
// ^ Digital pin for SPI Data Out (MOSI as host, MISO as client)
#define PIN_SPI_MISO (14)
// ^ Digital pin for SPI Data In (MISO as host, MOSI as client)
#define PIN_SPI_SCK (15)
// ^ Digital pin for SPI SCK

// SS for primary SPI device (the SD card on the Mayfly)
#define PIN_SPI_SS (12)
// ^ Digital pin for SPI CS

// defines for the SD Card for SDFat - Adafruit Fork
#define SDCARD_SPI SPI
// ^ The SPI interface to use (e.g. SPI, SPI1, etc.)
#define SDCARD_SS_PIN (12)
// ^ The chip select for the SD card (not the external flash, which is EXTERNAL_FLASH_USE_CS)

// defines for the internal flash for Adafruit SPI Flash library
#define EXTERNAL_FLASH_USE_SPI SPI
// ^ The SPI interface to use (e.g. SPI, SPI1, etc.)
#define EXTERNAL_FLASH_USE_CS (20)
// ^ The chip select for the external flash (not the SD card, which is PIN_SPI_SS)

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

// X-Bee Socket
#define SerialBee Serial1
static const uint8_t BEEPWR = 18;
static const uint8_t BEERX = 11;
static const uint8_t BEETX = 12;
static const uint8_t BEEDTR = 23;
static const uint8_t BEERTS = 29; // A5=29, must switch solder jumper
static const uint8_t BEECTS = 19;
static const uint8_t BEESTATUS = 19; // must switch solder jumper
static const uint8_t BEERESET = 29;  // A5=29

// Grove ports
static const uint8_t GROVEPWR = 22;
static const uint8_t GROVEPWR_OFF = 0;
static const uint8_t GROVEPWR_ON = 1;

// Battery voltage measurement
static const uint8_t BATVOLTPIN = 30; // A6
#define BATVOLT_R1 47                 // in fact 4.7M
#define BATVOLT_R2 100                // in fact 10M

#if defined(MAYFLY_VERSION) && defined(MAYFLY_VERSION_VAL) && MAYFLY_VERSION >= MAYFLY_VERSION_VAL(2, 0, 0)

#define SC16IS7XX_DEFAULT_ADDRESS \
    (0X9A) ///< A0 tied to GND, A1 tied to GND
#define SC16IS7XX_DEFAULT_XTAL_FREQ \
    (3686000UL) ///< The default frequency of the crystal in hertz

// External serial chip
// Port expander pins as static constants
static const uint8_t X0 = 0;
static const uint8_t X1 = 1;
static const uint8_t X2 = 2;
static const uint8_t X3 = 3;
static const uint8_t X4 = 4;
static const uint8_t X5 = 5;
static const uint8_t X6 = 6;
static const uint8_t X7 = 7;

// #include <SC16IS752.h>

// SC16IS752 Serial2(SC16IS752_CHANNEL_A);
// SC16IS752 Serial3(SC16IS752_CHANNEL_B);

// extern SC16IS752 Serial2;
// extern SC16IS752 Serial3;
#endif

#endif // Pins_Arduino_h
// vim:ai:cin:sts=2 sw=2 ft=cpp

// cSpell:words RXPO DIPO DOPO MSSEN SYNCBUSY PERIPH XCLK LINUXBRIDGE
// cSpell:words ATMEGA TQFN XTAL AVCC XBEE PCICR
// cSpell:words DDRA DDRB DDRC DDRD PORTA PORTB PORTC PORTD PINA PINB PINC PIND
