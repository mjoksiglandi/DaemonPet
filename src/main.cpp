#include <Arduino.h>

#include "config.h"
#include "pet_state.h"
#include "status_client.h"
#include "ui.h"
#include "wifi_manager.h"

namespace {

WiFiManager wifiManager;
StatusClient statusClient;
UI ui;
SystemStatus systemStatus;

uint32_t lastFetchMs = 0;
bool endpointHealthy = false;

void printMemoryInfo() {
  Serial.printf("Flash: %u MB\n", ESP.getFlashChipSize() / (1024 * 1024));
  Serial.printf("PSRAM: %u bytes\n", ESP.getPsramSize());
}

void refreshStatus() {
  if (!wifiManager.isConnected()) {
    endpointHealthy = false;
    systemStatus.valid = false;
    systemStatus.lastError = "WiFi offline";
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

  PetState petState = evaluatePetState(systemStatus, wifiManager.isConnected(), endpointHealthy);

  if (!wifiManager.isConnected()) {
    ui.showMessage("DaemonPet", "Conectando WiFi", wifiManager.localIP());
  } else if (!endpointHealthy) {
    ui.showMessage("DaemonPet", "Sin endpoint", systemStatus.lastError);
  } else {
    ui.render(systemStatus, petState, wifiManager.isConnected());
  }

  delay(50);
}
