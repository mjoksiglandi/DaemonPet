#pragma once

#include <Arduino.h>
#include "pet_state.h"
#include "system_status.h"
#include "weather_status.h"

enum class ScreenView {
  Pet,
  Clock,
  System,
  Network,
  Docker,
  Weather
};

class UI {
 public:
  bool begin();
  void showSplash();
  void showMessage(const String& title, const String& line1, const String& line2 = "");
  void render(const SystemStatus& status,
              const WeatherStatus& weather,
              const PetState& petState,
              bool wifiConnected);
  void updateInteraction();

 private:
  bool initTouch();
  bool readTouch(int16_t& x, int16_t& y);
  void nextView();
  void drawFrame(uint16_t accentColor, const String& header);
  void drawAmbientBackground(uint16_t accentColor, bool showHeader, const String& header = "");
  void drawTechShell(uint16_t accentColor);
  void drawListMockBackground(uint16_t accentColor, int rowCount);
  void drawDockerMockBackground(uint16_t accentColor);
  void drawViewDots(ScreenView activeView, uint16_t activeColor);
  void drawRoundedPanel(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t borderColor);
  void drawIconDisc(int16_t cx, int16_t cy, uint16_t accentColor);
  void drawGlyphCpu(int16_t cx, int16_t cy);
  void drawGlyphTemperature(int16_t cx, int16_t cy);
  void drawGlyphRam(int16_t cx, int16_t cy);
  void drawGlyphDisk(int16_t cx, int16_t cy);
  void drawGlyphHost(int16_t cx, int16_t cy);
  void drawGlyphGlobe(int16_t cx, int16_t cy);
  void drawGlyphDownload(int16_t cx, int16_t cy);
  void drawGlyphUpload(int16_t cx, int16_t cy);
  void drawGlyphClock(int16_t cx, int16_t cy);
  void drawSystemRow(int16_t y, const String& value, uint16_t valueColor, uint8_t iconType);
  void drawNetworkRow(int16_t y, const String& value, uint16_t valueColor, uint8_t iconType);
  String clipText(const String& text, size_t maxChars);
  void drawTextLeftInBox(const String& text,
                         int16_t x,
                         int16_t y,
                         int16_t w,
                         int16_t h,
                         uint8_t scale,
                         uint16_t color,
                         int16_t padX = 0,
                         int16_t offsetY = 0);
  void drawTextCenteredInBox(const String& text,
                             int16_t x,
                             int16_t y,
                             int16_t w,
                             int16_t h,
                             uint8_t scale,
                             uint16_t color,
                             int16_t offsetX = 0,
                             int16_t offsetY = 0);
  void drawTextRightInBox(const String& text,
                          int16_t x,
                          int16_t y,
                          int16_t w,
                          int16_t h,
                          uint8_t scale,
                          uint16_t color,
                          int16_t padX = 0,
                          int16_t offsetY = 0);
  void drawWeatherIcon(int16_t cx, int16_t cy, int weatherCode, bool large = false);
  void drawSunIcon(int16_t cx, int16_t cy, uint16_t color, bool large = false);
  void drawMoonIcon(int16_t cx, int16_t cy, bool large = false);
  void drawCloudIcon(int16_t cx, int16_t cy, uint16_t color, bool large = false);
  void drawRainIcon(int16_t cx, int16_t cy);
  void drawWindIcon(int16_t cx, int16_t cy, bool compact = false);
  void drawSnowIcon(int16_t cx, int16_t cy);
  void drawTornadoIcon(int16_t cx, int16_t cy);
  void drawStormIcon(int16_t cx, int16_t cy);
  void drawPetFace(const PetState& petState);
  void drawPetView(const SystemStatus& status, const PetState& petState, bool wifiConnected);
  void drawClockView();
  void drawSystemView(const SystemStatus& status, const PetState& petState);
  void drawNetworkView(const SystemStatus& status, const PetState& petState, bool wifiConnected);
  void drawDockerView(const SystemStatus& status, const PetState& petState);
  void drawWeatherView(const WeatherStatus& weather, const PetState& petState, bool wifiConnected);
  void drawMetricLine(int16_t y, const String& label, const String& value, uint16_t color);
  void centerTextInBox(const String& text,
                       int16_t boxX,
                       int16_t boxWidth,
                       int16_t y,
                       uint8_t scale,
                       uint16_t color);
  String shortDayLabel(const String& isoDate);
  String rtcWeekdayLabel(uint8_t weekday);
  String rtcMonthLabel(uint8_t month);
  String weatherLabel(int weatherCode);
  void centerText(const String& text, int16_t y, uint8_t scale, uint16_t color);
  bool readRegister8(uint8_t address, uint8_t reg, uint8_t& value);

  ScreenView currentView_ = ScreenView::Pet;
  bool touchReady_ = false;
  uint32_t lastViewSwitchMs_ = 0;
  uint32_t lastTouchMs_ = 0;
};
