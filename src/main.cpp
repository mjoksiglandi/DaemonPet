#include <Arduino.h>
#include <stdlib.h>
#include <time.h>

#include "config.h"
#include "pet_state.h"
#include "rtc_clock.h"
#include "status_client.h"
#include "ui.h"
#include "weather_client.h"
#include "wifi_manager.h"

namespace {

WiFiManager wifiManager;
RTCClock rtcClock;
StatusClient statusClient;
WeatherClient weatherClient;
UI ui;
SystemStatus systemStatus;
WeatherStatus weatherStatus;

uint32_t lastFetchMs = 0;
uint32_t lastWeatherFetchMs = 0;
uint32_t lastRtcSyncMs = 0;
uint32_t lastRtcAttemptMs = 0;
bool endpointHealthy = false;
bool weatherHealthy = false;
bool lastWifiConnected = false;

void syncRtcFromNtpIfNeeded(bool wifiConnected) {
  if (!wifiConnected) {
    return;
  }

  const uint32_t now = millis();
  const bool firstSync = lastRtcSyncMs == 0;
  const bool intervalElapsed = !firstSync && now - lastRtcSyncMs >= AppConfig::RTC_NTP_RESYNC_MS;
  const bool wifiJustConnected = wifiConnected && !lastWifiConnected;
  const bool retryWindowElapsed = lastRtcAttemptMs == 0 || now - lastRtcAttemptMs >= AppConfig::WIFI_RETRY_MS;

  if ((!firstSync && !intervalElapsed && !wifiJustConnected) || !retryWindowElapsed) {
    return;
  }

  lastRtcAttemptMs = now;
  if (!rtcClock.syncFromNtp(AppConfig::DEVICE_TIMEZONE,
                            AppConfig::NTP_SERVER_1,
                            AppConfig::NTP_SERVER_2,
                            AppConfig::NTP_SYNC_TIMEOUT_MS)) {
    Serial.println("RTC sync failed");
    return;
  }

  lastRtcSyncMs = now;
  Serial.println("RTC synced from NTP");
}

void printMemoryInfo() {
  Serial.printf("Flash: %u MB\n", ESP.getFlashChipSize() / (1024 * 1024));
  Serial.printf("PSRAM: %u bytes\n", ESP.getPsramSize());
}

void refreshStatus() {
  if (!wifiManager.isConnected()) {
    endpointHealthy = false;
    systemStatus.valid = false;
    systemStatus.lastError = "WiFi offline";
    weatherHealthy = false;
    weatherStatus.valid = false;
    weatherStatus.lastError = "WiFi offline";
    return;
  }

  String errorMessage;
  endpointHealthy = statusClient.fetch(systemStatus, errorMessage);
  if (!endpointHealthy) {
    systemStatus.valid = false;
    systemStatus.lastError = errorMessage;
    Serial.printf("Status fetch failed: %s\n", errorMessage.c_str());
  }
}

void refreshWeather() {
  if (!wifiManager.isConnected()) {
    weatherHealthy = false;
    weatherStatus.valid = false;
    weatherStatus.lastError = "WiFi offline";
    return;
  }

  String errorMessage;
  weatherHealthy = weatherClient.fetch(weatherStatus, errorMessage);
  if (!weatherHealthy) {
    weatherStatus.valid = false;
    weatherStatus.lastError = errorMessage;
    Serial.printf("Weather fetch failed: %s\n", errorMessage.c_str());
  }
}

}  // namespace

void setup() {
  Serial.begin(115200);
  delay(300);
  printMemoryInfo();
  setenv("TZ", AppConfig::DEVICE_TIMEZONE, 1);
  tzset();

  if (rtcClock.begin()) {
    if (rtcClock.syncSystemTime()) {
      Serial.println("System time restored from RTC");
    } else {
      Serial.println("RTC available but time invalid");
    }
  } else {
    Serial.println("RTC init failed");
  }

  wifiManager.begin();

  if (!ui.begin()) {
    Serial.println("Display init failed");
    return;
  }

  ui.showSplash();
  delay(AppConfig::SPLASH_SCREEN_MS);
}

void loop() {
  wifiManager.update();
  ui.updateInteraction();
  const bool wifiConnected = wifiManager.isConnected();

  syncRtcFromNtpIfNeeded(wifiConnected);

  const uint32_t now = millis();
  if (lastFetchMs == 0 || now - lastFetchMs >= AppConfig::HTTP_REFRESH_MS) {
    lastFetchMs = now;
    refreshStatus();
  }

  if (lastWeatherFetchMs == 0 || now - lastWeatherFetchMs >= AppConfig::WEATHER_REFRESH_MS) {
    lastWeatherFetchMs = now;
    refreshWeather();
  }

  PetState petState = evaluatePetState(systemStatus, wifiConnected, endpointHealthy);
  ui.render(systemStatus, weatherStatus, petState, wifiConnected);
  lastWifiConnected = wifiConnected;

  delay(50);
}
