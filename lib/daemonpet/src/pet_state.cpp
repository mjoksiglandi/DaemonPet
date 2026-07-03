#include "pet_state.h"

#include <Arduino_GFX_Library.h>
#include "config.h"

namespace {

uint16_t colorForMood(PetMood mood) {
  switch (mood) {
    case PetMood::Disconnected:
      return RGB565_RED;
    case PetMood::Overheated:
      return RGB565_ORANGE;
    case PetMood::Alert:
      return RGB565_YELLOW;
    case PetMood::Nervous:
      return RGB565_CYAN;
    case PetMood::Sleepy:
      return RGB565_BLUE;
    case PetMood::Normal:
    default:
      return RGB565_GREEN;
  }
}

}  // namespace

PetState evaluatePetState(const SystemStatus& status, bool wifiConnected, bool endpointHealthy) {
  PetState result;

  if (!wifiConnected || !endpointHealthy || !status.valid) {
    result.mood = PetMood::Disconnected;
    result.label = "Desconectada";
    result.detail = "Sin datos";
    result.primaryColor = colorForMood(result.mood);
    return result;
  }

  if (status.temp > AppConfig::TEMP_HOT_THRESHOLD) {
    result.mood = PetMood::Overheated;
    result.label = "Acalorada";
    result.detail = String(status.temp, 1) + " C";
  } else if (status.dockerExit > 0) {
    result.mood = PetMood::Alert;
    result.label = "Alerta";
    result.detail = String(status.dockerExit) + " caidos";
  } else if (status.cpu > AppConfig::CPU_HIGH_THRESHOLD) {
    result.mood = PetMood::Nervous;
    result.label = "Nerviosa";
    result.detail = "CPU " + String(status.cpu) + "%";
  } else if (status.cpu < AppConfig::CPU_SLEEPY_THRESHOLD) {
    result.mood = PetMood::Sleepy;
    result.label = "Somnolienta";
    result.detail = "CPU " + String(status.cpu) + "%";
  } else {
    result.mood = PetMood::Normal;
    result.label = "Normal";
    result.detail = "Todo OK";
  }

  result.primaryColor = colorForMood(result.mood);
  return result;
}
