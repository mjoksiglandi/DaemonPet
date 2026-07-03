#include "ui.h"

#include <math.h>
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
constexpr uint16_t COLOR_SKY = 0x6D7F;
constexpr uint16_t COLOR_PANEL = 0x10A2;
constexpr uint16_t COLOR_PANEL_ALT = 0x18E3;
constexpr uint16_t COLOR_PURPLE = 0x49B0;
constexpr uint16_t COLOR_PURPLE_DARK = 0x30CC;
constexpr uint16_t COLOR_LAVENDER = 0xB63F;
constexpr uint16_t COLOR_MINT = 0xA77A;
constexpr uint16_t COLOR_SOFT_WHITE = 0xEF7D;
constexpr uint16_t COLOR_DEEP_PANEL = 0x08C2;
constexpr uint16_t COLOR_STROKE = 0x31A8;
constexpr uint16_t COLOR_DOT = 0x18A5;
constexpr uint16_t COLOR_VIOLET_GLOW = 0x7B92;

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

void UI::render(const SystemStatus& status,
                const WeatherStatus& weather,
                const PetState& petState,
                bool wifiConnected) {
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
    case ScreenView::Weather:
      drawWeatherView(weather, petState, wifiConnected);
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
      currentView_ = ScreenView::Weather;
      break;
    case ScreenView::Weather:
      currentView_ = ScreenView::Pet;
      break;
  }
  lastViewSwitchMs_ = millis();
}

void UI::drawFrame(uint16_t accentColor, const String& header) {
  drawAmbientBackground(accentColor, true, header);
}

void UI::drawAmbientBackground(uint16_t accentColor, bool showHeader, const String& header) {
  g_gfx->fillScreen(COLOR_BLACK);
  g_gfx->fillCircle(120, 120, 114, rgb(18, 16, 44));
  g_gfx->fillCircle(120, 120, 100, rgb(24, 22, 58));
  g_gfx->fillCircle(68, 58, 26, rgb(48, 38, 96));
  g_gfx->fillCircle(176, 170, 28, rgb(32, 92, 132));
  g_gfx->fillCircle(168, 52, 18, rgb(68, 52, 128));
  g_gfx->drawCircle(120, 120, 118, COLOR_DARKGREY);
  g_gfx->drawCircle(120, 120, 116, accentColor);
  if (showHeader) {
    g_gfx->fillRoundRect(68, 8, 104, 22, 10, rgb(28, 24, 66));
    g_gfx->drawRoundRect(68, 8, 104, 22, 10, accentColor);
    centerText(header, 12, 1, accentColor);
  }
}

void UI::drawTechShell(uint16_t accentColor) {
  g_gfx->fillScreen(COLOR_BLACK);
  g_gfx->fillCircle(120, 120, 116, rgb(7, 10, 18));
  g_gfx->fillCircle(120, 120, 108, rgb(10, 15, 28));
  g_gfx->fillCircle(120, 120, 96, rgb(11, 18, 34));
  g_gfx->drawCircle(120, 120, 118, rgb(26, 38, 62));
  g_gfx->drawCircle(120, 120, 114, accentColor);
  g_gfx->drawCircle(120, 120, 106, COLOR_STROKE);
  g_gfx->fillCircle(120, 12, 4, COLOR_CYAN);
  g_gfx->fillCircle(120, 228, 4, COLOR_CYAN);
  g_gfx->fillCircle(16, 120, 4, COLOR_LAVENDER);
  g_gfx->fillCircle(224, 120, 4, COLOR_LAVENDER);
  g_gfx->fillCircle(42, 72, 3, COLOR_PURPLE);
  g_gfx->fillCircle(192, 154, 3, COLOR_PURPLE);
  g_gfx->fillCircle(34, 152, 3, COLOR_SKY);
  g_gfx->fillCircle(200, 86, 2, COLOR_CYAN);

  for (int i = 0; i < 7; ++i) {
    g_gfx->fillCircle(28 + i * 4, 98 + (i % 2) * 4, 1, COLOR_DOT);
    g_gfx->fillCircle(212 - i * 4, 140 - (i % 2) * 4, 1, COLOR_DOT);
  }
}

void UI::drawListMockBackground(uint16_t accentColor, int rowCount) {
  drawTechShell(accentColor);

  const int16_t startY = rowCount == 4 ? 38 : 26;
  const int16_t rowHeight = rowCount == 4 ? 33 : 27;
  const int16_t gap = 7;

  for (int i = 0; i < rowCount; ++i) {
    const int16_t y = startY + i * (rowHeight + gap);
    g_gfx->fillRoundRect(56, y, 126, rowHeight, 16, COLOR_PANEL);
    g_gfx->drawRoundRect(56, y, 126, rowHeight, 16, COLOR_STROKE);
    g_gfx->drawRoundRect(58, y + 2, 122, rowHeight - 4, 14, rgb(58, 73, 98));
    g_gfx->fillCircle(73, y + rowHeight / 2, rowCount == 4 ? 18 : 16, COLOR_DEEP_PANEL);
    g_gfx->drawCircle(73, y + rowHeight / 2, rowCount == 4 ? 18 : 16, accentColor);
    g_gfx->drawCircle(73, y + rowHeight / 2, rowCount == 4 ? 20 : 18, rgb(42, 49, 78));
    g_gfx->drawFastHLine(92, y + rowHeight - 7, 66, rgb(72, 88, 118));
    g_gfx->fillCircle(160, y + rowHeight - 7, 1, COLOR_CYAN);
    g_gfx->fillCircle(167, y + rowHeight - 7, 1, COLOR_LIGHTGREY);
    g_gfx->fillCircle(171, y + rowHeight - 7, 1, COLOR_LIGHTGREY);

    for (int8_t dy = 0; dy < 3; ++dy) {
      for (int8_t dx = 0; dx < 3; ++dx) {
        g_gfx->fillCircle(155 + dx * 4, y + 8 + dy * 4, 1, rgb(28, 38, 56));
      }
    }
  }
}

void UI::drawDockerMockBackground(uint16_t accentColor) {
  drawTechShell(accentColor);

  g_gfx->drawCircle(120, 84, 58, COLOR_VIOLET_GLOW);
  g_gfx->drawCircle(120, 84, 54, accentColor);
  g_gfx->drawCircle(120, 84, 44, COLOR_CYAN);
  g_gfx->fillCircle(120, 84, 40, rgb(9, 14, 25));

  g_gfx->fillCircle(120, 176, 25, COLOR_DEEP_PANEL);
  g_gfx->drawCircle(120, 176, 25, COLOR_STROKE);
  g_gfx->drawCircle(120, 176, 21, accentColor);

  g_gfx->drawCircle(56, 182, 22, rgb(44, 42, 84));
  g_gfx->drawCircle(184, 182, 22, rgb(44, 42, 84));
  g_gfx->fillRoundRect(38, 180, 36, 12, 6, rgb(28, 24, 58));
  g_gfx->fillRoundRect(166, 180, 36, 12, 6, rgb(28, 24, 58));
  g_gfx->drawRoundRect(42, 156, 28, 24, 4, COLOR_PURPLE);
  g_gfx->drawRoundRect(170, 156, 28, 24, 4, COLOR_PURPLE);
  g_gfx->drawFastVLine(49, 160, 16, COLOR_PURPLE_DARK);
  g_gfx->drawFastVLine(56, 160, 16, COLOR_PURPLE_DARK);
  g_gfx->drawFastVLine(63, 160, 16, COLOR_PURPLE_DARK);
  g_gfx->drawFastVLine(177, 160, 16, COLOR_PURPLE_DARK);
  g_gfx->drawFastVLine(184, 160, 16, COLOR_PURPLE_DARK);
  g_gfx->drawFastVLine(191, 160, 16, COLOR_PURPLE_DARK);

  for (int i = 0; i < 5; ++i) {
    g_gfx->fillCircle(48 + i * 4, 142 - i * 5, 1, COLOR_LAVENDER);
    g_gfx->fillCircle(192 - i * 4, 142 - i * 5, 1, COLOR_LAVENDER);
  }
}

void UI::drawViewDots(ScreenView activeView, uint16_t activeColor) {
  const int totalViews = 5;
  const int startX = 94;
  const int y = 224;
  for (int i = 0; i < totalViews; ++i) {
    const int x = startX + (i * 13);
    const bool active = i == static_cast<int>(activeView);
    g_gfx->fillCircle(x, y, active ? 4 : 2, active ? activeColor : COLOR_DARKGREY);
  }
}

void UI::drawRoundedPanel(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t borderColor) {
  g_gfx->fillRoundRect(x, y, w, h, 14, COLOR_PANEL);
  g_gfx->drawRoundRect(x, y, w, h, 14, borderColor);
}

void UI::drawIconDisc(int16_t cx, int16_t cy, uint16_t accentColor) {
  g_gfx->fillCircle(cx, cy, 14, COLOR_DEEP_PANEL);
  g_gfx->drawCircle(cx, cy, 14, accentColor);
  g_gfx->drawCircle(cx, cy, 16, rgb(42, 49, 78));
}

void UI::drawGlyphCpu(int16_t cx, int16_t cy) {
  g_gfx->drawRoundRect(cx - 6, cy - 6, 12, 12, 2, COLOR_LAVENDER);
  for (int i = -4; i <= 4; i += 4) {
    g_gfx->drawFastVLine(cx + i, cy - 10, 3, COLOR_CYAN);
    g_gfx->drawFastVLine(cx + i, cy + 8, 3, COLOR_CYAN);
    g_gfx->drawFastHLine(cx - 10, cy + i, 3, COLOR_LAVENDER);
    g_gfx->drawFastHLine(cx + 8, cy + i, 3, COLOR_LAVENDER);
  }
}

void UI::drawGlyphTemperature(int16_t cx, int16_t cy) {
  g_gfx->drawCircle(cx, cy + 4, 4, COLOR_CYAN);
  g_gfx->drawFastVLine(cx, cy - 8, 12, COLOR_CYAN);
  g_gfx->drawRoundRect(cx - 3, cy - 10, 6, 18, 3, COLOR_CYAN);
  g_gfx->drawFastHLine(cx + 6, cy - 4, 3, COLOR_CYAN);
  g_gfx->drawFastHLine(cx + 6, cy, 3, COLOR_CYAN);
}

void UI::drawGlyphRam(int16_t cx, int16_t cy) {
  g_gfx->drawRoundRect(cx - 9, cy - 5, 18, 10, 2, COLOR_LAVENDER);
  for (int i = -6; i <= 6; i += 6) {
    g_gfx->drawFastVLine(cx + i, cy + 6, 3, COLOR_LAVENDER);
  }
  g_gfx->drawRect(cx - 6, cy - 2, 4, 4, COLOR_LAVENDER);
  g_gfx->drawRect(cx + 2, cy - 2, 4, 4, COLOR_LAVENDER);
}

void UI::drawGlyphDisk(int16_t cx, int16_t cy) {
  g_gfx->drawCircle(cx, cy, 9, COLOR_CYAN);
  g_gfx->drawCircle(cx, cy, 2, COLOR_CYAN);
  g_gfx->drawLine(cx, cy, cx + 6, cy + 4, COLOR_CYAN);
}

void UI::drawGlyphHost(int16_t cx, int16_t cy) {
  g_gfx->drawRoundRect(cx - 7, cy - 8, 14, 16, 2, COLOR_LAVENDER);
  g_gfx->drawFastHLine(cx - 4, cy - 2, 8, COLOR_LAVENDER);
  g_gfx->drawFastHLine(cx - 3, cy + 3, 6, COLOR_CYAN);
}

void UI::drawGlyphGlobe(int16_t cx, int16_t cy) {
  g_gfx->drawCircle(cx, cy, 9, COLOR_CYAN);
  g_gfx->drawFastVLine(cx, cy - 8, 16, COLOR_CYAN);
  g_gfx->drawFastHLine(cx - 7, cy, 14, COLOR_CYAN);
  g_gfx->drawCircle(cx, cy, 4, COLOR_CYAN);
}

void UI::drawGlyphDownload(int16_t cx, int16_t cy) {
  g_gfx->drawFastVLine(cx, cy - 8, 10, COLOR_CYAN);
  g_gfx->drawLine(cx - 5, cy - 1, cx, cy + 5, COLOR_CYAN);
  g_gfx->drawLine(cx + 5, cy - 1, cx, cy + 5, COLOR_CYAN);
  g_gfx->drawRoundRect(cx - 8, cy + 4, 16, 5, 2, COLOR_CYAN);
}

void UI::drawGlyphUpload(int16_t cx, int16_t cy) {
  g_gfx->drawFastVLine(cx, cy - 2, 10, COLOR_LAVENDER);
  g_gfx->drawLine(cx - 5, cy + 1, cx, cy - 5, COLOR_LAVENDER);
  g_gfx->drawLine(cx + 5, cy + 1, cx, cy - 5, COLOR_LAVENDER);
  g_gfx->drawRoundRect(cx - 8, cy + 4, 16, 5, 2, COLOR_LAVENDER);
}

void UI::drawGlyphClock(int16_t cx, int16_t cy) {
  g_gfx->drawCircle(cx, cy, 9, COLOR_CYAN);
  g_gfx->drawFastVLine(cx, cy - 5, 6, COLOR_CYAN);
  g_gfx->drawLine(cx, cy, cx + 4, cy + 3, COLOR_CYAN);
}

String UI::clipText(const String& text, size_t maxChars) {
  if (text.length() <= maxChars) {
    return text;
  }
  return text.substring(0, maxChars - 1) + ".";
}

void UI::drawSystemRow(int16_t y, const String& value, uint16_t valueColor, uint8_t iconType) {
  const int16_t cy = y + 16;
  drawIconDisc(73, cy, iconType % 2 == 0 ? COLOR_CYAN : COLOR_LAVENDER);

  switch (iconType) {
    case 0:
      drawGlyphCpu(73, cy);
      break;
    case 1:
      drawGlyphTemperature(73, cy);
      break;
    case 2:
      drawGlyphRam(73, cy);
      break;
    default:
      drawGlyphDisk(73, cy);
      break;
  }
}

void UI::drawNetworkRow(int16_t y, const String& value, uint16_t valueColor, uint8_t iconType) {
  const int16_t cy = y + 13;
  drawIconDisc(73, cy, iconType == 3 ? COLOR_LAVENDER : COLOR_CYAN);

  switch (iconType) {
    case 0:
      drawGlyphHost(73, cy);
      break;
    case 1:
      drawGlyphGlobe(73, cy);
      break;
    case 2:
      drawGlyphDownload(73, cy);
      break;
    case 3:
      drawGlyphUpload(73, cy);
      break;
    default:
      drawGlyphClock(73, cy);
      break;
  }
}

void UI::drawSunIcon(int16_t cx, int16_t cy, uint16_t color, bool large) {
  const int r = large ? 18 : 10;
  const int ray = large ? 30 : 18;
  for (int i = 0; i < 8; ++i) {
    const float angle = i * 0.785398f;
    const int x1 = cx + static_cast<int>((r + 3) * cos(angle));
    const int y1 = cy + static_cast<int>((r + 3) * sin(angle));
    const int x2 = cx + static_cast<int>(ray * cos(angle));
    const int y2 = cy + static_cast<int>(ray * sin(angle));
    g_gfx->drawLine(x1, y1, x2, y2, color);
  }
  g_gfx->fillCircle(cx, cy, r, color);
}

void UI::drawMoonIcon(int16_t cx, int16_t cy, bool large) {
  const int radius = large ? 16 : 10;
  g_gfx->fillCircle(cx, cy, radius, COLOR_LAVENDER);
  g_gfx->fillCircle(cx + (large ? 8 : 5), cy - (large ? 3 : 2), radius - 3, rgb(26, 22, 62));
}

void UI::drawCloudIcon(int16_t cx, int16_t cy, uint16_t color, bool large) {
  const int baseW = large ? 44 : 28;
  const int baseH = large ? 16 : 10;
  g_gfx->fillCircle(cx - (large ? 14 : 9), cy, large ? 12 : 8, color);
  g_gfx->fillCircle(cx, cy - (large ? 6 : 4), large ? 15 : 10, color);
  g_gfx->fillCircle(cx + (large ? 14 : 9), cy, large ? 11 : 7, color);
  g_gfx->fillRoundRect(cx - baseW / 2, cy, baseW, baseH, baseH / 2, color);
}

void UI::drawRainIcon(int16_t cx, int16_t cy) {
  for (int i = -10; i <= 10; i += 10) {
    g_gfx->drawLine(cx + i, cy + 12, cx + i - 4, cy + 20, COLOR_CYAN);
  }
}

void UI::drawWindIcon(int16_t cx, int16_t cy, bool compact) {
  const int width = compact ? 18 : 30;
  g_gfx->drawFastHLine(cx - width / 2, cy, width, COLOR_SOFT_WHITE);
  g_gfx->drawFastHLine(cx - width / 2 + 4, cy + 7, width - 8, COLOR_SOFT_WHITE);
  g_gfx->drawFastHLine(cx - width / 2 + 10, cy + 14, width - 18, COLOR_SOFT_WHITE);
  g_gfx->drawCircle(cx - width / 2 + 4, cy + 6, compact ? 3 : 5, COLOR_SOFT_WHITE);
}

void UI::drawSnowIcon(int16_t cx, int16_t cy) {
  for (int i = -10; i <= 10; i += 10) {
    const int x = cx + i;
    const int y = cy + 14;
    g_gfx->drawLine(x - 3, y, x + 3, y, COLOR_WHITE);
    g_gfx->drawLine(x, y - 3, x, y + 3, COLOR_WHITE);
    g_gfx->drawLine(x - 2, y - 2, x + 2, y + 2, COLOR_WHITE);
    g_gfx->drawLine(x - 2, y + 2, x + 2, y - 2, COLOR_WHITE);
  }
}

void UI::drawStormIcon(int16_t cx, int16_t cy) {
  const int points[] = {cx + 2, cy + 8, cx - 4, cy + 20, cx + 1, cy + 20,
                        cx - 2, cy + 30, cx + 8, cy + 16, cx + 2, cy + 16};
  g_gfx->fillTriangle(points[0], points[1], points[2], points[3], points[4], points[5], COLOR_YELLOW);
  g_gfx->fillTriangle(points[6], points[7], points[8], points[9], points[10], points[11], COLOR_YELLOW);
}

void UI::drawTornadoIcon(int16_t cx, int16_t cy) {
  g_gfx->drawCircle(cx, cy, 16, COLOR_SOFT_WHITE);
  g_gfx->drawCircle(cx + 2, cy + 10, 12, COLOR_SOFT_WHITE);
  g_gfx->drawCircle(cx + 4, cy + 18, 8, COLOR_SOFT_WHITE);
  g_gfx->drawCircle(cx + 5, cy + 24, 5, COLOR_SOFT_WHITE);
  g_gfx->drawFastHLine(cx - 18, cy - 1, 36, COLOR_SOFT_WHITE);
  g_gfx->drawFastHLine(cx - 12, cy + 9, 24, COLOR_SOFT_WHITE);
  g_gfx->drawFastHLine(cx - 7, cy + 17, 14, COLOR_SOFT_WHITE);
}

void UI::drawWeatherIcon(int16_t cx, int16_t cy, int weatherCode, bool large) {
  if (weatherCode == 0) {
    drawSunIcon(cx, cy, COLOR_YELLOW, large);
    return;
  }

  if (weatherCode == 1 || weatherCode == 2) {
    drawMoonIcon(cx + (large ? 18 : 10), cy - (large ? 16 : 10), false);
    drawSunIcon(cx - (large ? 12 : 8), cy - (large ? 8 : 5), COLOR_YELLOW, false);
    drawCloudIcon(cx + (large ? 4 : 2), cy + (large ? 2 : 0), COLOR_WHITE, large);
    return;
  }

  if (weatherCode == 3 || weatherCode == 45 || weatherCode == 48) {
    drawCloudIcon(cx, cy, weatherCode == 3 ? COLOR_LIGHTGREY : COLOR_DARKGREY, large);
    if (weatherCode == 45 || weatherCode == 48) {
      drawWindIcon(cx, cy + (large ? 18 : 10), !large);
    }
    return;
  }

  if ((weatherCode >= 51 && weatherCode <= 67) || (weatherCode >= 80 && weatherCode <= 82)) {
    drawMoonIcon(cx + (large ? 18 : 10), cy - (large ? 16 : 10), false);
    drawCloudIcon(cx, cy, COLOR_LIGHTGREY, large);
    drawRainIcon(cx, cy + (large ? 2 : 0));
    return;
  }

  if ((weatherCode >= 71 && weatherCode <= 77) || weatherCode == 85 || weatherCode == 86) {
    drawCloudIcon(cx, cy, COLOR_LIGHTGREY, large);
    drawSnowIcon(cx, cy);
    return;
  }

  if (weatherCode >= 95) {
    if (weatherCode == 95 || weatherCode == 96 || weatherCode == 99) {
      drawCloudIcon(cx, cy, COLOR_LIGHTGREY, large);
      drawStormIcon(cx, cy);
      return;
    }
    drawTornadoIcon(cx, cy);
    return;
  }

  drawCloudIcon(cx, cy, COLOR_LIGHTGREY, large);
}

void UI::drawPetFace(const PetState& petState) {
  const uint16_t shell = rgb(31, 32, 66);
  const uint16_t face = COLOR_SOFT_WHITE;
  const uint16_t cheek = rgb(255, 188, 196);
  const uint16_t belly = rgb(70, 210, 120);

  g_gfx->fillTriangle(58, 86, 90, 18, 116, 98, shell);
  g_gfx->fillTriangle(182, 86, 150, 18, 124, 98, shell);
  g_gfx->fillCircle(120, 112, 74, shell);
  g_gfx->fillCircle(120, 112, 52, face);
  g_gfx->fillCircle(120, 182, 28, shell);
  g_gfx->fillRoundRect(92, 176, 56, 26, 12, shell);
  g_gfx->fillRoundRect(90, 176, 60, 24, 12, belly);
  g_gfx->fillRoundRect(84, 198, 16, 18, 6, shell);
  g_gfx->fillRoundRect(140, 198, 16, 18, 6, shell);
  g_gfx->fillCircle(92, 150, 8, cheek);
  g_gfx->fillCircle(148, 150, 8, cheek);
  g_gfx->drawCircle(120, 112, 74, petState.primaryColor);
  g_gfx->drawCircle(120, 112, 76, rgb(88, 84, 140));

  switch (petState.mood) {
    case PetMood::Disconnected:
      g_gfx->drawLine(94, 96, 110, 112, shell);
      g_gfx->drawLine(110, 96, 94, 112, shell);
      g_gfx->drawLine(130, 96, 146, 112, shell);
      g_gfx->drawLine(146, 96, 130, 112, shell);
      g_gfx->drawFastHLine(104, 140, 32, shell);
      break;
    case PetMood::Overheated:
      g_gfx->fillCircle(100, 106, 8, shell);
      g_gfx->fillCircle(140, 106, 8, shell);
      g_gfx->fillCircle(100, 111, 4, COLOR_RED);
      g_gfx->fillCircle(140, 111, 4, COLOR_RED);
      g_gfx->drawFastHLine(108, 142, 24, shell);
      g_gfx->fillCircle(120, 150, 5, COLOR_RED);
      break;
    case PetMood::Alert:
      g_gfx->drawFastHLine(92, 102, 20, shell);
      g_gfx->drawFastHLine(128, 102, 20, shell);
      g_gfx->fillCircle(102, 112, 6, shell);
      g_gfx->fillCircle(138, 112, 6, shell);
      g_gfx->drawFastHLine(106, 142, 28, shell);
      break;
    case PetMood::Nervous:
      g_gfx->fillCircle(98, 108, 12, COLOR_WHITE);
      g_gfx->fillCircle(142, 108, 12, COLOR_WHITE);
      g_gfx->fillCircle(104, 110, 3, shell);
      g_gfx->fillCircle(146, 110, 3, shell);
      g_gfx->drawFastHLine(106, 142, 28, shell);
      break;
    case PetMood::Sleepy:
      g_gfx->drawFastHLine(92, 110, 22, shell);
      g_gfx->drawFastHLine(126, 110, 22, shell);
      g_gfx->drawCircle(120, 144, 14, shell);
      g_gfx->fillRect(106, 144, 28, 14, face);
      break;
    case PetMood::Normal:
    default:
      g_gfx->fillCircle(100, 108, 7, shell);
      g_gfx->fillCircle(140, 108, 7, shell);
      g_gfx->drawCircle(120, 136, 16, shell);
      g_gfx->fillRect(104, 120, 32, 16, face);
      break;
  }
}

void UI::drawPetView(const SystemStatus& status, const PetState& petState, bool wifiConnected) {
  drawAmbientBackground(petState.primaryColor, false);
  g_gfx->fillCircle(120, 122, 102, rgb(38, 32, 80));
  drawPetFace(petState);
  centerText(petState.detail, 198, 1, COLOR_LIGHTGREY);
  g_gfx->drawFastHLine(40, 212, 160, rgb(90, 78, 132));
  centerText((status.host.length() ? status.host : "-") + "  ·  " +
                 (wifiConnected ? String(status.cpu) + "% CPU" : "WiFi offline"),
             220, 1, COLOR_WHITE);
  drawViewDots(ScreenView::Pet, petState.primaryColor);
}

void UI::drawSystemView(const SystemStatus& status, const PetState& petState) {
  drawListMockBackground(petState.primaryColor, 4);

  const bool online = status.valid;
  const String ramText = online ? String(status.ramUsed, 1) + "G" : "--";
  drawSystemRow(38, online ? String(status.cpu) + "%" : "--", COLOR_CYAN, 0);
  drawSystemRow(78, online ? String(status.temp, 1) + "C" : "--", COLOR_ORANGE, 1);
  drawSystemRow(118, ramText, COLOR_GREEN, 2);
  drawSystemRow(158, online ? String(status.disk) + "%" : "--", COLOR_YELLOW, 3);

  drawTextCenteredInBox(online ? String(status.cpu) + "%" : "--", 88, 50, 57, 19, 2, COLOR_CYAN, 3, 0);
  drawTextCenteredInBox(online ? String(status.temp, 1) + "C" : "--", 89, 90, 55, 18, 2, COLOR_ORANGE, 3, 0);
  drawTextCenteredInBox(ramText, 86, 128, 59, 16, 2, COLOR_GREEN, 3, 0);
  drawTextCenteredInBox(online ? String(status.disk) + "%" : "--", 88, 163, 56, 19, 2, COLOR_YELLOW, 3, 0);

  drawViewDots(ScreenView::System, petState.primaryColor);
}

void UI::drawNetworkView(const SystemStatus& status, const PetState& petState, bool wifiConnected) {
  drawListMockBackground(petState.primaryColor, 5);

  const bool online = status.valid;
  drawNetworkRow(26, clipText(online ? status.host : "offline", 12), COLOR_WHITE, 0);
  drawNetworkRow(60, clipText(online && wifiConnected ? status.ip : "-", 12), COLOR_WHITE, 1);
  drawNetworkRow(94, clipText(online ? status.rx : "--", 8), COLOR_CYAN, 2);
  drawNetworkRow(128, clipText(online ? status.tx : "--", 8), COLOR_LAVENDER, 3);
  drawNetworkRow(162, clipText(online ? status.uptime : "--", 8), COLOR_WHITE, 4);

  drawTextLeftInBox(clipText(online ? status.host : "offline", 12), 86, 47, 57, 17, 1, COLOR_WHITE, 8, 3);
  drawTextLeftInBox(clipText(online && wifiConnected ? status.ip : "-", 12), 87, 80, 56, 15, 1, COLOR_WHITE, 8, 3);
  drawTextLeftInBox(clipText(online ? status.rx : "--", 8), 87, 113, 57, 14, 1, COLOR_CYAN, 8, 6);
  drawTextLeftInBox(clipText(online ? status.tx : "--", 8), 87, 143, 57, 14, 1, COLOR_LAVENDER, 8, 6);
  drawTextLeftInBox(clipText(online ? status.uptime : "--", 8), 87, 175, 55, 15, 1, COLOR_WHITE, 8, 6);

  drawViewDots(ScreenView::Network, petState.primaryColor);
}

void UI::drawDockerView(const SystemStatus& status, const PetState& petState) {
  drawDockerMockBackground(petState.primaryColor);

  const bool online = status.valid;
  const uint16_t downColor = !online ? COLOR_LIGHTGREY : (status.dockerExit > 0 ? COLOR_RED : COLOR_MINT);
  const uint16_t ringColor = !online ? COLOR_LIGHTGREY : (status.dockerExit > 0 ? COLOR_YELLOW : COLOR_CYAN);

  g_gfx->drawCircle(120, 84, 56, ringColor);
  drawTextCenteredInBox(online ? String(status.dockerUp) : "--", 82, 60, 76, 72, 4, COLOR_WHITE);
  drawTextCenteredInBox(online ? String(status.dockerExit) : "--", 94, 155, 52, 36, 3, downColor);

  drawViewDots(ScreenView::Docker, petState.primaryColor);
}

void UI::drawWeatherView(const WeatherStatus& weather, const PetState& petState, bool wifiConnected) {
  const uint16_t weatherAccent = COLOR_SKY;
  drawAmbientBackground(weatherAccent, false);

  if (!wifiConnected) {
    centerText("Sin WiFi", 104, 2, COLOR_WHITE);
    centerText("No hay clima", 134, 1, COLOR_LIGHTGREY);
    drawViewDots(ScreenView::Weather, weatherAccent);
    return;
  }

  if (!weather.valid) {
    centerText("Sin datos", 104, 2, COLOR_WHITE);
    centerText(weather.lastError.length() ? weather.lastError : "Error clima", 134, 1, COLOR_LIGHTGREY);
    drawViewDots(ScreenView::Weather, weatherAccent);
    return;
  }

  g_gfx->fillCircle(120, 82, 64, rgb(44, 40, 94));
  g_gfx->setTextColor(COLOR_WHITE);
  g_gfx->setTextSize(1);
  g_gfx->setCursor(86, 34);
  g_gfx->print(weather.locationLabel);
  g_gfx->setTextSize(4);
  g_gfx->setCursor(42, 94);
  g_gfx->print(String(weather.currentTemp, 0) + "C");
  g_gfx->setTextSize(1);
  g_gfx->setCursor(50, 132);
  g_gfx->print("H:");
  g_gfx->print(String(weather.forecast[0].tempMax, 0));
  g_gfx->print("  L:");
  g_gfx->print(String(weather.forecast[0].tempMin, 0));
  centerText(weatherLabel(weather.currentWeatherCode), 150, 1, COLOR_SOFT_WHITE);
  drawWeatherIcon(158, 78, weather.currentWeatherCode, true);
  g_gfx->drawFastHLine(46, 166, 148, rgb(62, 90, 120));

  const int16_t textX[3] = {52, 120, 188};
  for (int i = 1; i <= 3; ++i) {
    const WeatherForecastDay& day = weather.forecast[i];
    drawWeatherIcon(textX[i - 1], 184, day.weatherCode, false);
    centerTextInBox(shortDayLabel(day.date), textX[i - 1] - 24, 48, 206, 1, COLOR_SOFT_WHITE);
    centerTextInBox(String(day.tempMax, 0) + "/" + String(day.tempMin, 0), textX[i - 1] - 24, 48, 220, 1,
                    COLOR_WHITE);
  }
  drawViewDots(ScreenView::Weather, weatherAccent);
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

void UI::centerTextInBox(const String& text,
                         int16_t boxX,
                         int16_t boxWidth,
                         int16_t y,
                         uint8_t scale,
                         uint16_t color) {
  int16_t x1 = 0;
  int16_t y1 = 0;
  uint16_t w = 0;
  uint16_t h = 0;
  g_gfx->setTextSize(scale);
  g_gfx->getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  const int16_t x = boxX + (boxWidth - static_cast<int16_t>(w)) / 2;
  g_gfx->setCursor(x, y);
  g_gfx->setTextColor(color);
  g_gfx->print(text);
}

void UI::drawTextLeftInBox(const String& text,
                           int16_t x,
                           int16_t y,
                           int16_t w,
                           int16_t h,
                           uint8_t scale,
                           uint16_t color,
                           int16_t padX,
                           int16_t offsetY) {
  int16_t x1 = 0;
  int16_t y1 = 0;
  uint16_t textW = 0;
  uint16_t textH = 0;
  g_gfx->setTextSize(scale);
  g_gfx->getTextBounds(text, 0, 0, &x1, &y1, &textW, &textH);
  const int16_t drawY = y + (h - static_cast<int16_t>(textH)) / 2 + textH + offsetY;
  g_gfx->setCursor(x + padX, drawY);
  g_gfx->setTextColor(color);
  g_gfx->print(text);
}

void UI::drawTextCenteredInBox(const String& text,
                               int16_t x,
                               int16_t y,
                               int16_t w,
                               int16_t h,
                               uint8_t scale,
                               uint16_t color,
                               int16_t offsetX,
                               int16_t offsetY) {
  int16_t x1 = 0;
  int16_t y1 = 0;
  uint16_t textW = 0;
  uint16_t textH = 0;
  g_gfx->setTextSize(scale);
  g_gfx->getTextBounds(text, 0, 0, &x1, &y1, &textW, &textH);
  const int16_t drawX = x + (w - static_cast<int16_t>(textW)) / 2 + offsetX;
  const int16_t drawY = y + (h - static_cast<int16_t>(textH)) / 2 + textH + offsetY;
  g_gfx->setCursor(drawX, drawY);
  g_gfx->setTextColor(color);
  g_gfx->print(text);
}

void UI::drawTextRightInBox(const String& text,
                            int16_t x,
                            int16_t y,
                            int16_t w,
                            int16_t h,
                            uint8_t scale,
                            uint16_t color,
                            int16_t padX,
                            int16_t offsetY) {
  int16_t x1 = 0;
  int16_t y1 = 0;
  uint16_t textW = 0;
  uint16_t textH = 0;
  g_gfx->setTextSize(scale);
  g_gfx->getTextBounds(text, 0, 0, &x1, &y1, &textW, &textH);
  const int16_t drawX = x + w - static_cast<int16_t>(textW) - padX;
  const int16_t drawY = y + (h - static_cast<int16_t>(textH)) / 2 + textH + offsetY;
  g_gfx->setCursor(drawX, drawY);
  g_gfx->setTextColor(color);
  g_gfx->print(text);
}

String UI::shortDayLabel(const String& isoDate) {
  if (isoDate.length() < 10) {
    return isoDate;
  }

  const int year = isoDate.substring(0, 4).toInt();
  const int month = isoDate.substring(5, 7).toInt();
  const int day = isoDate.substring(8, 10).toInt();

  int adjustedYear = year;
  int adjustedMonth = month;
  if (adjustedMonth < 3) {
    adjustedMonth += 12;
    adjustedYear -= 1;
  }

  const int k = adjustedYear % 100;
  const int j = adjustedYear / 100;
  const int h = (day + (13 * (adjustedMonth + 1)) / 5 + k + k / 4 + j / 4 + (5 * j)) % 7;

  switch (h) {
    case 0:
      return "Sab";
    case 1:
      return "Dom";
    case 2:
      return "Lun";
    case 3:
      return "Mar";
    case 4:
      return "Mie";
    case 5:
      return "Jue";
    case 6:
    default:
      return "Vie";
  }
}

String UI::weatherLabel(int weatherCode) {
  if (weatherCode == 0) {
    return "Despejado";
  }
  if (weatherCode == 1 || weatherCode == 2) {
    return "Parcial";
  }
  if (weatherCode == 3) {
    return "Nublado";
  }
  if (weatherCode == 45 || weatherCode == 48) {
    return "Niebla";
  }
  if ((weatherCode >= 51 && weatherCode <= 67) || (weatherCode >= 80 && weatherCode <= 82)) {
    return "Lluvia";
  }
  if ((weatherCode >= 71 && weatherCode <= 77) || weatherCode == 85 || weatherCode == 86) {
    return "Nieve";
  }
  if (weatherCode >= 95) {
    return "Tormenta";
  }
  return "Variable";
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
