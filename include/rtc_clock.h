#pragma once

#include <Arduino.h>
#include <time.h>

class RTCClock {
 public:
  bool begin();
  bool isAvailable() const;
  bool read(tm& timeinfo) const;
  bool write(const tm& timeinfo) const;
  bool syncSystemTime() const;
  bool syncFromNtp(const char* timezone,
                   const char* primaryServer,
                   const char* secondaryServer,
                   uint32_t timeoutMs) const;

 private:
  bool readRegisters(uint8_t reg, uint8_t* buffer, size_t length) const;
  bool writeRegister(uint8_t reg, uint8_t value) const;

  bool available_ = false;
};
