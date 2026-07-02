#pragma once

#include <Arduino.h>
#include "system_status.h"

enum class PetMood {
  Disconnected,
  Overheated,
  Alert,
  Nervous,
  Sleepy,
  Normal
};

struct PetState {
  PetMood mood = PetMood::Disconnected;
  String label;
  String detail;
  uint16_t primaryColor = 0;
};

PetState evaluatePetState(const SystemStatus& status, bool wifiConnected, bool endpointHealthy);
