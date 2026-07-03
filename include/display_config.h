#pragma once

#include <Arduino.h>

namespace DisplayConfig {

static constexpr int SCREEN_WIDTH = 240;
static constexpr int SCREEN_HEIGHT = 240;

static constexpr int PIN_LCD_DC = 18;
static constexpr int PIN_LCD_CS = 2;
static constexpr int PIN_LCD_WR = 3;
static constexpr int PIN_LCD_RST = 21;
static constexpr int PIN_LCD_BL = 42;

static constexpr int PIN_LCD_D0 = 10;
static constexpr int PIN_LCD_D1 = 11;
static constexpr int PIN_LCD_D2 = 12;
static constexpr int PIN_LCD_D3 = 13;
static constexpr int PIN_LCD_D4 = 14;
static constexpr int PIN_LCD_D5 = 15;
static constexpr int PIN_LCD_D6 = 16;
static constexpr int PIN_LCD_D7 = 17;

static constexpr int PIN_I2C_SDA = 8;
static constexpr int PIN_I2C_SCL = 9;
static constexpr int PIN_TOUCH_RST = 0;
static constexpr int PIN_TCA_INT = 45;

static constexpr uint8_t TOUCH_I2C_ADDRESS = 0x15;
static constexpr uint8_t TOUCH_CHIP_ID_REGISTER = 0xA7;
static constexpr uint8_t TOUCH_REGISTER_XH = 0x03;
static constexpr uint8_t TOUCH_REGISTER_XL = 0x04;
static constexpr uint8_t TOUCH_REGISTER_YH = 0x05;
static constexpr uint8_t TOUCH_REGISTER_YL = 0x06;

static constexpr uint8_t TCA6408_ADDRESS = 0x20;
static constexpr uint8_t TCA6408_INPUT_PORT = 0x00;
static constexpr uint8_t TCA6408_CONFIGURATION = 0x03;

static constexpr int BACKLIGHT_CHANNEL = 1;
static constexpr int BACKLIGHT_FREQ = 5000;
static constexpr int BACKLIGHT_RESOLUTION_BITS = 8;

}  // namespace DisplayConfig
