/**
 * @file knock_config.h
 * Levin software knock inputs. Both channels must be ADC3-capable pins on F4:
 * ADC1 is the slow ADC and ADC2 the fast (MAP averaging) ADC.
 */

#pragma once

// Knock is on ADC3
#define KNOCK_ADC ADCD3

// KNOCK1 - connector CN11_11 - pin PC3 (ADC123_IN13)
#define KNOCK_ADC_CH1 ADC_CHANNEL_IN13
#define KNOCK_PIN_CH1 Gpio::C3

// KNOCK2 - connector CN11_10 - pin PC0 (ADC123_IN10)
#define KNOCK_HAS_CH2 true
#define KNOCK_ADC_CH2 ADC_CHANNEL_IN10
#define KNOCK_PIN_CH2 Gpio::C0

// Sample rate & time - depends on the exact MCU
#define KNOCK_SAMPLE_TIME ADC_SAMPLE_84
#define KNOCK_SAMPLE_RATE (STM32_PCLK2 / (4 * (84 + 12)))
