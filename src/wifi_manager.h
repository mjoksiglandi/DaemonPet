#pragma once

#include <Arduino.h>

class WiFiManager {
 public:
  void begin();
  void update();
  bool isConnected() const;
  String localIP() const;

 private:
  uint32_t lastAttemptMs_ = 0;
};
