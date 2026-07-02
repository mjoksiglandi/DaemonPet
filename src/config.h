#pragma once

#include <Arduino.h>

namespace AppConfig {

static constexpr char WIFI_SSID[] = "REPLACE_WITH_WIFI";
static constexpr char WIFI_PASSWORD[] = "REPLACE_WITH_PASSWORD";
static constexpr char STATUS_ENDPOINT[] = "http://192.168.1.20/status.json";

static constexpr uint32_t HTTP_REFRESH_MS = 5000;
static constexpr uint32_t WIFI_RETRY_MS = 10000;
static constexpr uint32_t VIEW_ROTATION_MS = 7000;
static constexpr uint32_t SPLASH_SCREEN_MS = 1500;

static constexpr int CPU_HIGH_THRESHOLD = 80;
static constexpr int CPU_SLEEPY_THRESHOLD = 10;
static constexpr float TEMP_HOT_THRESHOLD = 70.0f;

static constexpr uint8_t BACKLIGHT_BRIGHTNESS = 255;

}  // namespace AppConfig
