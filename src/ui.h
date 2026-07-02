#pragma once

#include <Arduino.h>
#include "pet_state.h"
#include "system_status.h"

enum class ScreenView {
  Pet,
  System,
  Network,
  Docker
};

class UI {
 public:
  bool begin();
  void showSplash();
  void showMessage(const String& title, const String& line1, const String& line2 = "");
  void render(const SystemStatus& status, const PetState& petState, bool wifiConnected);
  void updateInteraction();

 private:
  bool initTouch();
  bool readTouch(int16_t& x, int16_t& y);
  void nextView();
  void drawFrame(uint16_t accentColor, const String& header);
  void drawPetFace(const PetState& petState);
  void drawPetView(const SystemStatus& status, const PetState& petState, bool wifiConnected);
  void drawSystemView(const SystemStatus& status, const PetState& petState);
  void drawNetworkView(const SystemStatus& status, const PetState& petState, bool wifiConnected);
  void drawDockerView(const SystemStatus& status, const PetState& petState);
  void drawMetricLine(int16_t y, const String& label, const String& value, uint16_t color);
  void centerText(const String& text, int16_t y, uint8_t scale, uint16_t color);
  bool readRegister8(uint8_t address, uint8_t reg, uint8_t& value);

  ScreenView currentView_ = ScreenView::Pet;
  bool touchReady_ = false;
  uint32_t lastViewSwitchMs_ = 0;
  uint32_t lastTouchMs_ = 0;
};
