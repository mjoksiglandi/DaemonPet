#include "ui.h"

#include <Arduino_GFX_Library.h>
#include <Wire.h>
#include "config.h"
#include "display_config.h"

namespace {

Arduino_DataBus* g_bus = new Arduino_ESP32LCD8(
    DisplayConfig::PIN_LCD_DC,
    DisplayConfig::PIN_LCD_CS,
    DisplayConfig::PIN_LCD_WR,
    -1,
    DisplayConfig::PIN_LCD_D0,
    DisplayConfig::PIN_LCD_D1,
    DisplayConfig::PIN_LCD_D2,
    DisplayConfig::PIN_LCD_D3,
    DisplayConfig::PIN_LCD_D4,
    DisplayConfig::PIN_LCD_D5,
    DisplayConfig::PIN_LCD_D6,
    DisplayConfig::PIN_LCD_D7);

Arduino_GFX* g_gfx = new Arduino_GC9A01(g_bus, DisplayConfig::PIN_LCD_RST, 0, true);

constexpr uint16_t COLOR_BLACK = RGB565_BLACK;
constexpr uint16_t COLOR_WHITE = RGB565_WHITE;
constexpr uint16_t COLOR_RED = RGB565_RED;
constexpr uint16_t COLOR_BLUE = RGB565_BLUE;
constexpr uint16_t COLOR_CYAN = RGB565_CYAN;
constexpr uint16_t COLOR_GREEN = RGB565_GREEN;
constexpr uint16_t COLOR_YELLOW = RGB565_YELLOW;
constexpr uint16_t COLOR_ORANGE = RGB565_ORANGE;
constexpr uint16_t COLOR_LIGHTGREY = RGB565_LIGHTGREY;
constexpr uint16_t COLOR_DARKGREY = RGB565_DARKGREY;

uint16_t rgb(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

}  // namespace

bool UI::begin() {
  Wire.begin(DisplayConfig::PIN_I2C_SDA, DisplayConfig::PIN_I2C_SCL);

  pinMode(DisplayConfig::PIN_LCD_BL, OUTPUT);
  ledcAttach(DisplayConfig::PIN_LCD_BL, DisplayConfig::BACKLIGHT_FREQ,
             DisplayConfig::BACKLIGHT_RESOLUTION_BITS);
  ledcWrite(DisplayConfig::PIN_LCD_BL, AppConfig::BACKLIGHT_BRIGHTNESS);

  if (!g_gfx->begin()) {
    return false;
  }

  g_gfx->fillScreen(COLOR_BLACK);
  g_gfx->setTextWrap(false);
  g_gfx->setTextColor(COLOR_WHITE);

  touchReady_ = initTouch();
  lastViewSwitchMs_ = millis();
  return true;
}

void UI::showSplash() {
  g_gfx->fillScreen(COLOR_BLACK);
  g_gfx->drawCircle(120, 90, 46, COLOR_CYAN);
  g_gfx->fillCircle(104, 82, 7, COLOR_WHITE);
  g_gfx->fillCircle(136, 82, 7, COLOR_WHITE);
  g_gfx->drawFastHLine(103, 115, 34, COLOR_CYAN);
  centerText("DaemonPet", 150, 2, COLOR_WHITE);
  centerText("Booting...", 180, 1, COLOR_LIGHTGREY);
}

void UI::showMessage(const String& title, const String& line1, const String& line2) {
  g_gfx->fillScreen(COLOR_BLACK);
  drawFrame(rgb(200, 70, 70), title);
  centerText(line1, 96, 2, COLOR_WHITE);
  if (line2.length() > 0) {
    centerText(line2, 132, 1, COLOR_LIGHTGREY);
  }
}

void UI::render(const SystemStatus& status, const PetState& petState, bool wifiConnected) {
  switch (currentView_) {
    case ScreenView::Pet:
      drawPetView(status, petState, wifiConnected);
      break;
    case ScreenView::System:
      drawSystemView(status, petState);
      break;
    case ScreenView::Network:
      drawNetworkView(status, petState, wifiConnected);
      break;
    case ScreenView::Docker:
      drawDockerView(status, petState);
      break;
  }
}

void UI::updateInteraction() {
  const uint32_t now = millis();

  if (touchReady_) {
    int16_t x = 0;
    int16_t y = 0;
    if (readTouch(x, y) && now - lastTouchMs_ > 350) {
      lastTouchMs_ = now;
      nextView();
      return;
    }
  }

  if (now - lastViewSwitchMs_ >= AppConfig::VIEW_ROTATION_MS) {
    nextView();
  }
}

bool UI::initTouch() {
  pinMode(DisplayConfig::PIN_TOUCH_RST, OUTPUT);
  digitalWrite(DisplayConfig::PIN_TOUCH_RST, LOW);
  delay(20);
  digitalWrite(DisplayConfig::PIN_TOUCH_RST, HIGH);
  delay(50);

  Wire.beginTransmission(DisplayConfig::TCA6408_ADDRESS);
  Wire.write(DisplayConfig::TCA6408_CONFIGURATION);
  Wire.write(0xFF);
  if (Wire.endTransmission() != 0) {
    return false;
  }

  uint8_t chipId = 0;
  return readRegister8(DisplayConfig::TOUCH_I2C_ADDRESS, DisplayConfig::TOUCH_CHIP_ID_REGISTER,
                       chipId);
}

bool UI::readTouch(int16_t& x, int16_t& y) {
  uint8_t tcaState = 0xFF;
  if (!readRegister8(DisplayConfig::TCA6408_ADDRESS, DisplayConfig::TCA6408_INPUT_PORT, tcaState)) {
    return false;
  }

  if ((tcaState & 0x01) != 0x00) {
    return false;
  }

  uint8_t xh = 0;
  uint8_t xl = 0;
  uint8_t yh = 0;
  uint8_t yl = 0;

  if (!readRegister8(DisplayConfig::TOUCH_I2C_ADDRESS, DisplayConfig::TOUCH_REGISTER_XH, xh) ||
      !readRegister8(DisplayConfig::TOUCH_I2C_ADDRESS, DisplayConfig::TOUCH_REGISTER_XL, xl) ||
      !readRegister8(DisplayConfig::TOUCH_I2C_ADDRESS, DisplayConfig::TOUCH_REGISTER_YH, yh) ||
      !readRegister8(DisplayConfig::TOUCH_I2C_ADDRESS, DisplayConfig::TOUCH_REGISTER_YL, yl)) {
    return false;
  }

  x = 0xFF - (((xh << 8) | xl) & 0x0FFF);
  y = ((yh << 8) | yl) & 0x0FFF;
  return true;
}

void UI::nextView() {
  switch (currentView_) {
    case ScreenView::Pet:
      currentView_ = ScreenView::System;
      break;
    case ScreenView::System:
      currentView_ = ScreenView::Network;
      break;
    case ScreenView::Network:
      currentView_ = ScreenView::Docker;
      break;
    case ScreenView::Docker:
      currentView_ = ScreenView::Pet;
      break;
  }
  lastViewSwitchMs_ = millis();
}

void UI::drawFrame(uint16_t accentColor, const String& header) {
  g_gfx->fillScreen(COLOR_BLACK);
  g_gfx->drawCircle(120, 120, 118, COLOR_DARKGREY);
  g_gfx->drawCircle(120, 120, 116, accentColor);
  centerText(header, 12, 1, accentColor);
}

void UI::drawPetFace(const PetState& petState) {
  const uint16_t face = rgb(30, 30, 30);
  g_gfx->fillCircle(120, 95, 48, face);
  g_gfx->drawCircle(120, 95, 48, petState.primaryColor);

  switch (petState.mood) {
    case PetMood::Disconnected:
      g_gfx->drawLine(102, 78, 114, 90, COLOR_WHITE);
      g_gfx->drawLine(114, 78, 102, 90, COLOR_WHITE);
      g_gfx->drawLine(126, 78, 138, 90, COLOR_WHITE);
      g_gfx->drawLine(138, 78, 126, 90, COLOR_WHITE);
      g_gfx->drawFastHLine(103, 116, 34, COLOR_WHITE);
      break;
    case PetMood::Overheated:
      g_gfx->fillCircle(105, 84, 7, COLOR_WHITE);
      g_gfx->fillCircle(135, 84, 7, COLOR_WHITE);
      g_gfx->fillCircle(105, 88, 3, COLOR_RED);
      g_gfx->fillCircle(135, 88, 3, COLOR_RED);
      g_gfx->drawFastHLine(105, 118, 30, COLOR_WHITE);
      g_gfx->fillCircle(120, 128, 4, COLOR_RED);
      break;
    case PetMood::Alert:
      g_gfx->drawFastHLine(98, 80, 16, COLOR_WHITE);
      g_gfx->drawFastHLine(126, 80, 16, COLOR_WHITE);
      g_gfx->fillCircle(106, 88, 5, COLOR_WHITE);
      g_gfx->fillCircle(134, 88, 5, COLOR_WHITE);
      g_gfx->drawFastHLine(105, 118, 30, COLOR_WHITE);
      break;
    case PetMood::Nervous:
      g_gfx->fillCircle(104, 84, 8, COLOR_WHITE);
      g_gfx->fillCircle(136, 84, 8, COLOR_WHITE);
      g_gfx->fillCircle(107, 84, 2, COLOR_BLACK);
      g_gfx->fillCircle(139, 84, 2, COLOR_BLACK);
      g_gfx->drawFastHLine(106, 118, 28, COLOR_WHITE);
      break;
    case PetMood::Sleepy:
      g_gfx->drawFastHLine(98, 85, 16, COLOR_WHITE);
      g_gfx->drawFastHLine(126, 85, 16, COLOR_WHITE);
      g_gfx->drawCircle(120, 116, 16, COLOR_WHITE);
      g_gfx->fillRect(103, 116, 34, 16, face);
      break;
    case PetMood::Normal:
    default:
      g_gfx->fillCircle(104, 84, 7, COLOR_WHITE);
      g_gfx->fillCircle(136, 84, 7, COLOR_WHITE);
      g_gfx->drawCircle(120, 108, 18, COLOR_WHITE);
      g_gfx->fillRect(101, 91, 38, 18, face);
      break;
  }
}

void UI::drawPetView(const SystemStatus& status, const PetState& petState, bool wifiConnected) {
  drawFrame(petState.primaryColor, "Mascota");
  drawPetFace(petState);
  centerText(petState.label, 154, 2, petState.primaryColor);
  centerText(petState.detail, 182, 1, COLOR_WHITE);
  drawMetricLine(202, "Host", status.host.length() ? status.host : "-", COLOR_WHITE);
  drawMetricLine(220, wifiConnected ? "CPU" : "WiFi", wifiConnected ? String(status.cpu) + "%" : "offline",
                 COLOR_LIGHTGREY);
}

void UI::drawSystemView(const SystemStatus& status, const PetState& petState) {
  drawFrame(petState.primaryColor, "Sistema");
  drawMetricLine(54, "CPU", String(status.cpu) + "%", COLOR_WHITE);
  drawMetricLine(84, "Temp", String(status.temp, 1) + " C", COLOR_WHITE);
  drawMetricLine(114, "RAM", String(status.ramUsed, 1) + "/" + String(status.ramTotal, 1) + " GB",
                 COLOR_WHITE);
  drawMetricLine(144, "Disco", String(status.disk) + "%", COLOR_WHITE);
  centerText(petState.label, 192, 2, petState.primaryColor);
}

void UI::drawNetworkView(const SystemStatus& status, const PetState& petState, bool wifiConnected) {
  drawFrame(petState.primaryColor, "Red");
  drawMetricLine(54, "Host", status.host, COLOR_WHITE);
  drawMetricLine(84, "IP", wifiConnected ? status.ip : "-", COLOR_WHITE);
  drawMetricLine(114, "RX", status.rx, COLOR_WHITE);
  drawMetricLine(144, "TX", status.tx, COLOR_WHITE);
  drawMetricLine(174, "Uptime", status.uptime, COLOR_WHITE);
}

void UI::drawDockerView(const SystemStatus& status, const PetState& petState) {
  drawFrame(petState.primaryColor, "Docker");
  centerText(String(status.dockerUp), 68, 4, COLOR_GREEN);
  centerText("Activos", 108, 1, COLOR_WHITE);
  centerText(String(status.dockerExit), 148, 4, status.dockerExit > 0 ? COLOR_RED : COLOR_LIGHTGREY);
  centerText("Caidos", 188, 1, COLOR_WHITE);
}

void UI::drawMetricLine(int16_t y, const String& label, const String& value, uint16_t color) {
  g_gfx->setTextSize(1);
  g_gfx->setTextColor(COLOR_LIGHTGREY);
  g_gfx->setCursor(30, y);
  g_gfx->print(label);

  g_gfx->setTextColor(color);
  g_gfx->setCursor(110, y);
  g_gfx->print(value);
}

void UI::centerText(const String& text, int16_t y, uint8_t scale, uint16_t color) {
  int16_t x1 = 0;
  int16_t y1 = 0;
  uint16_t w = 0;
  uint16_t h = 0;
  g_gfx->setTextSize(scale);
  g_gfx->getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  const int16_t x = (DisplayConfig::SCREEN_WIDTH - static_cast<int16_t>(w)) / 2;
  g_gfx->setCursor(x, y);
  g_gfx->setTextColor(color);
  g_gfx->print(text);
}

bool UI::readRegister8(uint8_t address, uint8_t reg, uint8_t& value) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) {
    return false;
  }
  if (Wire.requestFrom(static_cast<int>(address), 1, true) != 1) {
    return false;
  }
  value = Wire.read();
  return true;
}
