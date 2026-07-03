#include <Arduino.h>

#include "config.h"
#include "pet_state.h"
#include "status_client.h"
#include "ui.h"
#include "weather_client.h"
#include "wifi_manager.h"

namespace {

WiFiManager wifiManager;
StatusClient statusClient;
WeatherClient weatherClient;
UI ui;
SystemStatus systemStatus;
WeatherStatus weatherStatus;

uint32_t lastFetchMs = 0;
uint32_t lastWeatherFetchMs = 0;
bool endpointHealthy = false;
bool weatherHealthy = false;

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

  const uint32_t now = millis();
  if (lastFetchMs == 0 || now - lastFetchMs >= AppConfig::HTTP_REFRESH_MS) {
    lastFetchMs = now;
    refreshStatus();
  }

  if (lastWeatherFetchMs == 0 || now - lastWeatherFetchMs >= AppConfig::WEATHER_REFRESH_MS) {
    lastWeatherFetchMs = now;
    refreshWeather();
  }

  PetState petState = evaluatePetState(systemStatus, wifiManager.isConnected(), endpointHealthy);

  if (!wifiManager.isConnected()) {
    ui.showMessage("DaemonPet", "Conectando WiFi", wifiManager.localIP());
  } else {
    ui.render(systemStatus, weatherStatus, petState, wifiManager.isConnected());
  }

  delay(50);
}
