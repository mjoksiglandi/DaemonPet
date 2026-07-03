#include "wifi_manager.h"

#include <WiFi.h>
#include "config.h"

void WiFiManager::begin() {
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  lastAttemptMs_ = 0;
}

void WiFiManager::update() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  const uint32_t now = millis();
  if (now - lastAttemptMs_ < AppConfig::WIFI_RETRY_MS && lastAttemptMs_ != 0) {
    return;
  }

  lastAttemptMs_ = now;
  WiFi.disconnect(true, true);
  delay(100);
  WiFi.begin(AppConfig::WIFI_SSID, AppConfig::WIFI_PASSWORD);
}

bool WiFiManager::isConnected() const {
  return WiFi.status() == WL_CONNECTED;
}

String WiFiManager::localIP() const {
  return isConnected() ? WiFi.localIP().toString() : String("-");
}
