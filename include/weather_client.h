#pragma once

#include <Arduino.h>
#include "weather_status.h"

class WeatherClient {
 public:
  bool fetch(WeatherStatus& weather, String& errorMessage);
};
