#include "rtc_clock.h"

#include <Wire.h>
#include <sys/time.h>
#include "display_config.h"

namespace {

constexpr uint8_t RTC_I2C_ADDRESS = 0x51;
constexpr uint8_t RTC_SECONDS_REGISTER = 0x04;

uint8_t bcdToDec(uint8_t value) {
  return static_cast<uint8_t>(((value >> 4) * 10) + (value & 0x0F));
}

uint8_t decToBcd(uint8_t value) {
  return static_cast<uint8_t>(((value / 10) << 4) | (value % 10));
}

}  // namespace

bool RTCClock::begin() {
  Wire.begin(DisplayConfig::PIN_I2C_SDA, DisplayConfig::PIN_I2C_SCL);

  uint8_t ramValue = 0;
  available_ = false;
  if (!readRegisters(0x03, &ramValue, 1)) {
    return false;
  }

  available_ = true;
  return true;
}

bool RTCClock::isAvailable() const {
  return available_;
}

bool RTCClock::read(tm& timeinfo) const {
  if (!available_) {
    return false;
  }

  uint8_t data[7] = {0};
  if (!readRegisters(RTC_SECONDS_REGISTER, data, sizeof(data))) {
    return false;
  }

  if ((data[0] & 0x80) != 0) {
    return false;
  }

  const int seconds = bcdToDec(data[0] & 0x7F);
  const int minutes = bcdToDec(data[1] & 0x7F);
  const int hours = bcdToDec(data[2] & 0x3F);
  const int day = bcdToDec(data[3] & 0x3F);
  const int weekday = data[4] & 0x07;
  const int month = bcdToDec(data[5] & 0x1F);
  const int year = 2000 + bcdToDec(data[6]);

  if (seconds > 59 || minutes > 59 || hours > 23 || day < 1 || day > 31 || month < 1 || month > 12) {
    return false;
  }

  timeinfo = {};
  timeinfo.tm_sec = seconds;
  timeinfo.tm_min = minutes;
  timeinfo.tm_hour = hours;
  timeinfo.tm_mday = day;
  timeinfo.tm_wday = weekday;
  timeinfo.tm_mon = month - 1;
  timeinfo.tm_year = year - 1900;
  timeinfo.tm_isdst = -1;
  return true;
}

bool RTCClock::write(const tm& timeinfo) const {
  if (!available_) {
    return false;
  }

  if (!writeRegister(0x04, decToBcd(static_cast<uint8_t>(timeinfo.tm_sec)))) {
    return false;
  }
  if (!writeRegister(0x05, decToBcd(static_cast<uint8_t>(timeinfo.tm_min)))) {
    return false;
  }
  if (!writeRegister(0x06, decToBcd(static_cast<uint8_t>(timeinfo.tm_hour)))) {
    return false;
  }
  if (!writeRegister(0x07, decToBcd(static_cast<uint8_t>(timeinfo.tm_mday)))) {
    return false;
  }
  if (!writeRegister(0x08, static_cast<uint8_t>(timeinfo.tm_wday & 0x07))) {
    return false;
  }
  if (!writeRegister(0x09, decToBcd(static_cast<uint8_t>(timeinfo.tm_mon + 1)))) {
    return false;
  }
  if (!writeRegister(0x0A, decToBcd(static_cast<uint8_t>((timeinfo.tm_year + 1900) % 100)))) {
    return false;
  }

  return true;
}

bool RTCClock::syncSystemTime() const {
  tm timeinfo = {};
  if (!read(timeinfo)) {
    return false;
  }

  time_t epoch = mktime(&timeinfo);
  if (epoch <= 0) {
    return false;
  }

  timeval now = {};
  now.tv_sec = epoch;
  return settimeofday(&now, nullptr) == 0;
}

bool RTCClock::syncFromNtp(const char* timezone,
                           const char* primaryServer,
                           const char* secondaryServer,
                           uint32_t timeoutMs) const {
  configTzTime(timezone, primaryServer, secondaryServer);

  tm timeinfo = {};
  const uint32_t start = millis();
  while (millis() - start < timeoutMs) {
    if (getLocalTime(&timeinfo, 250)) {
      return write(timeinfo);
    }
    delay(100);
  }

  return false;
}

bool RTCClock::readRegisters(uint8_t reg, uint8_t* buffer, size_t length) const {
  Wire.beginTransmission(RTC_I2C_ADDRESS);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) {
    return false;
  }

  if (Wire.requestFrom(static_cast<int>(RTC_I2C_ADDRESS), static_cast<int>(length), true) !=
      static_cast<int>(length)) {
    return false;
  }

  for (size_t i = 0; i < length; ++i) {
    buffer[i] = Wire.read();
  }
  return true;
}

bool RTCClock::writeRegister(uint8_t reg, uint8_t value) const {
  Wire.beginTransmission(RTC_I2C_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission(true) == 0;
}
